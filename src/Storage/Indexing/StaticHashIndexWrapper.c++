#include<iostream>
#include<iostream>
#include"StaticHashIndexWrapper.h"
using namespace std;




void  StaticHashIndexWrapper::Insert(Field&field,string col_name, vector<Column> tuple_cols, RID rid){
    

    if(field.getFieldType() != this->field_type){
        cerr<<"field types don't match"<<endl;
        return;
    }

    cout<<"ratio: "<<this->staticHashIndex->get_number_ofentries()*1.0 / this->staticHashIndex->getCapacity()<<endl;

    if(this->staticHashIndex->get_number_ofentries()*1.0 / this->staticHashIndex->getCapacity() >=0.75){
        cout<<"=============================hash table is almost full==============================="<<endl;

        // doouble the capacity
        int old_table_capacity = this->staticHashIndex->getCapacity();
        hashIndex* new_staticHashIndex = new hashIndex(old_table_capacity);


        // copy the old one into the new hash index
        vector<optional<hashEntry>> hashindex_entries = this->staticHashIndex->getHashTable();

        for(auto& bucket:hashindex_entries){

            if(bucket.has_value()){
                hashEntry* curr = &bucket.value();

                while(curr != nullptr){
                    new_staticHashIndex->insertIndex(curr->key,curr->rid);
                    curr = curr->next;
                }

            }

        }

        // delete the old one and reassgin
        delete this->staticHashIndex;
        this->staticHashIndex = new_staticHashIndex;

    }
    
    this->staticHashIndex->insertIndex(field,rid);
    //cout<<  
}

// delete a hash index wrapper over the original one
void  StaticHashIndexWrapper::Delete(Field&field){
    this->staticHashIndex->deleteIndex(field);
    cout<<"field is deleted."<<endl;
}

// searching over the hash index
RID  StaticHashIndexWrapper::Search(Field&field)const{
    return this->staticHashIndex->getValue(field);
}

/*
int 
main(){
    //StaticHashIndexWrapper* s_hash_wrapper = new  StaticHashIndexWrapper (index, col_name, field_type):


}*/