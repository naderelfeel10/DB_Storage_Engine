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
        AbstractExecuter* outer_table;
        Index* inner_index;
        TableHeap* inner_table_heap;
        AbstractPredicate* join_condition;

        Tuple curr_tuple;
        vector<Column> output_schema;
        vector<RID> curr_matching_rids;
        int curr_rid_index{0};
    
    public:
        IndexedNestedLoopJoin(AbstractExecuter* outer_table,Index* inner_index ,TableHeap* inner_table_heap, AbstractPredicate* join_condition):
        outer_table(outer_table),inner_index(inner_index), inner_table_heap(inner_table_heap),join_condition(join_condition),curr_tuple({}){}

        void open();
        void close();
        bool getNext(Tuple*tuple);

        TableHeap* getTableHeap(){return nullptr;};
        TableHeap* getOuterTableHeap(){return this->outer_table->getTableHeap();};
        TableHeap* getInnerTableHeap(){return this->inner_table_heap;};

        vector<Column> get_output_schema();

};

#endif
