#pragma once

#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Catalog\Catalog.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Executer\AbstractExecuter.hpp"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\QueryPlan\AbstractPlanNode.hpp"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\BindContext.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Q_Execution\seq_scan_operator.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Q_Execution\Projection_operator.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Q_Execution\ComplexPredicate.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Q_Execution\Predicate.h"

//in this executer i will use it to convert from BoundedStmts into actual component i use, then call it's operator
class ExecutorFactory{
private:
    Catalog* catalog;
    BindContext* context;

public:

    ExecutorFactory(Catalog* catalog, BindContext* context):catalog(catalog), context(context){}
    AbstractExecuter* createExecutor(AbstractPlanNode* plan);
    
    AbstractPredicate* BuildPredicate(BoundExpression* expression);
};