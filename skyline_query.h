#ifndef SKYLINE_QUERY_H
#define SKYLINE_QUERY_H

#include <vector>
#include "r_tree.h"
#include "range_query.h"

// Function to perform skyline query
std::vector<Record> skylineQuery(const std::vector<double>& queryPoint, InternalNode* root, size_t dimensions);

#endif // SKYLINE_QUERY_H
