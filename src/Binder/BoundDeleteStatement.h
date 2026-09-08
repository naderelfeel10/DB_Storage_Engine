#ifndef DELETE_H
#define DELETE_H

#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\Expression.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\BoundExpression.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\BoundStatement.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\BoundSelectStatement.h"


#include<iostream>
#include<vector>
using namespace std;


class BoundDeleteStatement : public BoundStatement {
public:
    BoundTable* table;
    BoundExpression* where;

    BoundDeleteStatement() :table(nullptr), where(nullptr){}

    BoundStatementType type()const override{
        return BoundStatementType::DELETE;
    }

    //just printing

    void PrintTree() const override {
        cout << "|-- BoundDeleteStatement" << endl;

        if (table) {
            cout << "|   |-- Table: "
                 << table->table_name << endl;

            cout << "|   |-- table_oid: "
                 << table->table_oid << endl;
        }

        if (where) {
            cout << "|   |-- WHERE" << endl;
            where->PrintTree();
        }
        else {
            cout << "|   |-- WHERE: NULL (delete all)" << endl;
        }
    }
};

#endif