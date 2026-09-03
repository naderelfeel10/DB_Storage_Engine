#ifndef SEQ_SCAN_H
#define SEQ_SCAN_H

#include"../Storage/Table/TableIterator.h"
#include"../Storage/Table/TableHeap.h"
#include"../Storage/Table/RID.h"
#include"../Storage/Table/Column.h"
#include"../Storage/Indexing/Index.h"
#include"../Storage/Indexing/StaticHashIndexWrapper.h"
#include"../Storage/Indexing/BPlusTreeIndexWrapper.h"
#include"./AbstractExecuter.h"
using namespace std;

class SeqScan: public AbstractExecuter{
    private:
        RID  curr_rid_pointer = RID(-1,-1);
        TableHeap* Table_heap;
        vector<Column> output_schema;
    public:
        SeqScan(TableHeap* table_heap);
        void open()override;
        void close()override;
        bool getNext(Tuple*tuple)override;
        TableHeap* getTableHeap()override{return this->Table_heap;}
        vector<Column> get_output_schema(){return this->output_schema;}

        bool has_column(string col_name);
};

#endif
