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

    //second is where, so we filter onlt needed rows
    if(statement->where != nullptr){
        plan = new FilterPlan(statement->where, plan);
    }

    //then the projection to choose needed cols from the row
    plan = new ProjectionPlan(statement->select_list, plan);

    return plan;
}


/*int 
main(){

}*/