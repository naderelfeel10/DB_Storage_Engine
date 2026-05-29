#ifndef NESTED_LOOP_JOIN_H

#define NESTED_LOOP_JOIN_H

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

class NestedLoopJoin: public AbstractExecuter{
    
    private:
        AbstractExecuter* outer_table;
        AbstractExecuter* inner_table;
        AbstractPredicate* join_condition;
        vector<Column> output_schema;

        bool has_inner;
        Tuple curr_tuple;
    
    public:
        NestedLoopJoin(AbstractExecuter* outer_table,AbstractExecuter* inner_table,AbstractPredicate* join_condition):
        outer_table(outer_table),inner_table(inner_table),join_condition(join_condition),curr_tuple({}){}

        void open();
        void close();
        bool getNext(Tuple*tuple);

        TableHeap* getTableHeap(){return nullptr;};
        TableHeap* getOuterTableHeap(){return this->outer_table->getTableHeap();};
        TableHeap* getInnerTableHeap(){return this->inner_table->getTableHeap();};

        vector<Column> get_output_schema();

};

#endif
