#ifndef page_table_H
#define page_table_H

#include<iostream>
#include"../Page/page.h"
#include"../Page/Tuple.h"
#include"../../Buffer/BufferPoolManager.h"
#include"RID.h"
#include"Column.h"





class TableHeap{

    private:
        string table_name;
        int first_page_id = -1;
        int last_page_id = -1;
        vector<Column> cols;    
            
    public:

        BufferPoolManager* BPM;
        TableHeap(BufferPoolManager* BPM);
        
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

        ~TableHeap();

};

#endif
