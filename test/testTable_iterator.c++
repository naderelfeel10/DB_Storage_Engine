#include<iostream>
#include<vector>
#include<algorithm>
#include"Table/TableIterator.h"
#include"Table/Column.h"
#include"Indexing/Index.h"
#include"Indexing/StaticHashIndexWrapper.h"
#include<chrono>
#include<cassert>
#include"Indexing/BPlusTreeIndexWrapper.h"

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

    Field* f1 = new Field(col1_type);
    Field* f2 = new Field(col2_type);
    Field* f3 = new Field(col3_type);
    Field* f4 = new Field(col4_type);

    Column col1 = Column(f1,col1_name,sizeof(int));
    Column col2 = Column(f2,col2_name,30);
    Column col3 = Column(f3,col3_name,30);
    Column col4 = Column(f4,col4_name,sizeof(int));

    TableHeap* table_heap = new TableHeap(BPM);
    dm->addTable(table_name,table_heap->get_first_page_id());
    
    vector<Column> cols = {col1, col2, col3, col4};
    table_heap->setCols(cols);
    table_heap->setTableName(table_name);

    table_heap->printColumns();



    //insert 10 of these as a test
    string query3 = "insert into User(user_id, firstName, lastName, age) values(0, \"nader\", \"elfeel\", 21.8);";
    
    vector<RID> table_rids;
    for(int i=0;i<500;i++){
    
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
    
    

    TableIterator table_iterator(table_heap,table_rids.front(), table_rids.back());

    
    string query4 = "select * from User where user_id=7";

    int user_id_value = 7;

    cout<<"displaying tuple with user_id = 7"<<endl;

    auto st1 = chrono::high_resolution_clock::now();
    
    for(; !table_iterator.end(); ++table_iterator){
        Tuple t =  *table_iterator;

        for(auto& f:t.fields){

            if(f.getFieldType() == TYPE_INT){
                if(f.getFieldValueInt() == user_id_value){
                    t.print();
                    break;
                }
            }
        }
    }
    auto end1 = chrono::high_resolution_clock::now();


    auto duration1 = chrono::duration_cast<chrono::microseconds>(end1 - st1);
    cout << "searching using linear scan  duration: " << duration1.count() << " microseconds" << endl;


    //////

    string query5 = "update User firstName = \"GOAT\" where user_id=5";

    user_id_value = 5;
    string new_f_name = "GOAT";

    cout<<"updating tuple with user_id = 5"<<endl;


    table_iterator = TableIterator(table_heap,table_rids.front(), table_rids.back());
    for (; !table_iterator.end(); ++table_iterator) {

    Tuple t = *table_iterator; 
    bool match = false;
    
        for(auto& f:t.fields){

            if(f.getFieldType() == TYPE_INT){
                if(f.getFieldValueInt() == user_id_value){
                    match = true;
                }
            }
        }

    if (match) {

        vector<Field> updated_fields = t.fields; 
        
        Field new_field(TYPE_STRING, new_f_name.c_str());

        updated_fields.erase(updated_fields.begin() + 1);
        updated_fields.insert(updated_fields.begin() + 1, new_field);
        
        Tuple new_tuple(updated_fields);

        RID curr_rid = table_iterator.getCurrRIDPointer(); 
        RID new_rid = table_heap->updateTuple(curr_rid, new_tuple);
        
        cout << "Update successful!" << endl;
        break;
    }
}



    ////////////////////////////////////////////////////

    string query6 = "delete from User where user_id=8";

    user_id_value = 8;

    cout<<"deleting tuple with user_id = 8"<<endl;


    table_iterator = TableIterator(table_heap,table_rids.front(), table_rids.back());

    for (; !table_iterator.end(); ++table_iterator) {

        Tuple t = *table_iterator; 
        bool match = false;
    
            for(auto& f:t.fields){

                if(f.getFieldType() == TYPE_INT){
                    if(f.getFieldValueInt() == user_id_value){
                        match = true;
                    }
                }
            }

        if (match) {
            RID curr_rid = table_iterator.getCurrRIDPointer();
            table_heap->deleteTuple(curr_rid);
            break;
            cout<<"deleted successfuly"<<endl;
        }
    }


cout<<"---------------------------------------"<<endl;

    table_iterator = TableIterator(table_heap,table_rids.front(), table_rids.back());
    for(; !table_iterator.end(); ++table_iterator){
        Tuple t =  *table_iterator;
        t.print();
        Field f(TYPE_INT, 3);
        if(find(t.fields.begin(), t.fields.end(),f) != t.fields.end()){
            cout<<"target field found"<<endl;
        }else{
            cout<<"target field not found"<<endl;
        }
            break;
    }


    cout<<"================================================"<<endl;
    cout<<"testing static hash iindex"<<endl;
    cout<<"================================================"<<endl;

    
    string query7 = "CREATE INDEX idx_stu_id_hash ON User USING HASH (stu_id)";
    
    //col1_name is "stu_id"
    table_heap->createIndex(STATIC_HASH_INDEX,col1_name,600);

    Index* static_hash_index_wrapper =  table_heap->indexes_map[col1_name][0];
    table_iterator = TableIterator(table_heap,table_rids.front(), table_rids.back());

    for(;!table_iterator.end();++table_iterator){

        Tuple t = *table_iterator;
        
        int counter{0};
        for(auto& col:table_heap->getCols()){
            if(col.getColName()==col1_name){

                //col.getField()->print();
                t.fields[counter].print();
                static_cast<StaticHashIndexWrapper*>(static_hash_index_wrapper)->Insert(t.fields[counter],col1_name,table_heap->getCols(),table_iterator.getCurrRIDPointer());
                break;
            }
            counter++;

        }
    
    }


    cout<<"done"<<endl;

    // cases when field is found
    auto start = chrono::high_resolution_clock::now();

    {
        Field search_index1(TYPE_INT, 450);
        RID search_index1_RID =  static_cast<StaticHashIndexWrapper*>(static_hash_index_wrapper)->Search(search_index1)[0];
        cout<<"search_index1 RID : "<<"( "<<search_index1_RID.getPageId()<<", "<<search_index1_RID.getActualPair().getSlotNum()<<" )"<<endl;
    }

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "searching using static hash index duration: " << duration.count() << " microseconds" << endl;



    Field search_index2(TYPE_INT, 200);
    RID search_index2_RID =  static_cast<StaticHashIndexWrapper*>(static_hash_index_wrapper)->Search(search_index2)[0];
    cout<<"search_index2 RID : "<<"( "<<search_index2_RID.getPageId()<<", "<<search_index2_RID.getActualPair().getSlotNum()<<" )"<<endl;

    
    // cases when field is not found should return (-1,-1)

    Field search_index3(TYPE_INT, 7191);
    RID search_index3_RID =  static_cast<StaticHashIndexWrapper*>(static_hash_index_wrapper)->Search(search_index3)[0];
    cout<<"search_index3 RID : "<<"( "<<search_index3_RID.getPageId()<<", "<<search_index3_RID.getActualPair().getSlotNum()<<" )"<<endl;
    assert(search_index3_RID.getActualPair().getPageId() == -1 && search_index3_RID.getActualPair().getSlotNum() == -1);


    // test delete hash index
    static_cast<StaticHashIndexWrapper*>(static_hash_index_wrapper)->Delete(search_index2);

    // try to search again
    search_index2_RID =  static_cast<StaticHashIndexWrapper*>(static_hash_index_wrapper)->Search(search_index2)[0];
    cout<<"search_index2 RID : "<<"( "<<search_index2_RID.getActualPair().getPageId()<<", "<<search_index2_RID.getActualPair().getSlotNum()<<" )"<<endl;
    assert(search_index2_RID.getActualPair().getPageId() == -1 && search_index2_RID.getActualPair().getSlotNum() == -1);


    table_iterator = TableIterator(table_heap, table_rids.front(), table_rids.back());

    start = chrono::high_resolution_clock::now();

    {
        for(int i=4;i<=400;i++){
            Field f(TYPE_INT, i);
            RID rid = static_cast<StaticHashIndexWrapper*>(static_hash_index_wrapper)->Search(f)[0];
            cout<<"search_index1 RID : "<<"( "<<rid.getPageId()<<", "<<rid.getActualPair().getSlotNum()<<" )"<<endl;
        }
                
    }

    end = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "searching in range 4:400 using static hash index  duration: " << duration.count() << " microseconds" << endl;



    
    cout<<"================================================"<<endl;
    cout<<"testing B plus Tree  iindex"<<endl;
    cout<<"================================================"<<endl;

    table_heap->createIndex(BPLUS_TREE_INDEX, col1_name, 8);

    for(auto& i:table_heap->indexes_map){
        cout<<i.first<<" ";
        for(auto& h:i.second)cout<<h<<" ";
        cout<<endl;
    }
    Index* B_plusTreeIndex = table_heap->indexes_map[col1_name][1];
    table_iterator = TableIterator(table_heap,table_rids.front(), table_rids.back());


    for(;!table_iterator.end();++table_iterator){

        Tuple t = *table_iterator;
        
        int counter{0};
        for(auto& col:table_heap->getCols()){
            if(col.getColName()==col1_name){

                //col.getField()->print();
                t.fields[counter].print();
                static_cast<BPlusTreeIndexWrapper*>(B_plusTreeIndex)->Insert(t.fields[counter],col1_name,table_heap->getCols(),table_iterator.getCurrRIDPointer());
                break;
            }
            counter++;

        }
    
    }

    cout<<"done"<<endl;

    // cases when field is found
    auto start2 = chrono::high_resolution_clock::now();

    {
        Field search_index1(TYPE_INT, 450);
        RID search_index1_RID =  static_cast<BPlusTreeIndexWrapper*>(B_plusTreeIndex)->Search(search_index1)[0];
        cout<<"search_index1 RID : "<<"( "<<search_index1_RID.getPageId()<<", "<<search_index1_RID.getActualPair().getSlotNum()<<" )"<<endl;
    }

    auto end2 = chrono::high_resolution_clock::now();
    auto duration2 = chrono::duration_cast<chrono::microseconds>(end2 - start2);
    cout << "searching using B plus tree index duration: " << duration2.count() << " microseconds" << endl;

    
     start2 = chrono::high_resolution_clock::now();

    {
        Field lower_index(TYPE_INT, 4);
        Field upper_index(TYPE_INT, 400);
        
        vector<RID> search_index_RIDs =  static_cast<BPlusTreeIndexWrapper*>(B_plusTreeIndex)->searchRange(lower_index, upper_index);
        for(auto& rid: search_index_RIDs)
        cout<<"search_index1 RID : "<<"( "<<rid.getPageId()<<", "<<rid.getActualPair().getSlotNum()<<" )"<<endl;
    }
    end2 = chrono::high_resolution_clock::now();
    duration2 = chrono::duration_cast<chrono::microseconds>(end2 - start2);
    cout << "searching in range 4:400 using B plus tree index duration: " << duration2.count() << " microseconds" << endl;
}
*/