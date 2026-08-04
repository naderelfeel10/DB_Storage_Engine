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
    

    void serializeIndex(char* buffer){
        //int index{0};
        offset = 0;

        // save index_name len (to be able to load it dynamically)
        int index_name_size = name.length();
        memcpy(buffer+offset, &index_name_size, sizeof(index_name_size));
        offset += sizeof(index_name_size);
 
        //save index name
        memcpy(buffer+offset, name.data(), name.size());
        offset += name.size();

        //same with col_name that the index is built on
        int col_name_size = column_name.length();
        memcpy(buffer+offset, &col_name_size, sizeof(col_name_size));
        offset += sizeof(col_name_size);

        //save column name
        memcpy(buffer+offset, column_name.data(), column_name.size());
        offset += column_name.size();

        // save the type of the index
        memcpy(buffer+offset, &type, sizeof(type));
        offset += sizeof(type);

        //save first page_id of the index where so coiuld be loaded later
        memcpy(buffer+offset, &root_page, sizeof(root_page));
        offset += sizeof(root_page);
    }

    void loadIndex(const char *buffer){
        offset = 0;

        //load index name length
        int len; 
        memcpy(&len, buffer +offset, sizeof(len));
        offset += sizeof(len);

        //load index name
        name.assign(buffer+offset, len);
        offset += len;

        //load col name size
        memcpy(&len, buffer + offset, sizeof(len));
        offset += sizeof(len);

        // load col name
        column_name.assign(buffer + offset, len);
        offset += len;

        //load index type
        memcpy(&type, buffer + offset, sizeof(type));
        offset += sizeof(type);

        //load index page_id
        memcpy(&root_page, buffer + offset, sizeof(root_page));
        offset += sizeof(root_page);
    }

    int getSize(){return offset;}
    
};



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
            offset = 0;
            // save table_name length
            int table_name_size = table_name.length();
            memcpy(buffer+offset, &table_name_size, sizeof(table_name_size));
            offset += sizeof(table_name_size);

            //save table_name
            memcpy(buffer+offset, table_name.data(), table_name.size());
            offset += table_name.size();

            //save first page_id of the table
            memcpy(buffer+offset, &first_page_id, sizeof(first_page_id));
            offset += sizeof(first_page_id);

            //save number of cols in schema
            int schema_size = schema.size();
            memcpy(buffer+offset, &schema_size, sizeof(schema_size));
            offset += sizeof(schema.size());

            // save each col
            for(auto& c : this->schema){
                c.printCol();
                switch (c.getColType()){
                case TYPE_INT:
                    {buffer[offset] = 'I'; break;}
                case TYPE_STRING:
                    {buffer[offset] = 'S'; break;}
                case TYPE_FLOAT:
                    {buffer[offset] = 'F'; break;}
                case TYPE_BOOL:
                    {buffer[offset] = 'B'; break;}
                default:
                    cerr<<"incorrect field type"<<endl;
                    break;
                }
                offset+=1;
                cout<<"from save table meta"<<endl;
                c.serializeCol(buffer+ offset);
                offset += c.getColSize();
            }

            //save number of indexes
            int indexes_size = indexes.size();
            memcpy(buffer+offset, &indexes_size, sizeof(indexes_size));
            offset += sizeof(indexes.size());

            // save table indexes:
            for(auto&index:indexes){
                index.serializeIndex(buffer+offset);
                offset += index.getSize();
            }

        }

        void loadTableInfo(char *buffer){
            offset = 0;
        
            int len;
            //load table name len
            memcpy(&len, buffer + offset, sizeof(len));
            offset += sizeof(len);
            //load table name
            table_name.assign(buffer + offset, len);
            offset += len;
        
            //load the first page id
            memcpy(&first_page_id, buffer + offset, sizeof(first_page_id));
            offset += sizeof(first_page_id);
        
            //load schema size
            int schema_size;
            memcpy(&schema_size, buffer + offset, sizeof(schema_size));
            offset += sizeof(schema_size);
        
            schema.clear();
            //load every col and add it to the schema

            for(int i=0;i<schema_size;i++){
                char type = buffer[offset];
                //cout<<"Type : "<<type<<endl;
                FieldType field_type;
                switch (type)
                {
                case 'I':
                    {field_type =  TYPE_INT; break;}
                case 'S':
                    {field_type =  TYPE_STRING; break;}
                case 'F':
                    {field_type =  TYPE_FLOAT; break;}
                case 'B':
                    {field_type =  TYPE_BOOL; break;}
                default:
                    cerr<<"invalid field type"<<endl;
                    break;
                }
                offset+=1;
                Column c(field_type,"",1);
                c.deSerializeCol(buffer+ offset);
                //c.getField()->print();
                //c.printCol();
                this->schema.push_back(c);
                offset += c.getColSize();
            }

            //load index size
            int indexes_size;
            memcpy(&indexes_size, buffer + offset, sizeof(indexes_size));
            offset += sizeof(indexes_size);
        
            indexes.clear();
            // load all indexes
            for (int i = 0; i < indexes_size; i++){
                IndexInfo idx;
                idx.loadIndex(buffer + offset);
                // add index size then push table indexes
                offset += idx.getSize();
                indexes.push_back(idx);
            }
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
        Catalog(BufferPoolManager* BPM, bool createNew):BPM(BPM){
            if(createNew){
                // allocate a page for first and last  page
                this->catalog_first_page_id = this->BPM->newPage();
                this->catalog_last_page_id = this->BPM->newPage();
                cout<<"first page id : "<<catalog_first_page_id<<endl;;
                cout<<"last page id : "<<catalog_last_page_id<<endl;

                //update next pointer of  first page to the last one
                char* buffer = this->BPM->fetchPage(catalog_first_page_id);
                PageHeader* header = reinterpret_cast<PageHeader*>(buffer);
                header->next_page_id = catalog_last_page_id;

                //mark as dirty just to be saved to disk later
                this->BPM->markAsDirty(catalog_first_page_id);
            }

        }

        TableInfo* CreateTable(string table_name, vector<Column>schema);
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