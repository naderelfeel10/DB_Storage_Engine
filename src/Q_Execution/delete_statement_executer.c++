#include"delete_statement_executer.h"
#include<iostream>
using namespace std;


bool DeleteTuple::is_deleted(){return this->deleted;}

bool DeleteTuple::getNext(Tuple* tuple){return false;};

TableHeap* DeleteTuple::getTableHeap(){return this->table_heap;}

vector<Column> DeleteTuple::get_output_schema(){return {};}

bool DeleteTuple::has_column(string col_name){return false;};



bool DeleteTuple::delete_tuple(RID rid){

    this->deleted =  this->table_heap->deleteTupleBool(rid);    
    return deleted;

}