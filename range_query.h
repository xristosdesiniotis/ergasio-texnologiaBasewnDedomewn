#ifndef RANGE_QUERY_H
#define RANGE_QUERY_H

#include "r_tree.h"
#include <vector>

// Function to perform range query on the R-tree
std::vector<Record> rangeQuery(const std::vector<double>& minBound, const std::vector<double>& maxBound, RTreeNode* node, size_t dimensions);

#endif // RANGE_QUERY_H
