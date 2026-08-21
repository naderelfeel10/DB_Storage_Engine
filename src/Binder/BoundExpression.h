#ifndef BOUND_EXPRESSION_H
#define BOUND_EXPRESSION_H


#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Page\Field.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\Expression.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Table\Column.h"

#include<iostream>
#include<vector>
using namespace std;


struct BoundExpression {
    FieldType return_type;
    virtual ~BoundExpression() = default;
};

// i want to represent the bounded col like this :
// ColRef("age") : 
    /*BoundColumnRef
        table_oid = 10
        column_oid = 2
        return_type = INT*/

struct BoundColumnRef: BoundExpression{

    int table_oid;
    int column_oid;

    string table_name;
    string column_name;
};

/*
BoundTable
    oid = 10
    table_name = users
    alias = u
*/
struct BoundTable{

    int table_oid;

    string table_name;
    string alias;
    vector<Column> schema;
};

#endif