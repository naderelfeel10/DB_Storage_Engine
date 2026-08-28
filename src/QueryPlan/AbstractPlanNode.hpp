#ifndef ABSTRACT_PLAN_NODE_H
#define ABSTRACT_PLAN_NODE_H

#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\BoundExpression.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\BoundSelectStatement.h"

using namespace std;

//multiple plan types like seq_scan, project, filer, ...
enum class PlanType {
    SEQ_SCAN,
    FILTER,
    PROJECTION,
    JOIN,
    AGGREGATION,
    SORT,
    LIMIT
};


class AbstractPlanNode{

public:
    PlanType type;
    virtual ~AbstractPlanNode() = default;

};

#endif