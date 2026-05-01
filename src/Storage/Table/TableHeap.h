#ifndef page_table_H
#define page_table_H

#include<iostream>
#include"../Page/page.h"
#include"../Page/Tuple.h"
#include"../../Buffer/BufferPoolManager.h"
#include"RID.h"
#include"Column.h"

#include<map>
#include"../Indexing/static_hash_index.h"
#include"../Indexing/Index.h"
#include"../Indexing/StaticHashIndexWrapper.h"

#include"../Indexing/BPlusTreeIndex.h"
#include"../Indexing/BPlusTreeIndexWrapper.h"

#include<memory>



// todo : 
// 1.serialzation and deserialization of indexes
// 2.create table schema
// 3.

typedef enum{
    STATIC_HASH_INDEX,
    BPLUS_TREE_INDEX
}indexes_t;

struct Index_pages_struct{
    int index_first_page_id;
    int index_last_page_id;
    Index_pages_struct():index_first_page_id(-1),index_last_page_id(-1){}
};

class TableHeap{

    private:
        string table_name;
        int first_page_id = -1;
        int last_page_id = -1;
        vector<Column> cols;
        // to do :
        //number_of_tuples 

    public:
        // a map between col_name -> vector of indexes on this col ex : (HASH_Index, B+, ...)
        map<string, vector<Index*> > indexes_map;
        // to keep track of indexes pages.
        // for each pair of (col_name, index_type) we have a struct of first_page_id , last_page_id of this specific index 
        map< pair<string,indexes_t>, Index_pages_struct*> indexes_pages_ids;
        

        BufferPoolManager* BPM;
        TableHeap(BufferPoolManager* BPM);
        
        //crud :
        RID insertTuple(Tuple tuple);
        RID updateTuple(RID rid, Tuple tuple);
        void deleteTuple(RID rid);
        Tuple* getTuple(RID rid);

        
        void saveMetaData();
        void loadMetaData(); // to be impelemented

        vector<Column> getCols();
        void setCols(vector<Column> cols);

        string getTableName();
        void setTableName(string table_name);
        
        void displayTablePages();
        void printColumns();

        //
        void createIndex(indexes_t index_type, string string,int index_size);


        ~TableHeap();

};

#endif
