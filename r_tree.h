#ifndef R_TREE_H
#define R_TREE_H

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <limits>

// Define the block size in bytes (32KB)
const int BLOCK_SIZE = 32 * 1024;

// Forward declarations
class InternalNode;
class LeafNode;

// Define the structure of a record
struct Record {
    int id;
    char name[100]; // Assuming the name has a maximum length of 100 characters
    double latitude;
    double longitude;
};

// Base class for R-tree nodes
class RTreeNode {
public:
    InternalNode* parent; // Add parent as a member of RTreeNode

    // Constructor
    RTreeNode() : parent(nullptr) {} // Initialize parent to nullptr

    // Destructor (made virtual for proper cleanup of derived classes)
    virtual ~RTreeNode() = default;

     // Εικονικές συναρτήσεις για την ανάκτηση των ελάχιστων και μέγιστων τιμών των MBR
    virtual std::vector<double> getMinBound() const = 0;
    virtual std::vector<double> getMaxBound() const = 0;
};

// Class for internal (non-leaf) nodes of the R-tree
class InternalNode : public RTreeNode {
public:
    std::vector<RTreeNode*> children; // Pointers to child nodes

    // Constructor
    InternalNode() : RTreeNode() {}

    // Destructor
    ~InternalNode() override;

     // Add methods to calculate the MBR for internal node
    std::vector<double> getMinBound() const override;
    std::vector<double> getMaxBound() const override;

     
};

// Class for leaf nodes of the R-tree
class LeafNode : public RTreeNode {
public:
    std::vector<Record> records; // Data entries (records)

    // Constructor
    LeafNode() : RTreeNode() {}

    // Destructor
    ~LeafNode() override;

     // Add methods to calculate the MBR for leaf node
    std::vector<double> getMinBound() const override;
    std::vector<double> getMaxBound() const override;

};

// Function to read records from the datafile
int readRecordsFromFile(const std::string& filename, std::vector<Record>& records);

// Function to group records based on latitude and longitude
std::vector<std::vector<Record>> groupRecords(const std::vector<Record>& records, int numGroups);

// Function to build R*-tree leaves for each group
std::vector<LeafNode*> buildRTreeLeaves(const std::vector<std::vector<Record>>& groupedRecords);

// Function to build R*-tree internal nodes from leaves
InternalNode* buildRTreeInternalNodes(const std::vector<RTreeNode*>& leaves);

// Function to release memory allocated for R*-tree nodes (leaves and internal nodes)
void releaseRTreeNodes(RTreeNode* node);

// Function to save an R-tree node to a file
void saveRtreeNode(RTreeNode* node, std::ofstream& outputFile);

// Function to save the R-tree to a file
void saveRtreeToFile(InternalNode* root, const std::string& filename);

// Function to load an R-tree node from a file
RTreeNode* loadRtreeNode(std::ifstream& inputFile);

// Function to load the R-tree from a file
InternalNode* loadRtreeFromFile(const std::string& filename);

void saveRtreeAfterDeletion(InternalNode* root, const std::string& filename);
#endif // R_TREE_H