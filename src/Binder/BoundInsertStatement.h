#ifndef INSERT_H
#define INSERT_H

#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\Expression.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\BoundExpression.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\BoundStatement.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\BoundSelectStatement.h"


#include<iostream>
#include<vector>
using namespace std;


//bound insert statement is gonna be like :
//insert into User (user_id, age) values [1,20],[2,30];
class BoundInsertStatement : public BoundStatement {

public:
    //table to insert into
    //cols to insert into 
    //values 

    BoundTable* table;
    vector<Column> columns;
    vector<BoundExpression*> values;

    BoundInsertStatement(){    
    }
    BoundInsertStatement(BoundTable* table, vector<Column> columns, vector<BoundExpression*> values)
        :table(table), columns(columns), values(values){
        }

    BoundStatementType type() const override {
            return BoundStatementType::INSERT;
    }

    BoundInsertStatement& operator=(const BoundInsertStatement& other){
        if(this != &other){
            this->table = other.table;
            this->columns = other.columns;
            this->values = other.values;
        }
        return *this;
    }


    //just printing
    void PrintTree() const override {
        cout << "BoundInsertStatement\n";

        // INTO
        cout << "|-- INTO\n";

        if (table) {
            table->printTable();
        } else {
            cout << "|   NULL\n";
        }
    }

};

#endif