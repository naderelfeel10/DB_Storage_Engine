#include<iostream>
#include"BPlusTreeIndex.h"
using namespace std;


// Implementation of splitChild function
void BPlusTree::splitChild(Node* parent, int index, Node* child) {
    Node* newChild = new Node(child->isLeaf);

    // If it's a leaf, the "split point" is different
    if (child->isLeaf) {
        // copy entries to new leaf
        newChild->entries.assign(child->entries.begin() + t - 1, child->entries.end());
        child->entries.resize(t - 1);
        
        // use the first key of the NEW child as the separator in the parent
        parent->keys.insert(parent->keys.begin() + index, newChild->entries[0].key);
        
        // maintain leaf linked list
        newChild->next = child->next;
        child->next = newChild;
    } 
    else {

        parent->keys.insert(parent->keys.begin() + index, child->keys[t - 1]);
        newChild->keys.assign(child->keys.begin() + t, child->keys.end());
        child->keys.resize(t - 1);

        newChild->children.assign(child->children.begin() + t, child->children.end());
        child->children.resize(t);
    }
    
    parent->children.insert(parent->children.begin() + index + 1, newChild);
}


// Implementation of insertNonFull function

// to do : operator overloading >, < in Field Class (done)
void BPlusTree::insertNonFull(Node* node, Field key)
{
    if (node->isLeaf) {
        node->keys.insert(upper_bound(node->keys.begin(),
                                      node->keys.end(),
                                      key),
                          key);
    }
    else {
        int i = node->keys.size() - 1;
        while (i >= 0 && key < node->keys[i]) {
            i--;
        }
        i++;
        if (node->children[i]->keys.size() == 2 * t - 1) {
            splitChild(node, i, node->children[i]);
            if (key > node->keys[i]) {
                i++;
            }
        }
        insertNonFull(node->children[i], key);
    }
}

void BPlusTree::insertNonFull(Node* node, Field key, RID value) {
    if (node->isLeaf) {
        // Create an Entry and find where to put it
        Entry newEntry(key, value);
        auto it = std::upper_bound(node->entries.begin(), node->entries.end(), 
                                   newEntry, [](const Entry& a, const Entry& b) {
            return a.key < b.key;
        });
        node->entries.insert(it, newEntry);
    } else {
        int i = node->keys.size() - 1;
        while (i >= 0 && key < node->keys[i]) {
            i--;
        }
        i++;
        if (node->children[i]->keys.size() == 2 * t - 1) {
            splitChild(node, i, node->children[i]);
            if (key > node->keys[i]) {
                i++;
            }
        }
        insertNonFull(node->children[i], key, value);
    }
}


// Implementation of remove function
void BPlusTree::remove(Node* node, Field key)
{
    //if node is a leaf
    if (node->isLeaf) {
        // find the entry in the entries vector
        auto it = find_if(node->entries.begin(), node->entries.end(),
                               [&](const Entry& e) { return e.key == key; });

        // remove from entries
        if (it != node->entries.end()) {
            node->entries.erase(it);

            // 2. Also keep the 'keys' vector in sync if you use it in leaves
            auto kit = find(node->keys.begin(), node->keys.end(), key);
            if (kit != node->keys.end()) {
                node->keys.erase(kit);
            }
        }
    }
    else {
        int idx = lower_bound(node->keys.begin(),node->keys.end(), key) - node->keys.begin();

        if (idx < node->keys.size()&& node->keys[idx] == key) {

            if (node->children[idx]->keys.size() >= t) {
                Node* predNode = node->children[idx];
                while (!predNode->isLeaf) {
                    predNode = predNode->children.back();
                }
                Field pred = predNode->keys.back();
                node->keys[idx] = pred;
                remove(node->children[idx], pred);
            }

            else if (node->children[idx + 1]->keys.size()
                     >= t) {
                Node* succNode = node->children[idx + 1];
                while (!succNode->isLeaf) {
                    succNode = succNode->children.front();
                }
                Field succ = succNode->keys.front();
                node->keys[idx] = succ;
                remove(node->children[idx + 1], succ);
            }
            else {
                merge(node, idx);
                remove(node->children[idx], key);
            }
        }
        else {
            if (node->children[idx]->keys.size() < t) {
                if (idx > 0
                    && node->children[idx - 1]->keys.size()
                           >= t) {
                    borrowFromPrev(node, idx);
                }
                else if (idx < node->children.size() - 1
                         && node->children[idx + 1]
                                    ->keys.size()
                                >= t) {
                    borrowFromNext(node, idx);
                }
                else {
                    if (idx < node->children.size() - 1) {
                        merge(node, idx);
                    }
                    else {
                        merge(node, idx - 1);
                    }
                }
            }
            remove(node->children[idx], key);
        }
    }
}

