#include<iostream>
#include"static_hash_index.h"
using namespace std;


hashIndex::hashIndex(int tablesize=-1){

            if(tablesize == -1){
                hashTable.resize(capacity);
            }
            else{
                hashTable.resize(tablesize);
                capacity = tablesize;
            }
        }


size_t hashIndex::hashFunction(Field key) {
            
            size_t raw_val = 0;

            if (key.getFieldType() == TYPE_INT ||
                key.getFieldType() == TYPE_FLOAT ||
                key.getFieldType() == TYPE_BOOL ) {
                    
                raw_val = static_cast<int>(key.getFieldValueInt());
                
            }
            else if (key.getFieldType() == TYPE_STRING) {
                // Use a standard hash for strings
                raw_val = std::hash<string>{}(key.getFieldValueStr());
            }
            cout<<"row value is : "<<raw_val <<endl;
            cout<<"index slot is : "<<raw_val % capacity<<endl;
            return raw_val % capacity;
        }
        

size_t hashIndex::getCapacity(){
        return this->capacity;
}


void hashIndex::insertIndex(Field key, RID value){

        key.print();
        size_t index = hashFunction(key); 
        cout<<"index : "<<index<<endl;
            
        if(hashTable[index].has_value()){

            hashEntry* curr_hash_entry = &hashTable[index].value();
            while(curr_hash_entry->next){
                curr_hash_entry = curr_hash_entry->next;
            }

            curr_hash_entry->next = new hashEntry(key, value);
            cout<<"inserted "<<endl;
                
        }else{
            cout<<"inserted into new hash entry "<<endl;
            hashTable[index] = hashEntry(key, value); 
        }
        number_of_entries++;

}



void hashIndex::updateIndex(Field key, RID value){
            hashEntry* curr_ptr = getHashEntryPtr(key);

            if(curr_ptr != nullptr){
                curr_ptr->key = key;
                curr_ptr->rid = value;
                cout<<"index updated successfuly"<<endl;
            }
            else cout<<"key not found"<<endl;
            
        }


void hashIndex::deleteIndex(Field key){

            size_t index = hashFunction(key);

            if(hashTable[index]->key == key){
                hashEntry* toDelete = hashTable[index]->next;
                
                if(toDelete != nullptr){
                    hashEntry* next_next = toDelete->next;
                    hashTable[index] = *toDelete;
                    hashTable[index]->next = next_next;
                    delete toDelete;
                }
                else{
                    hashTable[index] = nullopt;
                }
                cout<<"index deleted successfuly1"<<endl;
                number_of_entries--;
                

            }
            else{
                hashEntry* curr_ptr = &hashTable[index].value();

                while(curr_ptr->next != nullptr){
                    if(curr_ptr->next->key == key){
                        curr_ptr->next = curr_ptr->next->next;
                        cout<<"index deleted successfuly2"<<endl;
                        return;
                    }
                    curr_ptr = curr_ptr->next;
                }

                 cout<<"key not found"<<endl;
        }

        }


hashEntry* hashIndex::getHashEntryPtr(Field key){
            
            size_t index = hashFunction(key);

            if(!hashTable[index].has_value()){
                cout<<"hashTable has no value"<<endl;
                return nullptr;
            }

            hashEntry* curr_hash_entry = &hashTable[index].value();

            while(curr_hash_entry != nullptr ){

                    if(curr_hash_entry->key == key){
                        return curr_hash_entry;
                    }
                    curr_hash_entry = curr_hash_entry->next;
            }

            return nullptr;

        }


RID hashIndex::getValue(Field key){

            //size_t index = hashFunction(key);
            hashEntry* curr_ptr = getHashEntryPtr(key);

            if(curr_ptr == nullptr){
                cout<<"curr_ptr is null"<<endl;
                return RID(-1, -1);
            }else{
                return curr_ptr->rid;
            }

            return RID(-1, -1);

        } 

int hashIndex::get_number_ofentries(){
    return this->number_of_entries;
}
const vector<optional<hashEntry>>& hashIndex::getHashTable() const {
    return this->hashTable;
}
/*
int
main(){

    cout<<"nader elfeel"<<endl;

    hashIndex* hash_index1 = new hashIndex(200);
    cout<<"hash index capacity : "<<hash_index1->getCapacity()<<endl;


    int int_key1 = 21;
    Field f1(TYPE_INT, int_key1);
    RID rid1(1,13);
    hash_index1->insertIndex(f1, rid1);


    int int_key2 = 221;
    Field f2(TYPE_INT, int_key2);
    RID rid2(4,4);
    hash_index1->insertIndex(f2, rid2);

    int int_key3 = 421;
    Field f3(TYPE_INT, int_key3);
    RID rid3(6,6);
    hash_index1->insertIndex(f3, rid3);

    cout<<"RID 21 :  ("<<hash_index1->getValue(f1).getPageId()<<", "<<hash_index1->getValue(f1).getSlotNum()<<")"<<endl;
    cout<<"RID 221 :  ("<<hash_index1->getValue(f2).getPageId()<<", "<<hash_index1->getValue(f2).getSlotNum()<<")"<<endl;
    cout<<"RID 421 :  ("<<hash_index1->getValue(f3).getPageId()<<", "<<hash_index1->getValue(f3).getSlotNum()<<")"<<endl;


    hash_index1->updateIndex(f2, RID(3,3));
    cout<<"RID 221 :  ("<<hash_index1->getValue(f2).getPageId()<<", "<<hash_index1->getValue(f3).getSlotNum()<<")"<<endl;



    hash_index1->updateIndex(Field(TYPE_INT, 222) , RID(5,5));
    cout<<"RID 221 :  ("<<hash_index1->getValue(f2).getPageId()<<", "<<hash_index1->getValue(f2).getSlotNum()<<")"<<endl;
    

    hash_index1->deleteIndex(f1);
    cout<<"RID 21 :  ("<<hash_index1->getValue(f1).getPageId()<<", "<<hash_index1->getValue(f1).getSlotNum()<<")"<<endl;
    cout<<"RID 221 :  ("<<hash_index1->getValue(f2).getPageId()<<", "<<hash_index1->getValue(f2).getSlotNum()<<")"<<endl;
    cout<<"RID 421 :  ("<<hash_index1->getValue(f3).getPageId()<<", "<<hash_index1->getValue(f3).getSlotNum()<<")"<<endl;

    hash_index1->deleteIndex(f2);
    cout<<"RID 21 :  ("<<hash_index1->getValue(f1).getPageId()<<", "<<hash_index1->getValue(f1).getSlotNum()<<")"<<endl;
    cout<<"RID 221 :  ("<<hash_index1->getValue(f2).getPageId()<<", "<<hash_index1->getValue(f2).getSlotNum()<<")"<<endl;
    cout<<"RID 421 :  ("<<hash_index1->getValue(f3).getPageId()<<", "<<hash_index1->getValue(f3).getSlotNum()<<")"<<endl;


    Field f4(TYPE_STRING, "elfeel");
    hash_index1->insertIndex(f4,RID(7,7));
    cout<<"RID 'elfeel' :  ("<<hash_index1->getValue(f4).getPageId()<<", "<<hash_index1->getValue(f4).getSlotNum()<<")"<<endl;

    
    Field dm_f1(TYPE_FLOAT,1.344);
    Field dm_f2(TYPE_FLOAT,1.344);
    
    cout<<dm_f1.getFieldValueFloat()<<endl;
    cout<<dm_f2.getFieldValueFloat()<<endl;
    

    if(dm_f1 == dm_f2){
        cout<<"f1 = f2";
    }else{
        cout<<"f1 != f2";
    }
    
   



}
*/