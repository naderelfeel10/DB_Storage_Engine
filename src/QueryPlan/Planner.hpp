#ifndef PLANER_NODE_H
#define PLANER_NODE_H

#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\QueryPlan\AbstractPlanNode.hpp"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\QueryPlan\Plans.hpp"

using namespace std;

// the planner which takes an input statment, determines it's type then choose how to handle it
class Planner {

public:

    AbstractPlanNode* Plan(unique_ptr<BoundStatement> statement);

private:

    AbstractPlanNode* PlanSelect(BoundSelectStatement* statement);
};

#endif