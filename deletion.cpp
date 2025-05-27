#include "deletion.h"
#include <algorithm>
#include <iostream>

void deleteRecord(Record recordToDelete, RTreeNode* node) {
    LeafNode* leafNode = dynamic_cast<LeafNode*>(node);
    if (leafNode) {
        // Αν βρισκόμαστε σε φύλλο κόμβο, αναζητούμε και διαγράφουμε την εγγραφή
        for (auto it = leafNode->records.begin(); it != leafNode->records.end(); ++it) {
            if (it->id == recordToDelete.id) {
                leafNode->records.erase(it);
                std::cout << "Successfully deleted Record (ID: " << recordToDelete.id << ")" << std::endl;
                // Save the R-tree after the deletion operation
                saveRtreeAfterDeletion(dynamic_cast<InternalNode*>(node->parent), "indexfile.txt");
                return;
            }
        }
        std::cout << "Record (ID: " << recordToDelete.id << ") not found for deletion." << std::endl;
    } else {
        // Διαφορετικά, εισάγουμε την εγγραφή στον αντίστοιχο εσωτερικό κόμβο
        InternalNode* internalNode = dynamic_cast<InternalNode*>(node);
        int chosenChild = chooseChildForDelete(recordToDelete, internalNode);
    
        deleteRecord(recordToDelete, internalNode->children[chosenChild]);
        
    }
}

int chooseChildForDelete(Record recordToDelete, InternalNode* internalNode) {
    // Εδώ μπορείτε να υλοποιήσετε τον αλγόριθμο επιλογής παιδιού για τη διαγραφή
    // που θα εξετάζει τις εγγραφές και τις επεκτάσεις των παιδιών του εσωτερικού κόμβου.

    // Εδώ χρησιμοποιούμε έναν απλό αλγόριθμο για να επιλέξουμε το παιδί
    // που περιέχει την εγγραφή που θέλουμε να διαγράψουμε.
    int chosenChild = -1;

    for (size_t i = 0; i < internalNode->children.size(); ++i) {
        LeafNode* childLeaf = dynamic_cast<LeafNode*>(internalNode->children[i]);
        if (childLeaf) {
            for (const auto& rec : childLeaf->records) {
                if (rec.id == recordToDelete.id) {
                    chosenChild = i;
                    break;
                }
            }
        }

        if (chosenChild >= 0) {
            break;
        }
    }

    if (chosenChild < 0) {
        std::cout << "Record (ID: " << recordToDelete.id << ") not found for deletion." << std::endl;
    }
   

    return chosenChild;

  
}