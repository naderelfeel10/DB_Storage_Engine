#ifndef INSERT_OPERATOR_H
#define INSERT_OPERATOR_H

#include"../Storage/Table/TableIterator.h"
#include"../Storage/Table/TableHeap.h"
#include"../Storage/Table/RID.h"
#include"../Storage/Table/Column.h"
#include"../Storage/Indexing/Index.h"
#include"../Storage/Indexing/StaticHashIndexWrapper.h"
#include"../Storage/Indexing/BPlusTreeIndexWrapper.h"
#include"./AbstractExecuter.h"
#include"./ComplexPredicate.h"
#include"./seq_scan_operator.h"
#include"AbstractPredicate.h"

using namespace std;

class InsertTuple : public AbstractExecuter{
    private:
        TableHeap* table_heap;
    public:
        InsertTuple(TableHeap* table_heap,Tuple tuple):table_heap(table_heap){
            RID rid = this->table_heap->insertTuple(tuple);  
            rid.print(); 
        }

        void open(){};
        void close(){};
        bool getNext(Tuple* tuple){return false;};
        TableHeap* getTableHeap(){return this->table_heap;}
        vector<Column> get_output_schema(){return {};}

        bool has_column(string col_name){return false;};
};

#endif
