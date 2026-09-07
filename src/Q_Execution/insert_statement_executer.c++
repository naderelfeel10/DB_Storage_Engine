#include"insert_statement_executer.h"
#include<iostream>
using namespace std;


InsertTuple::InsertTuple(TableHeap* table_heap,Tuple tuple){
    this->tuple = tuple;
    this->table_heap = table_heap;
    RID rid = this->table_heap->insertTuple(tuple);  
    rid.print();
    
    if(rid.getPageId() !=-1 && rid.getSlotNum() != -1)
        inserted = true;
     
}


bool InsertTuple::is_inserted(){return this->inserted;}

bool InsertTuple::getNext(Tuple* tuple){return false;};

TableHeap* InsertTuple::getTableHeap(){return this->table_heap;}

vector<Column> InsertTuple::get_output_schema(){return {};}

bool InsertTuple::has_column(string col_name){return false;};

Tuple InsertTuple::get_tuple(){
    return this->tuple;
}


