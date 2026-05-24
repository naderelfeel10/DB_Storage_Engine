#ifndef BPlusIndex_H
#define BPlusIndex_H

#include <algorithm>
#include <iostream>
#include <vector>
#include"../Page/Field.h"
#include"../Table/RID.h"
#include"../../Buffer/BufferPoolManager.h"

using namespace std;

struct Node;


struct Entry{
    Field key;
    RID value; // for leaf nodes only
    Node* next; // for inner nodes , this is a pointer to the (virtecal) node

    // to do
    void serialize(char* data){
        int offset{0};

        this->key.serialize(data+offset);
        offset+=this->key.getSerializedSize();

        this->value.serialize(data+offset);
        offset+=this->value.getSerializedSize();

    }
    void deserialize(char* data){
        int offset{0};

        this->key.deserialize(data+offset);
        offset+=this->key.getSerializedSize();

        this->value.deserialize(data+offset);
        offset+=this->value.getSerializedSize();

        this->next = nullptr;
    }

    Entry():value(RID(-1,-1)) ,next(nullptr) {}
    Entry(Field key, RID rid):key(key),value(rid),next(nullptr){}

};

struct Node {

    bool isLeaf;
    std::vector<Entry> entries;// vector of entries used in leaf nodes
    vector<Field> keys;
    vector<Node*> children;
    vector<int> children_pages_ids;
    Node* next; // the sibling node (horizontal)
    // i need those for serialization, also for storage on the actual disk
    int curr_page_id;
    int next_page_id;

    // to do
    void serialize(char* data) {
        int offset = sizeof(PageHeader);

        // save some meta data
        memcpy(data + offset, &this->isLeaf, sizeof(bool));
        offset += sizeof(bool);

        // if leaf save entries else if internal save keys
        int num_elements = this->isLeaf ? this->entries.size() : this->keys.size();
        memcpy(data + offset, &num_elements, sizeof(int));
        offset += sizeof(int);

        // Write the right horizontal sibling page id
        memcpy(data + offset, &this->next_page_id, sizeof(int));
        offset += sizeof(int);

        // saving the content 
        if (this->isLeaf) {
            // if leaf so we have to save all entries 
            for (auto& entry : this->entries) {
                entry.serialize(data + offset);
                offset += (entry.key.getSerializedSize() + entry.value.getSerializedSize()); 
            }
        } else {
            // inner : serialize the parallel Key and Children Page ID arrays
            //inner node has N keys and N+1 children pointers.
            for (size_t i = 0; i < this->keys.size(); ++i) {
                this->keys[i].serialize(data + offset);
                offset += this->keys[i].getSerializedSize();
            }

            // write out the raw integer child page ids
            for (size_t i = 0; i < this->children.size(); ++i) {

                int child_pid = this->children[i]->curr_page_id; 
                memcpy(data + offset, &child_pid, sizeof(int));
                offset += sizeof(int);
            }
        }
    }

    void deserialize(char* data) {
        int offset = sizeof(PageHeader);
        
        // load meta 
        memcpy(&this->isLeaf, data + offset, sizeof(bool));
        offset += sizeof(bool);
        
        int num_elements = 0;
        memcpy(&num_elements, data + offset, sizeof(int));
        offset += sizeof(int);
        
        memcpy(&this->next_page_id, data + offset, sizeof(int));
        offset += sizeof(int);
        
        this->entries.clear();
        this->keys.clear();
        this->children.clear();
        this->next = nullptr;
        
        // load body
        if (this->isLeaf) {
            for (int i = 0; i < num_elements; ++i) {
                Entry entry;
                entry.deserialize(data + offset);
                offset += (entry.key.getSerializedSize() + entry.value.getSerializedSize()); 
                this->entries.push_back(entry);
            }

        }else{

            // if not leaf so we need keys
            for (int i = 0; i < num_elements; ++i) {
                Field key;
                key.deserialize(data + offset);
                offset += key.getSerializedSize();
                this->keys.push_back(key);
            }

            // we also need children page ids
            if (num_elements > 0) {
                for (int i = 0; i <= num_elements; ++i) {
                    int child_pid;
                    memcpy(&child_pid, data + offset, sizeof(int));
                    offset += sizeof(int);
                    this->children_pages_ids.push_back(child_pid);
                }
            }
        }
    }

    Node(bool leaf = false): isLeaf(leaf), next(nullptr){}

};


class BPlusTree {
public:

    Node* root;
    // minimum degree
    int t;
    int meta_page_id{-1};
    BufferPoolManager* bpm;

    void saveBPlusTree();
    //save each node recursivly
    void saveNode(Node* node);

    void loadBPlusTree();
    Node* loadNode(int page_id);

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

    BPlusTree(int degree, int meta_page_id, BufferPoolManager* bpm) 
        :t(degree), root(nullptr), bpm(bpm), meta_page_id(meta_page_id) {}

    void insert(Field key);
    void insert(Field key, RID value);
    
    // to do : i need to change to return an Entry(done in findValue)
    bool search(Field key);
    vector<RID> findValue(Field key);

    void remove(Field key);
    vector<RID> rangeQuery(Field lower, Field upper);

    //to be implemented
    void deleteRangeQuery(Field lower, Field upper);
    
    void printTree(Node* node, int level, string indent, bool isLast);
    void printTree();
    void clear(Node* node);
    
    // to do
    void serialize(char* data);
    void deserialize(char* data);

};


#endif