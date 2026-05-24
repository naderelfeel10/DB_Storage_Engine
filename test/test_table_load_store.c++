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
/*
int 
main(){

    string query1 = "create database testDB";
    string DB_name = "testDB";
    
    // new database is created with this file name
    DiskManager* dm = new DiskManager(DB_name);
    BufferPoolManager* BPM = new BufferPoolManager(dm);

    vector<string> tables;
    string query2 = "create table User(user_id int, firstName varchar(30),lastName varchar(30), age int);";
    
    string table_name = "User";
    tables.push_back(table_name);

    string col1_name = "user_id";
    string col2_name = "firstName";  
    string col3_name = "lastName";
    string col4_name = "age";  

    FieldType col1_type = TYPE_INT;
    FieldType col2_type = TYPE_STRING;
    FieldType col3_type = TYPE_STRING;
    FieldType col4_type = TYPE_INT;


    Column col1 = Column(col1_type,col1_name,sizeof(int));
    Column col2 = Column(col2_type,col2_name,30);
    Column col3 = Column(col3_type,col3_name,30);
    Column col4 = Column(col4_type,col4_name,sizeof(int));

    int table_first_page_id{-1};

    cout<<" file name  : "<<dm->file_name<<endl;
    cout<<" tables_names size  : "<<dm->tables_names.size()<<endl;
    cout<<" pages_table size  : "<<dm->pages_table.size()<<endl;

    for(auto& e:dm->tables_names){
        cout<<e.first<<"-------------"<<e.second<<endl;
    }
    
    if(dm->tables_names.find(table_name) != dm->tables_names.end()){
        cout<<"table found in the database"<<endl;
        table_first_page_id = dm->tables_names[table_name];
    }

    TableHeap* table_heap = new TableHeap(BPM,table_first_page_id,-1);
    dm->addTable(table_name,table_heap->get_first_page_id());
    
    vector<Column> cols = {col1, col2, col3, col4};

    table_heap->setCols(cols);
    table_heap->setTableName(table_name);
    table_heap->printColumns();

    vector<RID> table_rids;
    for(int i=0;i<200;i++){

        Field in1 = Field(TYPE_INT, i);
        Field in2 = Field(TYPE_STRING, "nader");
        Field in3 = Field(TYPE_STRING, "elfeel");
        Field in4 = Field(TYPE_FLOAT, (21.8+i));
        //in1.print();
        Tuple t = Tuple({in1, in2, in3, in4});
        RID t_rid =  table_heap->insertTuple(t);
        table_rids.push_back(t_rid);
    }
    Tuple* t1_res = table_heap->getTuple(table_rids[0]);
    t1_res->print();



    string index_col_name = "user_id";

    TableIterator table_iterator(table_heap,table_rids.front(), table_rids.back());
    //hashIndex* hash_index1 = new hashIndex(BPM,TYPE_INT,80,-1);
    //StaticHashIndexWrapper* s_hash_wrapper = new  StaticHashIndexWrapper(BPM,hash_index1, "user_id", TYPE_INT);

    table_heap->createIndex(STATIC_HASH_INDEX,index_col_name,300);
    StaticHashIndexWrapper* s_hash_wrapper = static_cast<StaticHashIndexWrapper*>(table_heap->getIndex(index_col_name,STATIC_HASH_INDEX));


    string  static_hashindex_col1_name = "user_id";
    for(;!table_iterator.end();++table_iterator){
        Tuple tuple = *table_iterator;
        int counter{0};
        for(auto& col:table_heap->getCols()){
            if(col.getColName() == static_hashindex_col1_name){
                static_cast<StaticHashIndexWrapper*>(s_hash_wrapper)->Insert(tuple.fields[counter],static_hashindex_col1_name,table_heap->getCols(),table_iterator.getCurrRIDPointer());
            }
            counter++;
        }
    }   
    Field f5(TYPE_INT,12);
    vector<RID> rid_res_vector = s_hash_wrapper->Search(f5);
    for(auto&rid:rid_res_vector)rid.print();
    table_heap->displayTablePages();
    s_hash_wrapper->displayIndexPages();
    
    table_heap->saveMetaData();


    cout<<" loading index****************************************************************  "<<endl;
    // index :
    for(auto&[Pair, i]:table_heap->indexes_pages_ids){
        cout<<Pair.first<< " - - "<<Pair.second<<endl;
        cout<<"first and last page ids in the index : ";
        cout<<i->index_first_page_id<<" "<<i->index_last_page_id<<endl;

        char* page_buffer = BPM->fetchPage(i->index_first_page_id);
        PageHeader* header = reinterpret_cast<PageHeader*>(page_buffer);
        cout<<"PAGE AND NEXT PAGE IDS OF THE INND#EX"<<endl;
        cout<<header->page_id<<" next page id "<<header->next_page_id<<endl;
    }

    for(auto&[col_name,indexes_vec]:table_heap->indexes_map){

        cout<<"col name : "<<col_name<<endl;
        for(auto&i:indexes_vec){
            StaticHashIndexWrapper* s_hash_wrapper = static_cast<StaticHashIndexWrapper*>(i);
            s_hash_wrapper->displayIndexPages();
        }
    }

    cout << "==========================  testing savin& loading table and index data  ==========================" << endl;

    //s_hash_wrapper->~StaticHashIndexWrapper();

    
    int saved_first_page_id = table_heap->get_first_page_id();
    int saved_last_page_id = table_heap->get_last_page_id();
    cout<<"saved_last_page_id"<<saved_last_page_id<<endl;
    
    delete s_hash_wrapper;
    
    delete table_heap;
    delete BPM; 
    delete dm;

    // 4.boot up the new engine
    DiskManager* new_dm = new DiskManager("testDB");
    BufferPoolManager* new_BPM = new BufferPoolManager(new_dm);
    TableHeap* loaded_table_heap = new TableHeap(new_BPM,saved_first_page_id,saved_last_page_id);

    loaded_table_heap->loadMetaData();

    cout << "loaded table data" << endl;

    cout<<loaded_table_heap->get_first_page_id()<<endl;
    cout<<loaded_table_heap->get_last_page_id()<<endl;
    cout << loaded_table_heap->getTableName() << endl;
    loaded_table_heap->printColumns();
    loaded_table_heap->displayTablePages();

    cout<<" testing loading index****************************************************************"<<endl;
    // index :
    for(auto&[Pair, i]:loaded_table_heap->indexes_pages_ids){
        cout<<Pair.first<< " - - "<<Pair.second<<endl;
        cout<<"first and last page ids in the index : ";
        cout<<i->index_first_page_id<<" "<<i->index_last_page_id<<endl;
        
        char* page_buffer = new_BPM->fetchPage(i->index_first_page_id);
        PageHeader* header = reinterpret_cast<PageHeader*>(page_buffer);
        cout<<"PAGE AND NEXT PAGE IDS OF THE INND#EX"<<endl;
        cout<<header->page_id<<" next page id "<<header->next_page_id<<endl;
    }

    for(auto&[col_name,indexes_vec]:loaded_table_heap->indexes_map){

        cout<<"col name : "<<col_name<<endl;
        for(Index* i:indexes_vec){
            StaticHashIndexWrapper* s_hash_wrapper = static_cast<StaticHashIndexWrapper*>(i);
            if (s_hash_wrapper != nullptr) {
                    s_hash_wrapper->displayIndexPages(); 
                    cout<<"++"<<endl;
                }
        }
    }
    








}
*/