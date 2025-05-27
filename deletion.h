#ifndef DELETION_H
#define DELETION_H
#include "r_tree.h"
#include <vector>
#include <iostream>

// Function to delete a record from the R-tree
void deleteRecord(Record recordToDelete, RTreeNode* node);
int chooseChildForDelete(Record recordToDelete, InternalNode* internalNode);

#endif // DELETION_H