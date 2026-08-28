#include"ExecutorFactory.hpp"

//create the factory executer based on plan 
AbstractExecuter* ExecutorFactory::createExecutor(AbstractPlanNode* plan){
    
    switch(plan->type){

        //fist case if seq scan
        // i will prepare the table_heap then feed it as a prameter to it's operator
        case PlanType::SEQ_SCAN:{

            auto* scan_plan = static_cast<SeqScanPlan*>(plan);

            //resolve table name from context, then get table heap from catalog
            BoundTable bound_table = context->getBoundTable(scan_plan->table_oid);
            string table_name = bound_table.table_name;

            // get table_info, then get the table heap
            TableInfo* table_info = catalog->GetTable(table_name);
            TableHeap* table_heap = table_info->get_table_heap();

            //check if null
            if(table_heap == nullptr){
                throw runtime_error("table heap not found");
            }
            
            // then return the Executer
            return new SeqScan(table_heap);
        }


        //now the projection
        case PlanType::PROJECTION:{

            auto* projection_plan = static_cast<ProjectionPlan*>(plan);
        
            //the child is also a child executer, so i will keep calling it recursevly 
            AbstractExecuter* child = createExecutor(projection_plan->child);
            
            //now construct projection cols to select
            vector<string> projection_cols;
            
            //loop through all select items in the projection plan
            //projection only needs vector of col names, and the child executer
            for(const auto& item : projection_plan->expressions){
                
                //first fetch the col_ref
                BoundColumnRef* column_ref = dynamic_cast<BoundColumnRef*>(item.expression);
                
                //check if null first
                if(column_ref == nullptr){
                    throw runtime_error("col ref is nullptr");
                }
                //push col_name into the col_ref
                projection_cols.push_back(column_ref->column_name);
            }
        
            //now return the operator
            return new Projection(child, projection_cols);
        }

        //select case
        case PlanType::FILTER:{

            //select needs child executer and a predicate to filer on
            auto* filter_plan = static_cast<FilterPlan*>(plan);

            //keep calling recursevly
            AbstractExecuter* child = createExecutor(filter_plan->child);
            //then constrcut the predicate from bound 
            AbstractPredicate* predicate =
                ConvertPredicate(
                    filter_plan->predicate
                );

            return new Select(
                child,
                predicate
            );
        }

    }
}


AbstractPredicate* ExecutorFactory::BuildPredicate(BoundExpression* expression){
    //check if null
    if(expression == nullptr){
        return nullptr;
    }

    // this has to be binary expression
    if(expression->exp_type != BoundExpressionType::BINARY){
        throw runtime_error("predicate has to be binary");
    }

    auto* binary=static_cast<BoundBinaryExpression*>(expression);
    //build the predicate based on it's type recursively
    switch (binary->op){
        //complex predicate 
        //and, or, not
        case BoundOperatorType::AND:{
            AbstractPredicate* left_child = BuildPredicate(binary->left);
            AbstractPredicate* right_child = BuildPredicate(binary->right);

            return new ComplexPredicate(left_child, right_child, ComplexPredicateType::AND);
        }

        case BoundOperatorType::OR:{
            AbstractPredicate* left_child = BuildPredicate(binary->left);
            AbstractPredicate* right_child = BuildPredicate(binary->right);

            return new ComplexPredicate(left_child, right_child, ComplexPredicateType::OR);
        }

        case BoundOperatorType::NOT:{
            AbstractPredicate* left_child = BuildPredicate(binary->left);

            return new ComplexPredicate(left_child, nullptr, ComplexPredicateType::NOT);
        }

        //now simple predicates like user_id = 1;

        case BoundOperatorType::EQ:{
            string col_name = dynamic_cast<BoundColumnRef*>(binary->left)->column_name;

            Column* left_col = Column(); 
            return Predicate(binary, PredicateType::EQ);

        }
        case BoundOperatorType::NE:
            return BuildSimplePredicate(
                binary,
                PredicateType::NE
            );

        case BoundOperatorType::GT:
            return BuildSimplePredicate(
                binary,
                PredicateType::GT
            );

        case BoundOperatorType::GE:
            return BuildSimplePredicate(
                binary,
                PredicateType::GE
            );

        case BoundOperatorType::LT:
            return BuildSimplePredicate(
                binary,
                PredicateType::LT
            );

        case BoundOperatorType::LE:
            return BuildSimplePredicate(
                binary,
                PredicateType::LE
            );

        default:
            throw runtime_error(
                "Unsupported predicate operator"
            );
    }
}