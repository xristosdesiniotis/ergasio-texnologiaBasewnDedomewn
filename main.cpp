#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "r_tree.h"
#include "insertion.h"
#include "deletion.h"
#include <cstring>
#include <limits>
#include "range_query.h"
#include "knn_query.h"
#include "skyline_query.h"
#include <chrono>

// Helper function to print records
void printRecords(const std::vector<Record>& records) {
    std::cout << "Found " << records.size() << " records:" << std::endl;
    for (const Record& record : records) {
        std::cout << "ID: " << record.id << ", Name: " << record.name << ", Latitude: " << record.latitude << ", Longitude: " << record.longitude << std::endl;
    }
    std::cout << std::endl;
}

// Function to get the range query parameters from the user
std::vector<std::vector<double>> getRangeQueryParameters(size_t dimensions) {
    std::vector<std::vector<double>> bounds(2, std::vector<double>(dimensions));

    std::cout << "Enter the range query parameters:" << std::endl;
    for (size_t i = 0; i < dimensions; ++i) {
        std::cout << "Dimension " << i + 1 << ":" << std::endl;
        std::cout << "Minimum bound: ";
        std::cin >> bounds[0][i];
        std::cout << "Maximum bound: ";
        std::cin >> bounds[1][i];
    }

    return bounds;
}


// Function to save the R-tree to a file
void saveRtreeToFile(InternalNode* root, const std::string& filename);


// Function to load the R-tree from a file
InternalNode* loadRtreeFromFile(const std::string& filename);


  

