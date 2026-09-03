#ifndef CATALOG_H
#define CATALOG_H

#include"../Storage/Table/TableIterator.h"
#include"../Storage/Table/TableHeap.h"
#include"../Storage/Table/RID.h"
#include"../Storage/Table/Column.h"
#include"../Storage/Indexing/Index.h"
#include"../Storage/Indexing/StaticHashIndexWrapper.h"
#include"../Storage/Indexing/BPlusTreeIndexWrapper.h"
using namespace std;


struct IndexInfo
{
    int offset = {0};
    
    //uint32_t index_id;
    string name;
    string column_name;
    indexes_t type;
    int root_page;

    void serializeIndex(char* buffer);
    void loadIndex(const char *buffer);
    int getSize();
    
};



struct TableInfo
{
public:
        int offset{0};
        
        //uint32_t table_id;
        string table_name;
        int table_id;
        vector<Column> schema;

        int first_page_id;
        TableHeap* table_heap;
        vector<IndexInfo> indexes;

        void serializeTableInfo(char* buffer);
        void loadTableInfo(char *buffer);
        int getSize();
   
        TableHeap*get_table_heap(){
            return this->table_heap;
        }
};


class Catalog
{
    private:

        // table_name : table_info
        unordered_map<string,TableInfo*> tables;
        BufferPoolManager* BPM;
        int next_table_id{0};
        int catalog_first_page_id{-1};
        int catalog_last_page_id{-1};

        //int number_of_tables;


    public:
        Catalog(BufferPoolManager* BPM, bool createNew);

        TableInfo* CreateTable(const string& table_name, const vector<Column>&schema);
        void DropTable(string table_name);
        TableInfo* GetTable(string table_name);

        bool TableExists(string table_name);
        vector<TableInfo*> GetTables();

        void save_catalog();
        void load_catalog(int catalog_first_page_id);

        unordered_map<string,TableInfo*> getTables(){
            return this->tables;
        }


};

#endif