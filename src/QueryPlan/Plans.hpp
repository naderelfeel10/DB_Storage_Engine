#ifndef SEQ_SCAN_PLAN_NODE_H
#define SEQ_SCAN_PLAN_NODE_H

#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\QueryPlan\AbstractPlanNode.hpp"

using namespace std;

//first type is seq_scan
class SeqScanPlan : public AbstractPlanNode{

public:

    int table_oid;
    SeqScanPlan(int table_oid):table_oid(table_oid){
        type = PlanType::SEQ_SCAN;
    }
};


//now filter and it need the child node and a condition to filter on
class FilterPlan : public AbstractPlanNode {


public:

    BoundExpression* predicate;
    AbstractPlanNode* child;
    
    FilterPlan(BoundExpression* predicate, AbstractPlanNode* child):predicate(predicate),child(child){
        type = PlanType::FILTER;
    }

};

//projection and it need cols to select and the child node
class ProjectionPlan :public AbstractPlanNode{

public:

    //vector of cols
    vector<BoundSelectItem> expressions;
    AbstractPlanNode* child;
    
    ProjectionPlan(const vector<BoundSelectItem>& expressions, AbstractPlanNode* child):expressions(expressions),child(child){
        type = PlanType::PROJECTION;
    }
};


#endif