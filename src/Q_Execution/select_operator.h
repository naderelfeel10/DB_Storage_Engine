#ifndef SELECT_H
#define SELECT_H

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

class Select:public AbstractExecuter{
    private:
        AbstractExecuter* child_operator;
        AbstractPredicate* predicate;
    public:
        Select(AbstractExecuter* child_operator, AbstractPredicate* predicate):child_operator(child_operator),predicate(predicate){};
        void open();
        void close();
        bool getNext(Tuple* tuple);
        TableHeap* getTableHeap(){return this->child_operator->getTableHeap();}
        vector<Column> get_output_schema(){return this->child_operator->get_output_schema();}

};

#endif