int main() {
  // Προσθέτουμε τον χρονόμετρο
    auto startTime = std::chrono::high_resolution_clock::now();
    // Assuming your datafile is named "datafile.dat"
    std::string datafile = "datafile.dat";

    std::vector<Record> records;
    int numRecords = readRecordsFromFile(datafile, records);

    // Now you have the records in the 'records' vector with 'numRecords' elements for further processing

    // Group the records based on latitude and longitude into 5 groups (for example)
    int numGroups = 5;
    std::vector<std::vector<Record>> groupedRecords = groupRecords(records, numGroups);


    // Build R*-tree leaves for each group
    std::vector<LeafNode*> leaves = buildRTreeLeaves(groupedRecords);

    // Build R*-tree internal nodes from the leaves
    std::vector<RTreeNode*> tempLeaves;
    for (LeafNode* leaf : leaves) {
        tempLeaves.push_back(leaf);
    }
    InternalNode* root = buildRTreeInternalNodes(tempLeaves);

    //Save the R-tree to the file "indexfile.txt"
    saveRtreeToFile(root, "indexfile.txt");

    // You can also load the R-tree from the file if needed
    // InternalNode* loadedRoot = loadRtreeFromFile("indexfile.txt");
  
   while (true) {
        std::cout << "Menu:" << std::endl;
        std::cout << "1. Insert Record" << std::endl;
        std::cout << "2. Delete Record" << std::endl;
        std::cout << "3. Range Query" << std::endl;
        std::cout << "4. k-nn Query" << std::endl;
        std::cout << "5. Skyline Query" << std::endl;
        std::cout << "6. Exit" << std::endl;
        std::cout << "Enter your choice: ";

        int choice;
        std::cin >> choice;

        if (choice == 1) {
            Record newRecord;
            std::cout << "Enter ID: ";
            std::cin >> newRecord.id;

            std::cout << "Enter Name: ";
            std::cin.ignore(); // Ignore the newline character left in the input stream
            std::cin.getline(newRecord.name, 100);

            std::cout << "Enter Latitude: ";
            std::cin >> newRecord.latitude;

            std::cout << "Enter Longitude: ";
            std::cin >> newRecord.longitude;

            insertRecord(newRecord, root);
          
            writeRecordToFile("indexfile.txt", "\n" + 
 std::to_string(newRecord.id) + " , "  + newRecord.name  + " , " +
 std::to_string(newRecord.latitude) + " , " + std::to_string(newRecord.longitude));
        } else if (choice == 2) {
            int idToDelete;
            std::cout << "Enter ID of the record to delete: ";
            std::cin >> idToDelete;

            Record recordToDelete;
            recordToDelete.id = idToDelete;

             LeafNode* leafNodeToDelete = nullptr;
    for (LeafNode* leaf : leaves) {
        for (const Record& rec : leaf->records) {
            if (rec.id == recordToDelete.id) {
                leafNodeToDelete = leaf;
                break;
            }
        }
        if (leafNodeToDelete) {
            break;
        }
    }

    if (leafNodeToDelete) {
        deleteRecord(recordToDelete, leafNodeToDelete);
        std::cout << "Successfully deleted Record (ID: " << recordToDelete.id << ")" << std::endl;
    } else {
        // Εδώ μπορείτε να προσθέσετε κάποιο μήνυμα για την περίπτωση που η εγγραφή δεν βρίσκεται στο R-tree
        std::cout << "Record (ID: " << recordToDelete.id << ") not found for deletion." << std::endl;
    }
          // Save the R-tree to the file after each deletion
            saveRtreeToFile(root, "indexfile.txt");
        } else if(choice==3)
        {
            // Εκτέλεση ερωτήματος εύρους (range query)

            std::vector<double> minBound, maxBound;
            size_t dimensions; // Ο αριθμός των διαστάσεων του R-tree

            std::cout << "Enter the number of dimensions for the range query: ";
            std::cin >> dimensions;

            std::vector<std::vector<double>> bounds = getRangeQueryParameters(dimensions);
    minBound = bounds[0];
    maxBound = bounds[1];

           // Measure execution time for range query
            auto start = std::chrono::high_resolution_clock::now();
            std::vector<Record> queryResult = rangeQuery(minBound, maxBound, root, dimensions);
           auto end = std::chrono::high_resolution_clock::now();
     auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    double timeTaken = duration.count() / 1000.0; // Convert microseconds to milliseconds
          
            printRecords(queryResult);
           std::cout << "Time taken for range query: " << timeTaken << " milliseconds" << std::endl;
        } else if(choice == 4){
            // k-nn Query
            std::vector<double> queryPoint;
            int k;

            std::cout << "Enter the number of dimensions for the k-nn query: ";
            size_t dimensions;
            std::cin >> dimensions;

            std::cout << "Enter the k value for the k-nn query: ";
            std::cin >> k;

            std::cout << "Enter the query point coordinates:" << std::endl;
            queryPoint.resize(dimensions);
            for (size_t i = 0; i < dimensions; ++i) {
                std::cout << "Dimension " << i + 1 << ": ";
                std::cin >> queryPoint[i];
            }
            auto start = std::chrono::high_resolution_clock::now();
            std::vector<Record> knnResult = knnQuery(queryPoint, k, root, dimensions);
              auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        double timeTaken = duration.count() / 1000.0; // Μετατροπή microseconds σε milliseconds

            printRecords(knnResult);
            std::cout << "Time taken for k-nn query: " << timeTaken << " milliseconds" << std::endl;
        } else if(choice ==5)
        {
           // Εκτέλεση ερωτήματος κορυφογραμμής (skyline query)
 
            std::vector<double> queryPoint;
            size_t dimensions; // Ο αριθμός των διαστάσεων του R-tree

            std::cout << "Enter the number of dimensions for the skyline query: ";
            std::cin >> dimensions;

            std::cout << "Enter the query point coordinates:" << std::endl;
            queryPoint.resize(dimensions);
            for (size_t i = 0; i < dimensions; ++i) {
                std::cout << "Dimension " << i + 1 << ": ";
                std::cin >> queryPoint[i];
            }
               // Προσθήκη μεταβλητής για την αποθήκευση του χρόνου εκτέλεσης
        auto start = std::chrono::high_resolution_clock::now();
            std::vector<Record> skylineResult = skylineQuery(queryPoint, root, dimensions);
          auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        double timeTaken = duration.count() / 1000.0; // Μετατροπή microseconds σε milliseconds

            printRecords(skylineResult);
            std::cout << "Time taken for skyline query: " << timeTaken << " milliseconds" << std::endl;
        }
        else if (choice == 6) {
            break;
        } else {
            std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }
 auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    std::cout << "Χρόνος εκτέλεσης: " << duration.count() << " ms" << std::endl;
   releaseRTreeNodes(root);
   
  for (LeafNode* leaf : leaves) {
    delete leaf;
  }
 leaves.clear();

    return 0;
}
