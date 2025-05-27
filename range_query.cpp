#include "range_query.h"
#include <iostream>
#include <vector>

// Function to check if two MBRs intersect
bool doMBRsIntersect(const std::vector<double>& minBound1, const std::vector<double>& maxBound1,
                     const std::vector<double>& minBound2, const std::vector<double>& maxBound2) {
    for (size_t i = 0; i < minBound1.size(); ++i) {
        if (minBound1[i] > maxBound2[i] || maxBound1[i] < minBound2[i]) {
            return false;
        }
    }
    return true;
}


// Helper function to check if a point lies within the range
bool isWithinRange(const std::vector<double>& point, const std::vector<double>& minBound, const std::vector<double>& maxBound) {
    for (size_t i = 0; i < point.size(); ++i) {
        if (point[i] < minBound[i] || point[i] > maxBound[i]) {
            return false;
        }
    }
    return true;
}

// Function to perform range query on the R-tree
std::vector<Record> rangeQuery(const std::vector<double>& minBound, const std::vector<double>& maxBound, RTreeNode* node, size_t dimensions) {
    std::vector<Record> result;

    if (dynamic_cast<LeafNode*>(node)) {
        LeafNode* leafNode = static_cast<LeafNode*>(node);
        for (const Record& record : leafNode->records) {
            // Check if the record lies within the given range
            if (isWithinRange({record.latitude, record.longitude}, minBound, maxBound)) {
                result.push_back(record);
            }
        }
    } else if (dynamic_cast<InternalNode*>(node)) {
  InternalNode* internalNode = static_cast<InternalNode*>(node);
        for (size_t i = 0; i < internalNode->children.size(); ++i) {
            // Here, you should call the appropriate functions to get the minBound and maxBound of the MBR for each child
            std::vector<double> childMinBound = internalNode->children[i]->getMinBound();
            std::vector<double> childMaxBound = internalNode->children[i]->getMaxBound();
            
            if (doMBRsIntersect(childMinBound, childMaxBound, minBound, maxBound)) {
                // If the MBR of the child node intersects with the range, recursively search within that child
                std::vector<Record> childResult = rangeQuery(minBound, maxBound, internalNode->children[i], dimensions);
                result.insert(result.end(), childResult.begin(), childResult.end());
            }
        }
    }

    return result;
}