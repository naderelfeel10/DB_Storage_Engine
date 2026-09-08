#ifndef ABSTRACT_PLAN_NODE_H
#define ABSTRACT_PLAN_NODE_H

#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\BoundExpression.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\BoundSelectStatement.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\BoundInsertStatement.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\BoundUpdateStatement.h"
using namespace std;

//multiple plan types like seq_scan, project, filer, ...
enum class PlanType {
    SEQ_SCAN,
    FILTER,
    PROJECTION,
    JOIN,
    AGGREGATION,
    SORT,
    LIMIT,
    //
    INSERT,
    UPDATE
};


class AbstractPlanNode{

public:
    PlanType type;
    virtual ~AbstractPlanNode() = default;

    virtual void PrintTree(int indent = 0) const = 0;
    
    static void PrintIndent(int indent){
        for(int i = 0; i < indent; i++){
            cout << "  ";
        }
    }
    
};

#endif