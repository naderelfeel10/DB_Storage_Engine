#include"update_statement_executer.h"
#include<iostream>
using namespace std;


bool UpdateTuple::is_updated(){return this->updated;}

bool UpdateTuple::getNext(Tuple* tuple){return false;};

TableHeap* UpdateTuple::getTableHeap(){return this->table_heap;}

vector<Column> UpdateTuple::get_output_schema(){return {};}

bool UpdateTuple::has_column(string col_name){return false;};

Tuple UpdateTuple::get_tuple(){
    return this->tuple;
}


bool UpdateTuple::update_tuple(RID rid, Tuple tuple){

    RID new_rid = this->table_heap->updateTuple(rid, tuple).getActualPair();
    //new_rid.print();
    
    if(new_rid.getPageId() !=-1 && new_rid.getSlotNum() != -1)
        updated = true;
    
    return updated;

}