#include "insertion.h"
#include <limits>
#include <cmath>
#include <fstream>
#include <iostream>

const int MAX_CHILDREN = 4; // Μέγιστος αριθμός παιδιών για κάθε εσωτερικό κόμβο
const int MAX_LEAF_SIZE = 3; // Μέγιστος αριθμός εγγραφών για κάθε φύλλο κόμβο

// Εδώ υλοποιούμε τις συναρτήσεις που περιλαμβάνουν τον αλγόριθμο για την εισαγωγή εγγραφών και τις λοιπές απαραίτητες συναρτήσεις
void insertRecord(Record record, InternalNode* node) {

   // Εμφανίζουμε τα στοιχεία της εγγραφής που πρόκειται να εισαχθεί
    std::cout << "Inserting Record (ID: " << record.id << ", Name: " << record.name << ", Latitude: " << record.latitude << ", Longitude: " << record.longitude << ")..." << std::endl;

   // Αν ο κόμβος είναι φύλλο, εισάγουμε την εγγραφή σε αυτόν
    LeafNode* leafNode = dynamic_cast<LeafNode*>(node);
    if (leafNode) {
        leafNode->records.push_back(record);

        // Αν ο αριθμός των εγγραφών στο φύλλο υπερβαίνει το μέγιστο, διαιρούμε το φύλλο
        if (leafNode->records.size() > MAX_LEAF_SIZE) {
            std::cout << "Splitting Leaf Node..." << std::endl;
            splitLeafNode(leafNode);
        }
    }
    // Αλλιώς, εισάγουμε την εγγραφή σε ένα εσωτερικό κόμβο
    else {
        InternalNode* internalNode = dynamic_cast<InternalNode*>(node);

        // Επιλέγουμε το παιδί που θα εισαγάγουμε την εγγραφή
        int chosenChild = chooseChildForInsert(record, internalNode);
      std::cout << "Inserting record in child " << chosenChild << " of Internal Node..." << std::endl;
        InternalNode* childInternalNode = dynamic_cast<InternalNode*>(internalNode->children[chosenChild]);
if (childInternalNode) {
    insertRecord(record, childInternalNode);
}


        // Αν το παιδί μας υπερβαίνει το μέγιστο, διαιρούμε τον εσωτερικό κόμβο
        if (internalNode->children.size() > MAX_CHILDREN) {
            std::cout << "Splitting Internal Node..." << std::endl;
            splitInternalNode(internalNode);
        }
    }
}

int chooseChildForInsert(Record record, InternalNode* internalNode) {
    // Ελέγχουμε το παιδί με την ελάχιστη επέκταση περιοχής που θα προκληθεί αν εισάγουμε τη νέα εγγραφή σε κάθε παιδί
    double minEnlargement = std::numeric_limits<double>::max();
    int chosenChild = 0;

    for (size_t i = 0; i < internalNode->children.size(); ++i) {
        LeafNode* childLeaf = dynamic_cast<LeafNode*>(internalNode->children[i]);
        if (childLeaf) {
            // Υπολογίζουμε τη νέα περιοχή επέκτασης που θα προκληθεί αν εισάγουμε τη νέα εγγραφή στο παιδί
            double enlargement = calculateEnlargement(childLeaf, record);
            if (enlargement < minEnlargement) {
                minEnlargement = enlargement;
                chosenChild = i;
            }
        }
    }

    return chosenChild;
}

double calculateEnlargement(LeafNode* leafNode, Record record) {
    // Υπολογίζουμε τη νέα περιοχή επέκτασης που θα προκληθεί αν εισάγουμε τη νέα εγγραφή στο φύλλο κόμβο
    double minX = leafNode->records[0].latitude;
    double maxX = leafNode->records[0].latitude;
    double minY = leafNode->records[0].longitude;
    double maxY = leafNode->records[0].longitude;

    for (const auto& rec : leafNode->records) {
        minX = std::min(minX, rec.latitude);
        maxX = std::max(maxX, rec.latitude);
        minY = std::min(minY, rec.longitude);
        maxY = std::max(maxY, rec.longitude);
    }

    double originalArea = (maxX - minX) * (maxY - minY);

    double newMinX = std::min(minX, record.latitude);
    double newMaxX = std::max(maxX, record.latitude);
    double newMinY = std::min(minY, record.longitude);
    double newMaxY = std::max(maxY, record.longitude);

    double newArea = (newMaxX - newMinX) * (newMaxY - newMinY);
    double enlargement = newArea - originalArea;

    return enlargement;
}

