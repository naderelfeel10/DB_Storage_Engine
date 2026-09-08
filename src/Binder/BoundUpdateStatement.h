#ifndef UPDATE_H
#define UPDATE_H

#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\Expression.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\BoundExpression.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\BoundStatement.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\BoundSelectStatement.h"


#include<iostream>
#include<vector>
using namespace std;


class BoundUpdateStatement : public BoundStatement {
public:
    BoundTable* table;

    //cols to update
    vector<Column> columns;
    //values to update with
    vector<BoundExpression*> values;
    //the condtion
    BoundExpression* where;

    BoundUpdateStatement():table(nullptr),where(nullptr){}

    BoundStatementType type() const override{
        return BoundStatementType::UPDATE;
    }

    void PrintTree()const override{
        cout<<"update statement tree"<<endl;
    }
};

#endif