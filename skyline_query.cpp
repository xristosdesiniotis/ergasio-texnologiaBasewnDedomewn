#include "skyline_query.h"
#include <vector>
#include <algorithm>

// Function to check if a record is dominated by a query point
bool isDominated(const Record& record, const std::vector<double>& queryPoint) {
    for (size_t i = 0; i < queryPoint.size(); ++i) {
        if (record.latitude <= queryPoint[i] || record.longitude <= queryPoint[i]) {
            return false;
        }
    }
    return true;
}

// Function to perform skyline query
std::vector<Record> skylineQuery(const std::vector<double>& queryPoint, InternalNode* root, size_t dimensions) {
    std::vector<Record> skyline;

    // Perform a range query to find all records within the query region
    std::vector<double> minBound(dimensions, std::numeric_limits<double>::lowest());
    std::vector<double> maxBound(dimensions, std::numeric_limits<double>::max());
    std::vector<Record> recordsInRange = rangeQuery(minBound, maxBound, root, dimensions);

    // Iterate through all records in the range
    for (const Record& record : recordsInRange) {
        // Check if the record is dominated by the query point
        if (!isDominated(record, queryPoint)) {
            // If not dominated, add it to the skyline
            skyline.push_back(record);
        }
    }

    return skyline;
}