#include"ExecutorFactory.hpp"
#include<iostream>
using namespace std;

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
            AbstractPredicate* predicate = build_predicate(filter_plan->predicate, child);

            return new Select(child, predicate);
        }
        default:{
            throw runtime_error("invalid planType");
            return nullptr;
        }

    }
}


AbstractPredicate* ExecutorFactory::build_predicate(BoundExpression* expression, AbstractExecuter* child){
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
            AbstractPredicate* left_child = build_predicate(binary->left,child );
            AbstractPredicate* right_child = build_predicate(binary->right,child );

            return new ComplexPredicate(left_child, right_child, ComplexPredicateType::AND);
        }

        case BoundOperatorType::OR:{
            AbstractPredicate* left_child = build_predicate(binary->left, child);
            AbstractPredicate* right_child = build_predicate(binary->right, child);

            return new ComplexPredicate(left_child, right_child, ComplexPredicateType::OR);
        }

        case BoundOperatorType::NOT:{
            AbstractPredicate* left_child = build_predicate(binary->left, child);

            return new ComplexPredicate(left_child, nullptr, ComplexPredicateType::NOT);
        }

        //now simple predicates like user_id = 1;

        case BoundOperatorType::EQ:{
            //string col_name = dynamic_cast<BoundColumnRef*>(binary->left)->column_name;
            Column* left_col = expr_to_col(binary->left, child);
            Column* right_col = expr_to_col(binary->right, child);

            return new Predicate(left_col, right_col, PredicateType::EQ);
        }
        case BoundOperatorType::NE:{
            //string col_name = dynamic_cast<BoundColumnRef*>(binary->left)->column_name;
            Column* left_col = expr_to_col(binary->left, child);
            Column* right_col = expr_to_col(binary->right, child);

            return new Predicate(left_col, right_col, PredicateType::NE);
        }

        case BoundOperatorType::GT:{
            //string col_name = dynamic_cast<BoundColumnRef*>(binary->left)->column_name;
            Column* left_col = expr_to_col(binary->left, child);
            Column* right_col = expr_to_col(binary->right, child);

            return new Predicate(left_col, right_col, PredicateType::GT);
        }

        case BoundOperatorType::GE:{
            //string col_name = dynamic_cast<BoundColumnRef*>(binary->left)->column_name;
            
            Column* left_col = expr_to_col(binary->left, child);
            Column* right_col = expr_to_col(binary->right, child);

            return new Predicate(left_col, right_col, PredicateType::GE);
        }

        case BoundOperatorType::LT:{
            //string col_name = dynamic_cast<BoundColumnRef*>(binary->left)->column_name;
            Column* left_col = expr_to_col(binary->left, child);
            Column* right_col = expr_to_col(binary->right, child);

            return new Predicate(left_col, right_col, PredicateType::LT);
        }

        case BoundOperatorType::LE:{
            //string col_name = dynamic_cast<BoundColumnRef*>(binary->left)->column_name;
            Column* left_col = expr_to_col(binary->left, child);
            Column* right_col = expr_to_col(binary->right, child);

            return new Predicate(left_col, right_col, PredicateType::LE);
        }

        default:
            throw runtime_error(
                "Unsupported predicate operator"
            );
    }
}




// expression can't just be passed to select operator
// i have to transform it from expression into col, then pass it into the operator
Column* ExecutorFactory::expr_to_col(BoundExpression* expr,AbstractExecuter* child){

    //based on expression type, choose how to handle it
    // might be col_ref, or const
    cout<<expr->exp_type<<endl;
    switch(expr->exp_type){
        //fist case if col_ref, ex: "user_id"
        case BoundExpressionType::COLUMN_REF:{

            BoundColumnRef* col_ref = static_cast<BoundColumnRef*>(expr);

             //fetch schema from the child
            vector<Column> schema = child->get_output_schema();
            //then select the needed col
            Column* result = new Column(schema[col_ref->column_oid]);

            return result;
        }

        // case 2 is const values, like : integer or string value
        case BoundExpressionType::CONSTANT:{

            BoundConstantExpression* constant = static_cast<BoundConstantExpression*>(expr);
            //convert this concrete value into actual col
            Column* result = const_to_col(constant);

            return result;
        }

        default:
            throw runtime_error("invalid expression");
    }
}