// Implementation of borrowFromPrev function
void BPlusTree::borrowFromPrev(Node* node, int index)
{
    Node* child = node->children[index];
    Node* sibling = node->children[index - 1];

    child->keys.insert(child->keys.begin(),
                       node->keys[index - 1]);
    node->keys[index - 1] = sibling->keys.back();
    sibling->keys.pop_back();

    if (!child->isLeaf) {
        child->children.insert(child->children.begin(),
                               sibling->children.back());
        sibling->children.pop_back();
    }
}

// Implementation of borrowFromNext function
void BPlusTree::borrowFromNext(Node* node, int index)
{
    Node* child = node->children[index];
    Node* sibling = node->children[index + 1];

    child->keys.push_back(node->keys[index]);
    node->keys[index] = sibling->keys.front();
    sibling->keys.erase(sibling->keys.begin());

    if (!child->isLeaf) {
        child->children.push_back(
            sibling->children.front());
        sibling->children.erase(sibling->children.begin());
    }
}

// Implementation of merge function
void BPlusTree::merge(Node* node, int index)
{
    Node* child = node->children[index];
    Node* sibling = node->children[index + 1];

    child->keys.push_back(node->keys[index]);
    child->keys.insert(child->keys.end(),
                       sibling->keys.begin(),
                       sibling->keys.end());
    if (!child->isLeaf) {
        child->children.insert(child->children.end(),
                               sibling->children.begin(),
                               sibling->children.end());
    }

    node->keys.erase(node->keys.begin() + index);
    node->children.erase(node->children.begin() + index
                         + 1);

    delete sibling;
}

// Implementation of printTree function
void BPlusTree::printTree(Node* node, int level)
{
    if (node != nullptr) {
        for (int i = 0; i < level; ++i) {
            cout << "  ";
        }
        for (Field& key : node->keys) {
            //cout << key << " ";
            key.print();
        }
        cout << endl;

        for (Node* child : node->children) {
            printTree(child, level + 1);
        }
    }
}

// Implementation of printTree wrapper function
void BPlusTree::printTree()
{
    printTree(root, 0);
}


// to do : operator overloading for comparision in Field (done)
// Implementation of search function (done)
bool BPlusTree::search(Field key)
{
    Node* current = root;
    while (current != nullptr) {
        int i = 0;
        while (i < current->keys.size()
               && key > current->keys[i]) {
            i++;
        }
        if (i < current->keys.size()
            && key == current->keys[i]) {

            return true;
        }
        if (current->isLeaf) {
            return false;
        }
        current = current->children[i];
    }
    return false;

}

RID BPlusTree::findValue(Field key) {
    if (root == nullptr) {
        return RID(-1,-1); 
    }

    Node* current = root;

    // traverse through the nodes
    while (!current->isLeaf) {
        int i = 0;

        // find the first key that is greater than or equal to our search key
        while (i < current->keys.size() && key >= current->keys[i]) {
            i++;
        }

        // move the pointer to this node
        current = current->children[i];
    }

    for (const auto& entry : current->entries) {
        if (entry.key == key) {
            return entry.value; 
        }
    }

    return RID(-1,-1); 
}



// to do : change the return values into vector of Entries not keys (done)
vector<RID> BPlusTree::rangeQuery(Field lower, Field upper)
{
    vector<RID> result;
    Node* current = root;
    while (!current->isLeaf) {
        int i = 0;
        while (i < current->keys.size()
               && lower > current->keys[i]) {
            i++;
        }
        current = current->children[i];
    }
    while (current != nullptr) {
        for (Entry& e : current->entries) {
            if (e.key >= lower && e.key <= upper) {
                //result.push_back(e.key);
                result.push_back(e.value);
            }
            if (e.key > upper) {
                return result;
            }
        }
        current = current->next;
    }
    return result;
}


