#include "r_tree.h"
#include <iostream>

// Function to calculate the minimum bound of an internal node
std::vector<double> InternalNode::getMinBound() const {
    std::vector<double> minBound(children[0]->getMinBound());
    for (size_t i = 1; i < children.size(); ++i) {
        std::vector<double> childMinBound = children[i]->getMinBound();
        for (size_t j = 0; j < minBound.size(); ++j) {
            minBound[j] = std::min(minBound[j], childMinBound[j]);
        }
    }
    return minBound;
}

// Function to calculate the maximum bound of an internal node
std::vector<double> InternalNode::getMaxBound() const {
    std::vector<double> maxBound(children[0]->getMaxBound());
    for (size_t i = 1; i < children.size(); ++i) {
        std::vector<double> childMaxBound = children[i]->getMaxBound();
        for (size_t j = 0; j < maxBound.size(); ++j) {
            maxBound[j] = std::max(maxBound[j], childMaxBound[j]);
        }
    }
    return maxBound;
}

// Function to calculate the minimum bound of a leaf node
std::vector<double> LeafNode::getMinBound() const {
    std::vector<double> minBound = {std::numeric_limits<double>::max(), std::numeric_limits<double>::max()};
    for (const Record& record : records) {
        minBound[0] = std::min(minBound[0], record.latitude);
        minBound[1] = std::min(minBound[1], record.longitude);
    }
    return minBound;
}

// Function to calculate the maximum bound of a leaf node
std::vector<double> LeafNode::getMaxBound() const {
    std::vector<double> maxBound = {std::numeric_limits<double>::min(), std::numeric_limits<double>::min()};
    for (const Record& record : records) {
        maxBound[0] = std::max(maxBound[0], record.latitude);
        maxBound[1] = std::max(maxBound[1], record.longitude);
    }
    return maxBound;
}


// Function to read records from the datafile
int readRecordsFromFile(const std::string& filename, std::vector<Record>& records) {
    std::ifstream infile(filename, std::ios::binary);
    if (!infile) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return 0;
    }

    // Find the number of records in the file
    infile.seekg(0, std::ios::end);
    int fileSize = infile.tellg();
    infile.seekg(0, std::ios::beg);
    int numRecords = fileSize / sizeof(Record);

      std::cout << "File size: " << fileSize << " bytes" <<
     std::endl;
    std::cout << "Number of records calculated: " << numRecords <<
     std::endl;

    // Resize the vector to hold all records
    records.resize(numRecords);

    // Read the datafile block by block
    char buffer[BLOCK_SIZE];
    int bytesRead = 0;
    int recordIndex = 0;

    while (infile.read(buffer, BLOCK_SIZE)) {
        int numRecordsInBlock = BLOCK_SIZE / sizeof(Record);
        Record* recordPtr = reinterpret_cast<Record*>(buffer);

        for (int i = 0; i < numRecordsInBlock; ++i) {
            records[recordIndex++] = recordPtr[i];
        }

        bytesRead += BLOCK_SIZE;
    }

    // Handle the last block if it's smaller than BLOCK_SIZE
    int remainingRecords = (fileSize - bytesRead) / sizeof(Record);
    Record* recordPtr = reinterpret_cast<Record*>(buffer);
    for (int i = 0; i < remainingRecords; ++i) {
        records[recordIndex++] = recordPtr[i];
    }

    infile.close();
    std::cout<<"Read" << numRecords <<" records from the file."
      <<std::endl;
    return numRecords;
}

// Function to group records based on latitude and longitude
std::vector<std::vector<Record>> groupRecords(const std::vector<Record>& records, int numGroups) {
    // Initialize the vector of groups
    std::vector<std::vector<Record>> groups(numGroups);

    // Calculate the number of records per group
    int recordsPerGroup = records.size() / numGroups;

    // Distribute the records into groups
    int recordIndex = 0;
    for (int i = 0; i < numGroups; ++i) {
        for (int j = 0; j < recordsPerGroup; ++j) {
            groups[i].push_back(records[recordIndex++]);
        }
    }

    // Handle the remaining records (if any)
    while (recordIndex < records.size()) {
        groups.back().push_back(records[recordIndex++]);
    }
    std::cout<<"Grouped"<<records.size()<<"records into"<<numGroups<<"groups."<<std::endl;
    return groups;
}



// Function to build R*-tree leaves for each group
std::vector<LeafNode*> buildRTreeLeaves(const std::vector<std::vector<Record>>& groupedRecords) {
    std::vector<LeafNode*> leaves;

    for (const auto& group : groupedRecords) {
        LeafNode* leaf = new LeafNode;
        leaf->records = group;
        leaves.push_back(leaf);
     
     
    }
  std::cout<<"Built R*-tree leaves for"<<leaves.size()<<"groups."
   <<std::endl;
    return leaves;
}

