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
        int offset{0};
        
        //uint32_t table_id;
        string table_name;
        vector<Column> schema;

        int first_page_id;
        TableHeap* table_heap;
        vector<IndexInfo> indexes;

        void serializeTableInfo(char* buffer){
            // save table_name length
            int table_name_size = table_name.length();
            memcpy(buffer+offset, &table_name_size, sizeof(table_name_size));
            offset += sizeof(table_name_size);

            //save first page_id of the table
            memcpy(buffer+offset, &first_page_id, sizeof(first_page_id));
            offset += sizeof(first_page_id);

            // save table schema
            for(auto&col:schema){
                //this function serializes the whole col adn saved into buffer
                col.serializeCol(buffer+offset);
                offset += col.getColSize();   
            }

            // save table indexes:
            for(auto&index:indexes){
                index.serializeIndex(buffer+offset);
                offset += index.getSize();
            }

        }

        int getSize(){return offset;}

};


typedef enum{
    STATIC_HASH_INDEX,
    BPLUS_TREE_INDEX
}indexes_t;

struct IndexInfo
{
    int offset = {0};
    
    //uint32_t index_id;
    string name;
    string column_name;
    indexes_t type;
    int root_page;
    

    void serializeIndex(char* buffer){
        //int index{0};
        // save index_name len (to be able to load it dynamically)
        int index_name_size = name.length();
        memcpy(buffer+offset, &index_name_size, sizeof(index_name_size));
        offset += sizeof(index_name_size);
 
        //same with col_name that the index is built on
        int col_name_size = column_name.length();
        memcpy(buffer+offset, &col_name_size, sizeof(col_name_size));
        offset += sizeof(col_name_size);
        // save the type of the index
        memcpy(buffer+offset, &type, sizeof(type));
        offset += sizeof(type);
        //save first page_id of the index where so coiuld be loaded later
        memcpy(buffer+offset, &root_page, sizeof(root_page));
        offset += sizeof(root_page);
    }
    
    int getSize(){return offset;}
    
    
};


class Catalog
{
    private:

        // table_name : table_info
        unordered_map<string,TableInfo*> tables;
        BufferPoolManager* BPM;

        int catalog_first_page_id{-1};
        int catalog_last_page_id{-1};

        //int number_of_tables;


    public:
        Catalog(BufferPoolManager* BPM):BPM(BPM){
            this->catalog_last_page_id = this->BPM->newPage();
        }

        TableInfo* CreateTable(string table_name, vector<Column>schema);
        void DropTable(string table_name);
        TableInfo* GetTable(string table_name);

        bool TableExists(string table_name);
        vector<TableInfo*> GetTables();

        void save_catalog(){
            char* buffer = this->BPM->fetchPage(catalog_first_page_id);
            int index = sizeof(PageHeader);
            
            memcpy(buffer+index, &catalog_first_page_id, sizeof(catalog_first_page_id));
            index += sizeof(catalog_first_page_id);

            memcpy(buffer+index, &catalog_last_page_id, sizeof(catalog_last_page_id));
            index += sizeof(catalog_last_page_id);

            int number_of_tables = tables.size();
            memcpy(buffer+index, &number_of_tables, sizeof(number_of_tables));
            index += sizeof(number_of_tables);

        }

        void load_catalog(){

        }

};

#endif