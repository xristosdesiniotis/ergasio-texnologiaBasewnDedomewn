#include "knn_query.h"
#include <cmath> // For distance calculation
#include <set> // For maintaining k-nearest neighbors
#include <limits> // For numeric_limits

// Helper function to calculate the distance between two points
double calculateDistance(const std::vector<double>& point1, const std::vector<double>& point2) {
    double distance = 0.0;
    for (size_t i = 0; i < point1.size(); ++i) {
        double diff = point1[i] - point2[i];
        distance += diff * diff;
    }
    return std::sqrt(distance);
}

// Function to perform k-nn query on the R-tree
std::vector<Record> knnQuery(const std::vector<double>& queryPoint, int k, RTreeNode* node, size_t dimensions) {
    // Using a set to maintain the k-nearest neighbors in ascending order of distance
    std::set<std::pair<double, const Record*>> nearestNeighbors;

    if (dynamic_cast<LeafNode*>(node)) {
        LeafNode* leafNode = static_cast<LeafNode*>(node);
        for (const Record& record : leafNode->records) {
            double distance = calculateDistance({record.latitude, record.longitude}, queryPoint);
            nearestNeighbors.insert({distance, &record});
            if (nearestNeighbors.size() > static_cast<size_t>(k)) {
                nearestNeighbors.erase(std::prev(nearestNeighbors.end()));
            }
        }
    } else if (dynamic_cast<InternalNode*>(node)) {
        InternalNode* internalNode = static_cast<InternalNode*>(node);
        std::vector<double> minBound = internalNode->getMinBound();
        std::vector<double> maxBound = internalNode->getMaxBound();

        // Calculate the distance of the query point to the MBR of the internal node
        double distanceToNodeMBR = calculateDistance(queryPoint, minBound);
        // Loop through each child node of the internal node
        for (size_t i = 0; i < internalNode->children.size(); ++i) {
            // Check if the distance of the query point to the MBR of the child node
            // is less than or equal to the distance to the MBR of the internal node
            if (calculateDistance(queryPoint, internalNode->children[i]->getMinBound()) <= distanceToNodeMBR ||
                calculateDistance(queryPoint, internalNode->children[i]->getMaxBound()) <= distanceToNodeMBR) {
                // If true, the child node's MBR is closer to the query point, so
                // we search within that child node recursively
                std::vector<Record> childResult = knnQuery(queryPoint, k, internalNode->children[i], dimensions);
                // Update the nearest neighbors set with the results from the child node
                for (const Record& record : childResult) {
                    double distance = calculateDistance({record.latitude, record.longitude}, queryPoint);
                    nearestNeighbors.insert({distance, &record});
                    if (nearestNeighbors.size() > static_cast<size_t>(k)) {
                        nearestNeighbors.erase(std::prev(nearestNeighbors.end()));
                    }
                }
            }
        }
    }

    // Extract the k-nearest neighbors from the set and return as a vector
    std::vector<Record> result;
    for (const auto& neighbor : nearestNeighbors) {
        result.push_back(*neighbor.second);
    }
    return result;
}