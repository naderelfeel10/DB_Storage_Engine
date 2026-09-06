#include"Planner.hpp"

AbstractPlanNode* Planner::Plan(unique_ptr<BoundStatement> statement){

    switch(statement->type()){
        //if stmt type is select call it's function to handle it
        case BoundStatementType::SELECT:{
            return PlanSelect(dynamic_cast<BoundSelectStatement*>(statement.get()));
        }

        default:
            throw runtime_error(" unsupported statement!!!!!!!!!!!");
    }
}


//plan select is the first actual plan 
AbstractPlanNode* Planner::PlanSelect(BoundSelectStatement* statement){

    //the order would be like this :
    //first from table to determine where to fetch the very first data
    int tabl_oid = statement->from_table.table_oid;
    AbstractPlanNode* plan = new SeqScanPlan(tabl_oid);

    //check for joins
    //we might get multiple joins in the same query
    for (auto& join : statement->joins){

        //extract right table
        AbstractPlanNode* right_plan = new SeqScanPlan(join.right_table.table_oid);
        //here plan represent the left side
        plan = new JoinPlan(join.type, join.condition, plan, right_plan);
    }

    //second is where, so we filter onlt needed rows
    if(statement->where != nullptr){
        plan = new FilterPlan(statement->where, plan);
    }

    // group by plan
    if(!statement->group_by.empty()){
        //prepare agg functions like avg, sum ...
        vector<BoundExpression*>functions;

        for(auto&item : statement->select_list){
            BoundExpression* expr = item.expression;
            if(expr->exp_type == BoundExpressionType::FUNCTION){
                functions.push_back(expr);
            }
        }
        
        plan = new GroupByPlan(statement->group_by, functions, plan,statement->having);
    }


    if(!statement->order_by.empty()) {
        plan = new OrderByPlan(statement->order_by[0], plan);
    }

    //then the projection to choose needed cols from the row
    plan = new ProjectionPlan(statement->select_list, plan);

    return plan;
}


/*int 
main(){

}*/