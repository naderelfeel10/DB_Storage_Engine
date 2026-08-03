#ifndef S_HASH_H
#define S_HASH_H

#include"../Table/RID.h"
#include"../Page/Field.h"
#include"../../Buffer/BufferPoolManager.h"
#include<vector>
#include <optional>
using namespace std;

#define STATIC_HASH_INDEX_DEFAULT_CAPACITY 100


//to do: change T into FieldType struct (done)

//template<typename T>
struct hashEntry{
    Field key;
    RID rid;
    RID next_rid;
    hashEntry* next;
    hashEntry(Field k,RID r ):key(k),rid(r),next(nullptr),next_rid(-1,-1){}
 
    void serializeOneEntry(char* data){
        int offset{0};
        // ser. field
        key.serialize(data+offset);
        offset+=key.getSerializedSize();
        // ser. rid
        rid.serialize(data+offset);
        offset += rid.getSerializedSize();
        
        // save next rid pointer
        next_rid.serialize(data+offset);
        offset += next_rid.getSerializedSize();
    }

    void deSerializeOneEntry(char* data){
        int offset{0};
        // deser. field
        this->key.deserialize(data+offset);
        offset+= this->key.getSerializedSize();

        // dser. rid
        this->rid.deserialize(data+offset);        
        offset += this->rid.getSerializedSize();
        
        this->next_rid.deserialize(data+offset);
        offset += this->next_rid.getSerializedSize();
        
        this->next = nullptr;
    }

    int getEntrySize(){
        return key.getSerializedSize()+2*rid.getSerializedSize();
    }

    int getLinkedListSize(){
        int res{0};
        hashEntry* dummy_next= next;
        while(dummy_next){
            res++;
            dummy_next = dummy_next->next;
        }
        
        return res;
    }



    hashEntry& operator=(const hashEntry& other){

        this->key = other.key;
        this->rid = other.rid;
        this->next = other.next;
        return *this;
    }


    void serialize(char* data, int&size){
        int offset{0};
        // ser. field
        key.serialize(data+offset);
        offset+=key.getSerializedSize();


        // ser. rid
        rid.serialize(data+offset);
        offset += rid.getSerializedSize();

        // i need to save next pointer
        // saving a pointer is not correct here , it's just a  location in memory 
        // i will save the key, and a punch of rids, and while seserializing the Entry again i will recreate them in the same order
        
        hashEntry* dummy_next = next;

        // i need to save linked list of entries size
        int linked_list_size = getLinkedListSize();

        memcpy(data+offset, &linked_list_size, sizeof(int));
        offset+=sizeof(int);

        dummy_next = next;

        while(dummy_next!=nullptr){
            
            dummy_next->key.serialize(data+offset);
            offset+= dummy_next->key.getSerializedSize();
            dummy_next->rid.serialize(data+offset);
            offset+= dummy_next->rid.getSerializedSize();

            //cout<<"ser_test : ";
            //dummy_next->rid.print();
            dummy_next = dummy_next->next;
        }
        size = offset;

}


void deserialize(char* data, int &size){
       int offset{0};

        // deserialize field
        this->key.deserialize(data+offset);
        offset+=this->key.getSerializedSize();

        // deserialize rid
        this->rid.deserialize(data+offset);
        offset += rid.getSerializedSize();

        // size of linked list of RIds
        int linked_list_size{0};
        memcpy(&linked_list_size, data+offset,sizeof(int));
        offset+=sizeof(int);
        
        hashEntry* curr =  this;
        while(linked_list_size--){

            // ddeserialize key
            Field dummyfield(this->key.getFieldType());
            dummyfield.deserialize(data+offset);
            offset += dummyfield.getSerializedSize();

            // deserialize each RID
            RID dummyRID(-1,-1);
            dummyRID.deserialize(data+offset);
            offset += dummyRID.getSerializedSize();
            
            // create new hashEntry on heap
            hashEntry* e = new hashEntry(dummyfield, dummyRID);
            curr->next = e;
            curr = e;
        }

    size = offset;
        
    }

};


class hashIndex {

    private:
    
        vector<optional<hashEntry>> hashTable;
        FieldType field_type;
        int col_index;
        size_t hashFunction(Field key);
        BufferPoolManager* BPM;
        int number_of_entries{0};
        int first_page_id{-1};
        int last_page_id{-1};

    public:
        size_t capacity{STATIC_HASH_INDEX_DEFAULT_CAPACITY}; 

        hashIndex(BufferPoolManager* BPM,FieldType field_type,int col_index, int tablesize=-1, int existing_first_page_id=-1,int existing_last_page_id=-1);
        size_t getCapacity();
        
        int get_number_ofentries();
        void insertIndex(Field key, RID value);
        void updateIndex(Field key, RID value);
        void deleteIndex(Field key);

        hashEntry* getHashEntryPtr(Field key);
        // i need to get the first entry of any given key, so i can search through it's next pointer
        hashEntry* getFirstValue(Field key);
        // let this function to return vector of RIDs(done)
        vector<RID> getValue(Field key);
        vector<optional<hashEntry>>& getHashTable() ;

        void saveIndexMeta();
        void loadIndexMeta();
        void load_hash_table(int page_id);

        void serializeHashIndex(char* data);
        void deserializeHashIndex(char* data);

        int get_first_pageid(){return this->first_page_id;}
        int get_last_pageid(){return this->last_page_id;}

        void set_last_page_id(int last_page_id) {this->last_page_id = last_page_id;}
        void set_first_page_id(int first_page_id) {this->first_page_id = first_page_id;}
        int getColindex(){return this->col_index;}

        ~hashIndex();
};

#endif