void BPlusTree::insert(Field key)
{
    if (root == nullptr) {
        root = new Node(true);
        root->keys.push_back(key);
    }
    else {
        if (root->keys.size() == 2 * t - 1) {
            Node* newRoot = new Node();
            newRoot->children.push_back(root);
            splitChild(newRoot, 0, root);
            root = newRoot;
        }
        insertNonFull(root, key);
    }
}


// insert overload funtion , for key,value insertion
void BPlusTree::insert(Field key, RID value) {

    if (root == nullptr) {
        root = new Node(true);
        root->entries.push_back(Entry(key, value));

    } else {
        if (root->keys.size() == 2 * t - 1) { 

            Node* newRoot = new Node(false);
            newRoot->children.push_back(root);
            splitChild(newRoot, 0, root);
            root = newRoot;
        }
        insertNonFull(root, key, value);
    }
}

void BPlusTree::remove(Field key)
{
    if (root == nullptr) {
        return;
    }

    remove(root, key);

    if (root->keys.empty() && !root->isLeaf) {
        Node* tmp = root;
        root = root->children[0];
        delete tmp;
    }

}



void BPlusTree::clear(Node* node) {

    if (!node) return;
    if (!node->isLeaf) {
        for (Node* child : node->children) clear(child);
    }
    delete node;
}
BPlusTree::~BPlusTree() { clear(root); }


/*
int main()
{
    BPlusTree tree(3);

    
    // Insert elements
    Field f1(TYPE_INT, 10);
    Field f2(TYPE_INT, 20);
    Field f3(TYPE_INT, 6);
    Field f4(TYPE_INT, 7);
    Field f5(TYPE_INT, 15);
    Field f6(TYPE_INT, 25);
    Field f7(TYPE_INT, 30);
    Field f8(TYPE_INT, 10);
    Field f9(TYPE_INT, 60);
    Field f10(TYPE_INT, 100);

    RID rid1(1,1);
    RID rid2(1,2);
    RID rid3(1,3);
    RID rid4(1,4);
    RID rid5(1,5);
    RID rid6(1,6);
    RID rid7(1,7);
    RID rid8(1,8);
    RID rid9(1,9);
    RID rid10(1,10);

    tree.insert(f1, rid1);
    tree.insert(f2, rid2);
    tree.insert(f3, rid3);
    tree.insert(f4, rid4);
    tree.insert(f5, rid5);
    tree.insert(f6, rid6);
    tree.insert(f7, rid7);
    tree.insert(f8, rid8);
    tree.insert(f9, rid9);
    tree.insert(f10, rid10);


    cout << "B+ Tree after insertions:" << endl;
    tree.printTree();

    // Search for a key
    Field searchKey = f1;
    cout << "\nSearching for key " << searchKey.getFieldValueInt() << ": "
         << (tree.search(searchKey) ? "Found" : "Not Found")
         << endl;


    // Perform a range query
    Field lower = f3, upper = f10;
    vector<Entry> rangeResult = tree.rangeQuery(lower, upper);
    cout << "\nRange query [" << lower.getFieldValueInt() << ", " << upper.getFieldValueInt()
         << "]: "<<endl;
    for (Entry e : rangeResult) {
        //cout << key << " ";
        e.key.print();
        cout<<"RID : "<<"( "<<e.value.getActualPair().getPageId()<<", "<<e.value.getActualPair().getSlotNum()<<" )"<<endl;
    }
    cout << endl;

    // Remove a key
    Field removeKey = f4;
    tree.remove(removeKey);
    cout << "\nB+ Tree after removing " << removeKey.getFieldValueInt() << ":"
         << endl;
    tree.printTree();

    RID searchRID1 = tree.findValue(f6);
    cout<<"RID : "<<"( "<<searchRID1.getActualPair().getPageId()<<", "<<searchRID1.getActualPair().getSlotNum()<<" )"<<endl;

    RID searchRID2 = tree.findValue(f4);
    cout<<"RID : "<<"( "<<searchRID2.getActualPair().getPageId()<<", "<<searchRID2.getActualPair().getSlotNum()<<" )"<<endl;

    return 0;
}
*/