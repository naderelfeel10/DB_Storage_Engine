#ifndef BOUND_CREATE_TABLE_STATEMENT_H
#define BOUND_CREATE_TABLE_STATEMENT_H

#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\Expression.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\BoundExpression.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\BoundStatement.h"

#include <string>
#include <vector>

using namespace std;



struct BoundForeignKey {
    vector<string> columns;
    string referenced_table;
    vector<string> referenced_columns;
};


enum class BoundConstraintType {
    PRIMARY_KEY,
    UNIQUE,
    FOREIGN_KEY
};


//table constraints : 
//type like : pk, fk, ..
//vector of col names
//referenced table name if found
//referenced cols if found

struct BoundTableConstraint {

    BoundConstraintType type;
    vector<string> columns;
    string referenced_table;
    vector<string> referenced_columns;
};



//table has :

//table_name
//schema
//bool to check if exists
//vector of cols
//vector of constraints

class BoundCreateTableStatement : public BoundStatement { 

public:

    string table_name;
    string schema_name;
    bool if_not_exists;

    vector<Column> columns;
    vector<BoundTableConstraint> constraints;

    BoundCreateTableStatement():table_name(""),schema_name(""),if_not_exists(false){}

    BoundStatementType type() const override {
            return BoundStatementType::CREATE_TABLE;
    }

    //just printing
    void PrintTree() const override {

        cout << "|-- BoundCreateTableStatement" << endl;

        cout << "|   |-- table_name: "
             << table_name << endl;

        cout << "|   |-- if_not_exists: "
             << (if_not_exists ? "true" : "false")
             << endl;


        cout << "|   |-- Columns" << endl;

        /*for (const auto& column : columns) {

            cout << "|   |   |-- "
                 << column.getColName()
                 << endl;

            column.printCol();
        }*/


        if (!constraints.empty()) {

            cout << "|   |-- Constraints"
                 << endl;

            for (const auto& constraint : constraints) {

                cout << "|   |   |-- ";

                switch (constraint.type) {

                    case BoundConstraintType::PRIMARY_KEY:
                        cout << "PRIMARY KEY";
                        break;

                    case BoundConstraintType::UNIQUE:
                        cout << "UNIQUE";
                        break;

                    case BoundConstraintType::FOREIGN_KEY:
                        cout << "FOREIGN KEY";
                        break;
                }

                cout << endl;
            }
        }
    }
};

#endif