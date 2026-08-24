#ifndef BOUND_CONTEXT_H
#define BOUND_CONTEXT_H

#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\Expression.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\BoundExpression.h"

#include<iostream>
#include<vector>
using namespace std;

//the context represents the currently visible namespace
class BindContext {

public:
    void AddTable(const BoundTable& table);
    BoundColumnRef* ResolveColumn(const string& table_name, const string& column_name);

    vector<BoundTable> tables;
};

#endif