//wrap const values into a col
Column* ExecutorFactory::const_to_col(BoundConstantExpression* expr){
        
    //convert based on return type
    //int, bool, float or string
    switch(expr->return_type){
        
        case FieldType::TYPE_INT:{
            //construct the field containing typen and value
            Field* f = new Field(TYPE_INT, static_cast<int>(expr->value.int_const));
            //then col has field, name and size
            Column* col = new Column(f,"_const_int_", sizeof(int));
            return col;
        }

        case FieldType::TYPE_FLOAT:{
            //construct the field containing typen and value
            Field* f = new Field(TYPE_FLOAT, static_cast<double>(expr->value.float_const));
            //then col has field, name and size
            Column* col = new Column(f,"_const_float_", sizeof(double));
            return col;
        }

        case FieldType::TYPE_BOOL:{
            //construct the field containing typen and value
            Field* f = new Field(TYPE_BOOL, static_cast<bool>(expr->value.bool_const));
            //then col has field, name and size
            Column* col = new Column(f,"_const_bool_", sizeof(bool));
            return col;
        }

        case FieldType::TYPE_STRING:{
            //construct the field containing typen and value
            Field* f = new Field(TYPE_STRING, static_cast<const char*>(expr->value.str_const.c_str()));
            //then col has field, name and size
            Column* col = new Column(f,"_const_str_", expr->value.str_const.size());
            return col;
        } 

        default:
            break;
        }
}


int
main(){

    DiskManager* dm = new DiskManager("catalog.db");
    BufferPoolManager* BPM = new BufferPoolManager(dm);

    Catalog* catalog = new Catalog(BPM, true);

    string table_name = "User";

    Column t1_col1 = Column(TYPE_INT, "user_id", sizeof(int));
    Column t1_col2 = Column(TYPE_STRING, "firstName", 30);
    Column t1_col3 = Column(TYPE_STRING, "lastName", 30);
    Column t1_col4 = Column(TYPE_INT, "age", sizeof(int));
    vector<Column> user_schema = {t1_col1, t1_col2, t1_col3, t1_col4};

    Column t3_col1 = Column(TYPE_INT, "order_id", sizeof(int));
    Column t3_col2 = Column(TYPE_INT, "user_id", sizeof(int));
    Column t3_col3 = Column(TYPE_FLOAT, "totalAmount", sizeof(float));
    Column t3_col4 = Column(TYPE_BOOL, "isShipped", sizeof(bool));
    vector<Column> order_schema = {t3_col1, t3_col2, t3_col3, t3_col4};


    catalog->CreateTable(table_name, user_schema);
    catalog->CreateTable("Orders", order_schema);

    cout<<endl;
    for(auto&[table_name, table_info]: catalog->getTables()){
        cout<<table_name<<endl;
        cout<<table_info->table_name<<endl;
        for(auto&col: table_info->schema){
            col.printCol();
        }
        cout<<endl;
    }
    cout<<"--------------"<<endl;

    //catalog->save_catalog();

    //const std::string sql = "SELECT u.user_id, u.firstName from User as u "
    //                        "inner join Orders on u.user_id = Orders.user_id "
    //                        "WHERE u.user_id = 2 order by u.user_id limit 10 offset 5;";
                            

    const string sql = "select u.firstName from User u where u.user_id > 10;";

    hsql::SQLParserResult result;

    hsql::SQLParser::parse(sql, &result);

    BindContext* context = new BindContext();
    Binder* binder = new Binder(catalog, context);

    const hsql::SQLStatement* stmt = result.getStatement(0);
    unique_ptr<BoundStatement> bound_stmt =  binder->bind(stmt);

    cout<<context->tables.size()<<endl;

    for(auto&table:context->tables){
        table.printTable();
    }

    cout<<endl<<"================================================================================================="<<endl;
    bound_stmt->PrintTree();
    cout<<endl<<"================================================================================================="<<endl;
    
    //create the plan
    Planner* planner = new Planner();
    AbstractPlanNode* plan = planner->Plan(move(bound_stmt));
    
    //print plan tree
    plan->PrintTree();

    //executer factory
    ExecutorFactory factory(catalog, context);
    AbstractExecuter* executor = factory.createExecutor(plan);

    //execution
    executor->open();

    cout<<"\n==========output==========\n";

    Tuple tuple({});

    while(executor->getNext(&tuple)){
        tuple.print();
    }

    executor->close();

    cout<<"=============================\n";
    


}