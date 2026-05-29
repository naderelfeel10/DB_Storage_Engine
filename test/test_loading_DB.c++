#include<iostream>
#include<vector>
#include<algorithm>
#include"../src/Storage/Table/TableIterator.h"
#include"../src/Storage/Table/Column.h"
#include"../src/Storage/Indexing/Index.h"
#include"../src/Storage/Indexing/StaticHashIndexWrapper.h"
#include<chrono>
#include<cassert>
#include"../src/Storage/Indexing/BPlusTreeIndexWrapper.h"

using namespace std;


int main(){

    vector<string> tables;
    map<string, TableHeap*>tables_map;
    map<string, StaticHashIndexWrapper*>tables_indexes;
    map<string, BPlusTreeIndexWrapper*>tables_B_indexes;

    string DB_name = "StoreDB";

    DiskManager* dm = new DiskManager(DB_name);
    BufferPoolManager* BPM = new BufferPoolManager(dm);
    
    cout<<"file name : "<<dm->file_name<<endl;
    for(auto&i:dm->tables_names){
        cout<<i.first<<endl;
    }
    
    TableHeap* loaded_table = new TableHeap(BPM,dm->tables_names["User"],-1);
    tables_map["User"] = loaded_table;
    loaded_table->loadMetaData();
    loaded_table->displayTablePages();


    for(auto&[col_name,indexes_vec]:loaded_table->indexes_map){
        cout<<"col name : "<<col_name<<endl;

        for(Index* i:indexes_vec){
            StaticHashIndexWrapper* s_hash_wrapper = dynamic_cast<StaticHashIndexWrapper*>(i);
            BPlusTreeIndexWrapper* b_plus_wrapper = dynamic_cast<BPlusTreeIndexWrapper*>(i);

            if (s_hash_wrapper != nullptr) {
                    tables_indexes["User"] = s_hash_wrapper;
                    s_hash_wrapper->displayIndexPages(); 
                    cout<<"++"<<endl;
            }
            if (b_plus_wrapper != nullptr) {
                    tables_B_indexes["User"] = b_plus_wrapper;
                    b_plus_wrapper->displayIndexPages(); 
                    cout<<"++"<<endl;
            }
        }
    }

    // load product table :
    loaded_table = new TableHeap(BPM,dm->tables_names["Product"],-1);
    tables_map["Product"] = loaded_table;
    loaded_table->loadMetaData();
    loaded_table->displayTablePages();

    for(auto&[col_name,indexes_vec]:loaded_table->indexes_map){

        cout<<"col name : "<<col_name<<endl;
        for(Index* i:indexes_vec){

            StaticHashIndexWrapper* s_hash_wrapper = dynamic_cast<StaticHashIndexWrapper*>(i);
            BPlusTreeIndexWrapper* b_plus_wrapper = dynamic_cast<BPlusTreeIndexWrapper*>(i);

            if (s_hash_wrapper != nullptr) {
                    tables_indexes["Product"] = s_hash_wrapper;
                    s_hash_wrapper->displayIndexPages(); 
                    cout<<"++"<<endl;
            }
            if (b_plus_wrapper != nullptr) {
                    tables_B_indexes["Product"] = b_plus_wrapper;
                    b_plus_wrapper->displayIndexPages(); 
                    cout<<"++"<<endl;
            }
        }
    }

    // CRUD operations on the loaded table

    ///////////////////////////////////////////////////////////

    //1. seq scan over User table to find one match
    string select_query = "select * from User where user_id=1000";
    TableIterator it(loaded_table,loaded_table->getStartingRID(), loaded_table->getStoppigRID());

    cout<<"displaying tuple with user_id = 1000"<<endl;
    auto st1 = chrono::high_resolution_clock::now();
    for(; !it.end(); ++it){
        Tuple t =  *it;
        for(auto& f:t.fields){

            if(f.getFieldType() == TYPE_INT){
                if(f.getFieldValueInt() == 1000){
                    t.print();
                    break;
                }
            }
        }
    }

    auto end1 = chrono::high_resolution_clock::now();
    auto duration1 = chrono::duration_cast<chrono::microseconds>(end1 - st1);
    cout << "searching using linear scan  duration: " << duration1.count() << " microseconds" << endl;
    ///////////////////////////////////////////////////////////

    //1B+ Tree index scan over User table to find one match

    st1 = chrono::high_resolution_clock::now();
    Field f102 = Field(TYPE_INT,1000);
    vector<RID> matched_rids = tables_B_indexes["User"]->Search(f102);
    cout<<"matched tuples for the value user_id 1000"<<endl;

    for(auto& rid:matched_rids){
        //rid.print();
        Tuple user102 = tables_map["User"]->getTupleFromRID(rid);
        user102.print();
    }
    
    end1 = chrono::high_resolution_clock::now();
    duration1 = chrono::duration_cast<chrono::microseconds>(end1 - st1);
    cout << "searching using B+ Tree scan  duration: " << duration1.count() << " microseconds" << endl;
    ///////////////////////////////////////////////////////////
    
    //2. hash index scan over User table to find one match

    st1 = chrono::high_resolution_clock::now();
    matched_rids = tables_indexes["User"]->Search(f102);
    cout<<"matched tuples for the value user_id 102"<<endl;

    for(auto& rid:matched_rids){
        //rid.print();
        Tuple user102 = tables_map["User"]->getTupleFromRID(rid);
        user102.print();
    }
    
    end1 = chrono::high_resolution_clock::now();
    duration1 = chrono::duration_cast<chrono::microseconds>(end1 - st1);
    cout << "searching using Hash index scan  duration: " << duration1.count() << " microseconds" << endl;

    ///////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////


    //update Product table
    string update_query = "update table Product set price=99.5 where product_id=550";

    Field search_key(TYPE_INT, 550);

    matched_rids = tables_indexes["Product"]->Search(search_key);
    Field new_field(TYPE_FLOAT, 99.5);
    bool update_successful = false;

    for (auto& rid : matched_rids) {
        Tuple t = tables_map["Product"]->getTupleFromRID(rid);

        // create your updated fields list
        vector<Field> updated_fields = t.fields;

        // generate new tuple
        updated_fields.erase(updated_fields.begin() + 2);
        updated_fields.insert(updated_fields.begin() + 2, new_field);

        Tuple new_tuple(updated_fields);

        RID new_rid = loaded_table->updateTuple(rid, new_tuple);

        cout << "updated successfufly"<<endl;
        update_successful = true;

        break; 
    }

    //2. hash index scan over User table to find one match

    st1 = chrono::high_resolution_clock::now();
    matched_rids = tables_indexes["Product"]->Search(search_key);
    cout<<"load the updated tuple"<<endl;

    for(auto& rid:matched_rids){
        //rid.print();
        Tuple res = tables_map["Product"]->getTupleFromRID(rid.getActualPair());
        res.print();
    }
    
    end1 = chrono::high_resolution_clock::now();
    duration1 = chrono::duration_cast<chrono::microseconds>(end1 - st1);
    cout << "searching using Hash index scan  duration: " << duration1.count() << " microseconds" << endl;

    ///////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////



    matched_rids = tables_indexes["Product"]->Search(search_key);

    for (auto& rid : matched_rids) {
        Tuple t = tables_map["Product"]->getTupleFromRID(rid);

        tables_map["Product"]->deleteTuple(rid.getActualPair());
        tables_indexes["Product"]->Delete(search_key);

        cout << "deleted successfufly"<<endl;
        break; 
    }

    /////////////////////////////////////////////////////////////////

    st1 = chrono::high_resolution_clock::now();
    matched_rids = tables_indexes["Product"]->Search(search_key);
    cout<<"load the removed tuple"<<endl;

    if(matched_rids.empty()){
        cout<<"product row with product_id=550 is deleted"<<endl;
    }
    for(auto& rid:matched_rids){
        //rid.print();
        Tuple res = tables_map["Product"]->getTupleFromRID(rid.getActualPair());
        if(!res.get_is_deleted())
            res.print();
    }
    
    end1 = chrono::high_resolution_clock::now();
    duration1 = chrono::duration_cast<chrono::microseconds>(end1 - st1);
    cout << "searching using Hash index scan  duration: " << duration1.count() << " microseconds" << endl;

    ///////////////////////////////////////////////////////////


    string delete_query = "delete from Product where product_id=550 ";





}
