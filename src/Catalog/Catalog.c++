#include<iostream>
#include"Catalog.h"



TableInfo* Catalog::CreateTable(string table_name, vector<Column>schema){
    //check if table already exists :
    if(this->tables.find(table_name) != tables.end()){
        cerr<<"this table already exists"<<endl;
        return nullptr;       
    }else{
        TableHeap* heap = new TableHeap(BPM, -1, -1);
        auto* info = new TableInfo();
        info->table_name = table_name;
        info->schema = schema;
        info->table_heap = heap;
        info->first_page_id = heap->get_first_page_id();

        tables[table_name] = info;

        return info;
    }

}

void Catalog::DropTable(string table_name){
    TableInfo* info = tables[table_name];
    //delete the actual table from heap
    info->table_heap->deleteTableHeap();
    //delete from teh catalog
    tables.erase(table_name);

}

TableInfo* Catalog::GetTable(string table_name){
    if(tables.find(table_name) != tables.end())
        return tables[table_name];

    return nullptr;
}


bool Catalog::TableExists(string table_name){
    if(tables.find(table_name) != tables.end())
        return true;

    return false;
}

vector<TableInfo*> Catalog::GetTables(){
    vector<TableInfo*> res;
    for(auto&[table_name, info] : this->tables){
        res.push_back(info);
    }
    return res;
}



int
main(){

}