// Function to build R*-tree internal nodes from leaves
InternalNode* buildRTreeInternalNodes(const std::vector<RTreeNode*>& leaves) {
    // If there is only one leaf, we return it as the root
    if (leaves.size() == 1) {
        return dynamic_cast<InternalNode*>(leaves[0]);
    }

    // Calculate the number of children per internal node
    int childrenPerNode = 2; // You can adjust this based on your requirements

    // Create a vector to store the current level of nodes (leaves or internal nodes)
    std::vector<RTreeNode*> currentLevel = leaves;

    // Loop until we have only one node left (the root)
    while (currentLevel.size() > 1) {
        // Create a vector to store the next level of nodes
        std::vector<RTreeNode*> nextLevel;

        // Process the current level of nodes and group them into internal nodes
        for (size_t i = 0; i < currentLevel.size(); i += childrenPerNode) {
            // Create a new internal node
            InternalNode* internalNode = new InternalNode;

            // Add children to the internal node
            for (size_t j = 0; j < childrenPerNode; ++j) {
                size_t childIndex = i + j;
                if (childIndex < currentLevel.size()) {
                    internalNode->children.push_back(currentLevel[childIndex]);
                }
            }

            // Add the internal node to the next level
            nextLevel.push_back(internalNode);
        }

        // Set the next level as the current level for the next iteration
        currentLevel = nextLevel;
    }
   std::cout<<"Built R*-tree internal nodes." << std::endl;

    // Return the root node
    return dynamic_cast<InternalNode*>(currentLevel[0]);
}

// Function to release memory allocated for R*-tree nodes (leaves and internal nodes)
void releaseRTreeNodes(RTreeNode* node) {
    if (!node) {
        return;
    }

    // Check if the node is an internal node or a leaf node
    InternalNode* internalNode = dynamic_cast<InternalNode*>(node);
    if (internalNode) {
        for (auto child : internalNode->children) {
            releaseRTreeNodes(child);
        }
    } else {
        LeafNode* leafNode = dynamic_cast<LeafNode*>(node);
        if (leafNode) {
            delete leafNode;
        }
    }
}

// Function to save an R-tree node to a file
void saveRtreeNode(RTreeNode* node, std::ofstream& outputFile) {
    if (!node)
        return;

    // Check if the node is an internal node or a leaf node
    InternalNode* internalNode = dynamic_cast<InternalNode*>(node);
    if (internalNode) {
        // Mark the node as internal in the file
        int type = 0;
        outputFile.write(reinterpret_cast<char*>(&type), sizeof(int));

        // Save the number of children
        int numChildren = internalNode->children.size();
        outputFile.write(reinterpret_cast<char*>(&numChildren), sizeof(int));

        // Recursively save each child
        for (RTreeNode* child : internalNode->children) {
            saveRtreeNode(child, outputFile);
        }
    } else {
        LeafNode* leafNode = dynamic_cast<LeafNode*>(node);
        if (leafNode) {
            // Mark the node as a leaf in the file
            int type = 1;
            outputFile.write(reinterpret_cast<char*>(&type), sizeof(int));

            // Save the number of records in the leaf node
            int numRecords = leafNode->records.size();
            outputFile.write(reinterpret_cast<char*>(&numRecords), sizeof(int));

            // Save the records
            for (const Record& record : leafNode->records) {
                outputFile.write(reinterpret_cast<const char*>(&record), sizeof(Record));
            }
        }
    }
}

// Function to save the R-tree to a file
void saveRtreeToFile(InternalNode* root, const std::string& filename) {
    std::ofstream outputFile(filename, std::ios::binary);
    if (!outputFile) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    saveRtreeNode(root, outputFile);
    outputFile.close();
}

// Function to load an R-tree node from a file
RTreeNode* loadRtreeNode(std::ifstream& inputFile) {
    int type;
    inputFile.read(reinterpret_cast<char*>(&type), sizeof(int));

    RTreeNode* node = nullptr;
    if (type == 0) {
        // Internal node
        node = new InternalNode;
        int numChildren;
        inputFile.read(reinterpret_cast<char*>(&numChildren), sizeof(int));

        for (int i = 0; i < numChildren; ++i) {
            RTreeNode* child = loadRtreeNode(inputFile);
            if (child) {
                child->parent = dynamic_cast<InternalNode*>(node);
                dynamic_cast<InternalNode*>(node)->children.push_back(child);
            }
        }
    } else if (type == 1) {
        // Leaf node
        node = new LeafNode;
        int numRecords;
        inputFile.read(reinterpret_cast<char*>(&numRecords), sizeof(int));

        for (int i = 0; i < numRecords; ++i) {
            Record record;
            inputFile.read(reinterpret_cast<char*>(&record), sizeof(Record));
            dynamic_cast<LeafNode*>(node)->records.push_back(record);
        }
    }

    return node;
}

// Function to load the R-tree from a file
InternalNode* loadRtreeFromFile(const std::string& filename) {
    std::ifstream inputFile(filename, std::ios::binary);
    if (!inputFile) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return nullptr;
    }

    InternalNode* root = dynamic_cast<InternalNode*>(loadRtreeNode(inputFile));
    inputFile.close();
    return root;
}

// Function to save the R-tree after the deletion operation
void saveRtreeAfterDeletion(InternalNode* root, const std::string& filename) {
    // Save the R-tree to the file
    saveRtreeToFile(root, filename);
    std::cout << "R-tree saved to file after deletion." << std::endl;
}


LeafNode::~LeafNode() {
    // Release any resources associated with the LeafNode, if needed
    // For this example, we don't need to release anything since the records vector will be automatically deallocated.
}

InternalNode::~InternalNode() {
    // Release any resources associated with the InternalNode, if needed
    // For this example, we don't need to release anything since the children vector will be automatically deallocated.
}