double calculateDistance(const Record& record1, const Record& record2) {
    double dx = record1.latitude - record2.latitude;
    double dy = record1.longitude - record2.longitude;
   // Εμφανίζουμε την απόσταση μεταξύ των δύο εγγραφών
    std::cout << "Distance between Record (ID: " << record1.id << ", Name: " << record1.name << ") and Record (ID: " << record2.id << ", Name: " << record2.name << "): " << sqrt(dx * dx + dy * dy) << std::endl;
    return std::sqrt(dx * dx + dy * dy);
   
}

void splitLeafNode(LeafNode* leafNode) {
    // Υπολογίζουμε τον αριθμό των εγγραφών που θα μεταφερθούν στο νέο φύλλο κόμβο
    int splitSize = leafNode->records.size() / 2;

    // Δημιουργούμε ένα νέο φύλλο κόμβο
    LeafNode* newLeafNode = new LeafNode;

    // Μεταφέρουμε τις εγγραφές από το τρέχον φύλλο κόμβο στο νέο
    newLeafNode->records.insert(newLeafNode->records.begin(), leafNode->records.begin() + splitSize, leafNode->records.end());
    leafNode->records.erase(leafNode->records.begin() + splitSize, leafNode->records.end());

    // Συνδέουμε τα φύλλα κόμβους ως γείτονες
    newLeafNode->parent = leafNode->parent;
    if (leafNode->parent) {
        leafNode->parent->children.push_back(newLeafNode);
        // Ενημερώνουμε τον γονέα για τον νέο φύλλο κόμβο
    }

    // Ελέγχουμε αν ο κόμβος είναι η ρίζα και δημιουργούμε νέο εσωτερικό κόμβο αν χρειαστεί
    if (!leafNode->parent) {
        InternalNode* newRoot = new InternalNode;
        leafNode->parent = newRoot;
        newLeafNode->parent = newRoot;
        newRoot->children.push_back(leafNode);
        newRoot->children.push_back(newLeafNode);
        return;
    }

    // Αν ο γονέας έχει υπερβεί το μέγιστο αριθμό παιδιών, πρέπει να διαιρέσουμε τον εσωτερικό κόμβο
    InternalNode* parentNode = leafNode->parent;
    if (parentNode->children.size() > MAX_CHILDREN) {
        splitInternalNode(parentNode);
    }
}

void splitInternalNode(InternalNode* internalNode) {
    // Υπολογίζουμε τον αριθμό των παιδιών που θα μεταφερθούν στο νέο εσωτερικό κόμβο
    int splitSize = internalNode->children.size() / 2;

    // Δημιουργούμε ένα νέο εσωτερικό κόμβο
    InternalNode* newInternalNode = new InternalNode;

    // Μεταφέρουμε τα παιδιά από το τρέχον εσωτερικό κόμβο στο νέο
    newInternalNode->children.insert(newInternalNode->children.begin(), internalNode->children.begin() + splitSize, internalNode->children.end());
    internalNode->children.erase(internalNode->children.begin() + splitSize, internalNode->children.end());

    // Συνδέουμε τα εσωτερικά κόμβους ως γείτονες
    newInternalNode->parent = internalNode->parent;
    if (internalNode->parent) {
        internalNode->parent->children.push_back(newInternalNode);
    }

    // Ελέγχουμε αν ο κόμβος είναι η ρίζα και δημιουργούμε νέο εσωτερικό κόμβο αν χρειαστεί
    if (!internalNode->parent) {
        InternalNode* newRoot = new InternalNode;
        internalNode->parent = newRoot;
        newInternalNode->parent = newRoot;
        newRoot->children.push_back(internalNode);
        newRoot->children.push_back(newInternalNode);
        return;
    }

    // Αν ο γονέας έχει υπερβεί το μέγιστο αριθμό παιδιών, πρέπει να διαιρέσουμε τον εσωτερικό κόμβο
    InternalNode* parentNode = internalNode->parent;
    if (parentNode->children.size() > MAX_CHILDREN) {
        splitInternalNode(parentNode);
    }
}

void writeRecordToFile(const std::string& filename, const std::string& message) {
    std::ofstream outputFile(filename, std::ios::app);
    if (!outputFile) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    outputFile << message << std::endl;
    outputFile.close();
}