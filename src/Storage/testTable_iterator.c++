#include<iostream>
#include<vector>
#include<algorithm>
#include"Table/TableIterator.h"
#include"Table/Column.h"

using namespace std;


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
    
    vector<Column> cols = {col1, col2, col3, col4};
    table_heap->setCols(cols);
    table_heap->setTableName(table_name);

    table_heap->printColumns();



    //insert 10 of these as a test
    string query3 = "insert into User(user_id, firstName, lastName, age) values(0, \"nader\", \"elfeel\", 21.8);";
    
    vector<RID> table_rids;
    for(int i=0;i<10;i++){
    
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



    //////

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
    }


}