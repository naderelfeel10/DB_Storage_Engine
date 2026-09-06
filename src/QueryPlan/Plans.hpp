#ifndef SEQ_SCAN_PLAN_NODE_H
#define SEQ_SCAN_PLAN_NODE_H

#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\QueryPlan\AbstractPlanNode.hpp"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Q_Execution\HashAggregateExecuter.h"

using namespace std;

//first type is seq_scan
class SeqScanPlan : public AbstractPlanNode{

public:

    int table_oid;
    SeqScanPlan(int table_oid):table_oid(table_oid){
        type = PlanType::SEQ_SCAN;
    }

    //just printing
    void PrintTree(int indent = 0) const override{

        PrintIndent(indent);
        cout << "SeqScan"
             << " [table_oid=" << table_oid << "]"
             << endl;
    
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

    //printing
    void PrintTree(int indent = 0)const override {

        PrintIndent(indent);

        cout << "Filter";
        cout << endl;
        if (child != nullptr) {
            child->PrintTree(indent + 1);
        }
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
    //printing
    void PrintTree(int indent = 0)const override {

        PrintIndent(indent);

        cout << "Projection";
        cout << endl;
        if (child != nullptr) {
            child->PrintTree(indent + 1);
        }
    }
};


//join plan, it has join type, condtion, left and right childs
class JoinPlan : public AbstractPlanNode{
private:
    JoinType join_type;
    BoundExpression* condition;
    AbstractPlanNode* left;
    AbstractPlanNode* right;

public:
    JoinPlan(JoinType join_type, BoundExpression* condition, AbstractPlanNode* left, AbstractPlanNode* right){

        this->type = PlanType::JOIN;
        this->join_type = join_type;
        this->condition = condition;
        this->left = left;
        this->right = right;
    }

    //getters
    JoinType getJoinType(){
        return join_type;
    }

    BoundExpression* getCondition(){
        return condition;
    }

    AbstractPlanNode* getLeft(){
        return left;
    }

    AbstractPlanNode* getRight(){
        return right;
    }

    //just printing
    void PrintTree(int indent = 0) const override{

        PrintIndent(indent);

        cout << "JOIN";

        switch (join_type) {

            case JoinType::INNER:
                cout << " [INNER]";
                break;

            case JoinType::LEFT:
                cout << " [LEFT]";
                break;

            case JoinType::RIGHT:
                cout << " [RIGHT]";
                break;
        }

        cout << endl;


        // Print join condition
        if (condition != nullptr) {

            PrintIndent(indent + 1);
            cout << "Condition:" << endl;

            condition->PrintTree(
                "|   ",
                true
            );
        }


        // Print left child
        PrintIndent(indent + 1);
        cout << "Left:" << endl;

        if (left != nullptr) {
            left->PrintTree(indent + 2);
        }


        // Print right child
        PrintIndent(indent + 1);
        cout << "Right:" << endl;

        if (right != nullptr) {
            right->PrintTree(indent + 2);
        }
    }
    
};

//same idea with order_By
class OrderByPlan : public AbstractPlanNode{

private:

    BoundOrderBy order_by;
    AbstractPlanNode* child;

public:
    
    OrderByPlan(BoundOrderBy order_by, AbstractPlanNode* child):order_by(order_by),child(child){
        this->type = PlanType::SORT;
    }
    BoundOrderBy getOrderBy(){
        return order_by;
    }
    AbstractPlanNode* getChild(){
        return child;
    }
    BoundExpression* getExpression(){
        return this->order_by.expression;
    }
    OrderType getOrderType(){
        return this->order_by.order_type;
    }

    //just printing
    void PrintTree(int indent = 0)const override{
        PrintIndent(indent);

        cout << "ORDER BY";

        // Print ASC / DESC
        switch (order_by.order_type) {
            case OrderType::ASC:
                cout << " [ASC]";
                break;

            case OrderType::DESC:
                cout << " [DESC]";
                break;
        }

        cout << endl;

        // Print sort expression
        if (order_by.expression != nullptr) {

            PrintIndent(indent + 1);
            cout << "Expression:" << endl;

            order_by.expression->PrintTree(
                "|   ",
                true
            );
        }

        // Print child
        PrintIndent(indent + 1);
        cout << "Child:" << endl;

        if (child != nullptr) {
            child->PrintTree(indent + 2);
        }
    }

    
};


//planning agg 
class GroupByPlan : public AbstractPlanNode{

private:
    //group by needs a vector of keys to group on and vector of grouping functions 
    vector<BoundExpression*> grouping_keys;
    vector<BoundExpression*> grouping_functions;
    BoundExpression* having;

    AbstractPlanNode* child;

public:

    GroupByPlan(vector<BoundExpression*> grouping_keys,vector<BoundExpression*> grouping_functions,
                AbstractPlanNode* child, BoundExpression*having) : grouping_keys(grouping_keys),

            grouping_functions(grouping_functions),child(child),having(having){
        this->type = PlanType::AGGREGATION;
    }

    vector<BoundExpression*> getGroupingKeys(){
        return grouping_keys;
    }
    
    vector<BoundExpression*> getGroupingFunctions(){
        return grouping_functions;
    }

    AbstractPlanNode* getChild(){
        return child;
    }
    BoundExpression* getHaving(){
        return this->having;
    }
    //just printing
    void PrintTree(int indent = 0) const override {

        PrintIndent(indent);

        cout << "GROUP BY" << endl;

        // Print grouping keys
        if (!grouping_keys.empty()) {

            PrintIndent(indent + 1);
            cout << "Grouping Keys:" << endl;

            for (auto* key : grouping_keys) {

                if (key != nullptr) {
                    key->PrintTree(
                        "|   ",
                        true
                    );
                }
            }
        }

        // Print grouping functions
        if (!grouping_functions.empty()) {

            PrintIndent(indent + 1);
            cout << "Grouping Functions:" << endl;

            for (auto* func : grouping_functions) {

                if (func != nullptr) {
                    func->PrintTree(
                        "|   ",
                        true
                    );
                }
            }
        }

        // Print child
        PrintIndent(indent + 1);
        cout << "Child:" << endl;

        if (child != nullptr) {
            child->PrintTree(indent + 2);
        }
    }
};


#endif