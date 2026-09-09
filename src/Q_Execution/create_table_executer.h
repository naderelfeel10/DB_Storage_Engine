#ifndef CREATE_TABLE_OPERATOR_H
#define CREATE_TABLE_OPERATOR_H

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
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Catalog\Catalog.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\BoundCreateTableStatement.h"
using namespace std;

class CreateTable : public AbstractExecuter{
    private:
        Catalog* catalog;
        bool created{false};

    public:

        CreateTable(Catalog* catalog, const BoundCreateTableStatement& statement): catalog(catalog){
            if(this->catalog->CreateTable(statement)){
                created=true;
            }
        }
        bool is_created();
        void open(){};
        void close(){};
        bool getNext(Tuple* tuple);
        
        bool update_tuple(RID rid, Tuple tuple);

        TableHeap* getTableHeap();
        vector<Column> get_output_schema();
        Tuple get_tuple();
        bool has_column(string col_name);
        
};

#endif
