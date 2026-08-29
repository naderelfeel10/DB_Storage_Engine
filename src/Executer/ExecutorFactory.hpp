#pragma once

#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Catalog\Catalog.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Executer\AbstractExecuter.hpp"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\QueryPlan\AbstractPlanNode.hpp"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\BindContext.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Q_Execution\seq_scan_operator.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Q_Execution\Projection_operator.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Q_Execution\ComplexPredicate.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Q_Execution\Predicate.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Table\Column.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Page\Field.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Q_Execution\select_operator.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\binder.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\QueryPlan\Planner.hpp"


//in this executer i will use it to convert from BoundedStmts into actual component i use, then call it's operator
class ExecutorFactory{
private:
    Catalog* catalog;
    BindContext* context;

public:

    ExecutorFactory(Catalog* catalog, BindContext* context):catalog(catalog), context(context){}
    AbstractExecuter* createExecutor(AbstractPlanNode* plan);
    
    AbstractPredicate* build_predicate(BoundExpression* expression, AbstractExecuter* child);

    Column* expr_to_col(BoundExpression* expr,AbstractExecuter* child);
    Column* const_to_col(BoundConstantExpression* expr);

};