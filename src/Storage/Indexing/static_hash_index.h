#ifndef S_HASH_H
#define S_HASH_H

#include"../Table/RID.h"
#include"../Page/Field.h"
#include<vector>
#include <optional>
using namespace std;

#define STATIC_HASH_INDEX_DEFAULT_CAPACITY 100


//to do: change T into FieldType struct 

//template<typename T>
struct hashEntry{
    Field key;
    RID rid;
    hashEntry* next;
    hashEntry(Field k,RID r ):key(k),rid(r),next(nullptr){

    }

    hashEntry& operator=(const hashEntry& other){

        this->key = other.key;
        this->rid = other.rid;
        this->next = other.next;
        return *this;
    }
};

//template<typename T>
class hashIndex{
    private:
        vector<optional<hashEntry>> hashTable;
        size_t capacity{STATIC_HASH_INDEX_DEFAULT_CAPACITY}; 

        /*size_t hashFunction(FieldType key){

            return key % capacity;
        }
        */

        size_t hashFunction(Field key);
        int number_of_entries{0};

    public:

        hashIndex(int tablesize);
        size_t getCapacity();
        int get_number_ofentries();
        void insertIndex(Field key, RID value);
        void updateIndex(Field key, RID value);
        void deleteIndex(Field key);

        hashEntry* getHashEntryPtr(Field key);
        RID getValue(Field key);
        const vector<optional<hashEntry>>& getHashTable() const;
        
};

#endif