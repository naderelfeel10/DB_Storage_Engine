#ifndef INDEX_NESTED_LOOP_JOIN_H
#define INDEX_NESTED_LOOP_JOIN_H

#include"../Storage/Table/TableIterator.h"
#include"../Storage/Table/TableHeap.h"
#include"../Storage/Table/RID.h"
#include"../Storage/Table/Column.h"
#include"../Storage/Indexing/Index.h"
#include"../Storage/Indexing/StaticHashIndexWrapper.h"
#include"../Storage/Indexing/BPlusTreeIndexWrapper.h"
#include"AbstractPredicate.h"
#include"AbstractExecuter.h"
#include"seq_scan_operator.h"
#include"select_operator.h"
#include"Projection_operator.h"
using namespace std;

class IndexedNestedLoopJoin: public AbstractExecuter{
    
    private:
        // outer table
        // inner index
        AbstractExecuter* outer_table;
        Index* inner_index;
        AbstractExecuter* inner_table;
        AbstractPredicate* join_condition;
        BufferPoolManager* BPM;

        Tuple curr_outer_tuple = Tuple({});
        vector<Column> output_schema;
        vector<RID> inner_matches;

        int col_index;

    
    public:
        IndexedNestedLoopJoin(BufferPoolManager* BPM,AbstractExecuter* outer_table,AbstractExecuter* inner_table,
        Index* inner_index, AbstractPredicate* join_condition):
        BPM(BPM),outer_table(outer_table),inner_table(inner_table), inner_index(inner_index),join_condition(join_condition){

            string col_index_name  = inner_index->get_index_col_name();
            cout<<col_index_name<<endl;
            this->col_index = this->outer_table->getTableHeap()->getColIndex(col_index_name);
        }

        void open();
        void close();
        bool getNext(Tuple*tuple);

        TableHeap* getTableHeap(){return nullptr;};
        TableHeap* getOuterTableHeap(){return this->outer_table->getTableHeap();};
        
        void getTuple(RID rid, Tuple& tuple);
        void set_output_schema();
        vector<Column> get_output_schema();

};
#endif