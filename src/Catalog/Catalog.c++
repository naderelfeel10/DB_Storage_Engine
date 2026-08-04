#include<iostream>
#include"Catalog.h"



TableInfo* Catalog::CreateTable(string table_name, vector<Column>schema){
    //check if table already exists :
    if(this->tables.find(table_name) != tables.end()){
        cerr<<"this table already exists"<<endl;
        return nullptr;       
    }else{
        //prepare table meta data, then create tableInfo
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

void Catalog::save_catalog(){
    //create a buffer to serialize data into
    char* buffer = new char[PAGE_SIZE];
    int index{0};
    
    //save first_page_id
    memcpy(buffer+index, &catalog_first_page_id, sizeof(catalog_first_page_id));
    index += sizeof(catalog_first_page_id);
    
    //save last_page_id
    memcpy(buffer+index, &catalog_last_page_id, sizeof(catalog_last_page_id));
    index += sizeof(catalog_last_page_id);

    //save tables size
    int number_of_tables = tables.size();
    cout<<number_of_tables<<endl;
    memcpy(buffer+index, &number_of_tables, sizeof(number_of_tables));
    index += sizeof(number_of_tables);

    //save actual table 
    for(auto&[table_name, tableInfo]:this->tables){
        tableInfo->serializeTableInfo(buffer+index);
        index+=tableInfo->getSize();
    }

    //now copy the whole buffer into the first page then save it to the disk
    char* page_buffer = this->BPM->fetchPage(catalog_first_page_id);
    memcpy(page_buffer, buffer, PAGE_SIZE);
    // mark as direty jsut to be presistent on disk
    BPM->markAsDirty(catalog_first_page_id);
    cout << "Saving to page: " << catalog_first_page_id << endl;
    //delete the buffer allocated
    delete[]buffer;

}

void Catalog::load_catalog(int page_id=1){
    //load the page 
    //by default it's the second page in the whole file (page_id =1)
    char* buffer = this->BPM->fetchPage(page_id);
    int offset{0};
    cout << "Loading from page: " << page_id << endl;
    //load data 

    //load first_page_id
    memcpy(&this->catalog_first_page_id,buffer+offset, sizeof(catalog_first_page_id));
    offset += sizeof(catalog_first_page_id);

    //load last_page_id
    memcpy(&this->catalog_last_page_id,buffer+offset, sizeof(catalog_last_page_id));
    offset += sizeof(catalog_last_page_id);

    //load tables size
    int number_of_tables;
    memcpy(&number_of_tables, buffer+offset, sizeof(number_of_tables));
    offset += sizeof(number_of_tables);

    cout<<"number of tables : "<<number_of_tables<<endl;
    tables.clear();
    //laod actual table 
    for(int i=0;i<number_of_tables;i++){
        TableInfo* table_info = new TableInfo();
        table_info->loadTableInfo(buffer+offset);
        offset+=table_info->getSize();

        string table_name=  table_info->table_name;
        tables[table_name] = table_info;
    }

}

int
main(){

    DiskManager* dm = new DiskManager("catalog.db");
    BufferPoolManager* BPM = new BufferPoolManager(dm);

    Catalog* catalog = new Catalog(BPM, true);
    string table_name = "User";

    Column t1_col1 = Column(TYPE_INT, "user_id", sizeof(int));
    Column t1_col2 = Column(TYPE_STRING, "firstName", 30);
    Column t1_col3 = Column(TYPE_STRING, "lastName", 30);
    Column t1_col4 = Column(TYPE_INT, "age", sizeof(int));
    vector<Column> user_schema = {t1_col1, t1_col2, t1_col3, t1_col4};

    catalog->CreateTable(table_name, user_schema);
    catalog->CreateTable("Order", user_schema);

    for(auto&[table_name, table_info]: catalog->getTables()){
        cout<<table_name<<endl;
        cout<<table_info->table_name<<endl;
        for(auto&col: table_info->schema){
            col.printCol();
        }
        cout<<endl;
    }
    cout<<"--------------"<<endl;

    catalog->save_catalog();
    //delete catalog;
    BPM->~BufferPoolManager();
    dm->~DiskManager();


    DiskManager* dm2 = new DiskManager("catalog.db");
    BufferPoolManager* BPM2 = new BufferPoolManager(dm2);

    Catalog* catalog2 = new Catalog(BPM2, false);
    //retest after loading
    catalog2->load_catalog();

    for(auto&[table_name, table_info]: catalog2->getTables()){
        cout<<table_name<<endl;
        cout<<table_info->table_name<<endl;
        for(auto&col: table_info->schema){
            col.printCol();
        }
        cout<<endl;
    }
    cout<<"--------------"<<endl;
}
