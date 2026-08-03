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


struct TableInfo
{
        //uint32_t table_id;
        string table_name;
        vector<Column> schema;

        int first_page_id;
        TableHeap* table_heap;
        vector<IndexInfo> indexes;

};

typedef enum{
    STATIC_HASH_INDEX,
    BPLUS_TREE_INDEX
}indexes_t;

struct IndexInfo
{
    //uint32_t index_id;
    string name;
    string column_name;
    indexes_t type;
    int root_page;
};


class Catalog
{
    private:
        // table_name : table_info
        unordered_map<string,TableInfo*> tables;
        BufferPoolManager* BPM;
    public:
        Catalog(BufferPoolManager* BPM):BPM(BPM){}

        TableInfo* CreateTable(string table_name, vector<Column>schema);
        void DropTable(string table_name);
        TableInfo* GetTable(string table_name);

        bool TableExists(string table_name);
        vector<TableInfo*> GetTables();
};

#endif