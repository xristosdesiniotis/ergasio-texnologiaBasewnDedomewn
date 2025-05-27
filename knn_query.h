#ifndef KNN_QUERY_H
#define KNN_QUERY_H

#include <vector>
#include "r_tree.h"

// Function to perform k-nn query on the R-tree
std::vector<Record> knnQuery(const std::vector<double>& queryPoint, int k, RTreeNode* node, size_t dimensions);

#endif // KNN_QUERY_H
