#include"create_table_executer.h"
#include<iostream>
using namespace std;


bool CreateTable::is_created(){return this->created;}

bool CreateTable::getNext(Tuple* tuple){return false;};

TableHeap* CreateTable::getTableHeap(){
    return nullptr;
}

vector<Column> CreateTable::get_output_schema(){return {};}

bool CreateTable::has_column(string col_name){return false;};

Tuple CreateTable::get_tuple(){
    return Tuple({});
}

