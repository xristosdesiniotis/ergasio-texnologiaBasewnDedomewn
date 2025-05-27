#ifndef INSERTION_H
#define INSERTION_H
#include "r_tree.h"
#include <vector>
#include <iostream>
#include <fstream>


void insertRecord(Record record, InternalNode* node);
int chooseChildForInsert(Record record, InternalNode* internalNode);
double calculateDistance(const Record& record1, const Record& record2);
void splitLeafNode(LeafNode* leafNode);
void splitInternalNode(InternalNode* internalNode);

// Συνάρτηση για εγγραφή αποτελέσματος σε αρχείο
void writeRecordToFile(const std::string& filename, const std::string& message);

// Function to calculate the enlargement of a leaf node if a new record is inserted
double calculateEnlargement(LeafNode* leafNode, Record record);

#endif // INSERTION_H