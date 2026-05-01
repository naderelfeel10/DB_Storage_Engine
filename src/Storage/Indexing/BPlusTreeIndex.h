#ifndef BPlusIndex_H
#define BPlusIndex_H

#include <algorithm>
#include <iostream>
#include <vector>
#include"../Page/Field.h"
#include"../Table/RID.h"

using namespace std;

struct Node;


struct Entry{
    Field key;
    RID value; // for leaf nodes only
    Node* next; // for inner nodes , this is a pointer to the (virtecal) node

    Entry():value(RID(-1,-1)) ,next(nullptr) {}
    Entry(Field key, RID rid):key(key),value(rid),next(nullptr){}

};

struct Node {

    bool isLeaf;
    std::vector<Entry> entries;// vector of entries used in leaf nodes
    vector<Field> keys;
    vector<Node*> children;
    Node* next; // the sibling node (horizontal)

    Node(bool leaf = false): isLeaf(leaf), next(nullptr){}

};


class BPlusTree {
public:

    Node* root;
    // Minimum degree (defines the range for the number of
    // keys)
    int t;

    //function to split a child node
    void splitChild(Node* parent, int index, Node* child);

    //function to insert a key in a non-full node
    void insertNonFull(Node* node, Field key);
    
    void insertNonFull(Node* node, Field key, RID value);

    //function to remove a key from a node
    void remove(Node* node, Field key);

    //function to borrow a key from the previous sibling
    void borrowFromPrev(Node* node, int index);

    //function to borrow a key from the next sibling
    void borrowFromNext(Node* node, int index);

    // Function to merge two nodes
    void merge(Node* node, int index);

    // function To print the tree
    void printTree(Node* node, int level);


public:
    BPlusTree(int degree): root(nullptr), t(degree){}

    void insert(Field key);
    void insert(Field key, RID value);
    
    // to do : i need to change to return an Entry(done in findValue)
    bool search(Field key);
    RID findValue(Field key);

    void remove(Field key);
    vector<RID> rangeQuery(Field lower, Field upper);

    //to be implemented
    void deleteRangeQuery(Field lower, Field upper);
    
    void printTree();

    void clear(Node* node);
    
    ~BPlusTree();
};


#endif