#include"binder.h"
#include<iostream>
#include "assert.h"

unique_ptr<BoundStatement> Binder::bind(const hsql::SQLStatement* statement) {
    
    cout<<"stmt type : " <<statement->type()<<endl;
    switch (statement->type()) {
        case hsql::kStmtSelect:{

            auto* select_statement = static_cast<const hsql::SelectStatement*>(statement);

            return unique_ptr<BoundStatement>(BindSelect(select_statement));
        }
        /*
        case hsql::kStmtInsert:
            return InsertBinder(catalog)
                .bind(
                    static_cast<
                        const hsql::InsertStatement*
                    >(statement));

        case hsql::kStmtUpdate:
            return UpdateBinder(catalog)
                .bind(
                    static_cast<
                        const hsql::UpdateStatement*
                    >(statement));

        case hsql::kStmtDelete:
            return DeleteBinder(catalog)
                .bind(
                    static_cast<
                        const hsql::DeleteStatement*
                    >(statement));

        default:
            throw BinderException(
                "Unsupported statement");

        */

        default: throw runtime_error("Unsupported statement type");
    }
}



/*
    in a select statement we expect alot of diff options :

    1. select items like [select name, age, salary]
    2."from table", where we use to fetch tuples
    3.might be join operations
    4. where clause and it's predicate
    5. might have aggregations and group by
    6.might have order by
    7. limit and offset
    // still more options to do later 

*/

BoundSelectStatement* Binder::BindSelect(const hsql::SelectStatement* statement){

    //check if null statement
    if(statement ==nullptr)
        throw runtime_error("cannot bind null SELECT statement");
    

    // main components of this statement :
    // boundSelectStatement is the output I need to return 
    auto* bound = new BoundSelectStatement();

    //FROM
    BindFrom(statement, *bound);

    //JOIN
    if(statement->fromTable->join != nullptr){
        BoundJoinClause bind_join = BindJoin(statement->fromTable->join);
        bound->joins.push_back(bind_join);
    }


    //select list :

    //loop through each expr in select list, resolve it's expression, then add it to bound list
    for(auto&expr : *statement->selectList){
        BoundSelectItem item;
        //resolve the expression binder
        item.expression = BindExpression(expr);
        
        item.alias = expr->alias?expr->alias :"";
        bound->select_list.push_back(item);
    }


    //WHERE, where is just an expression 
    if(statement->whereClause != nullptr)
        bound->where = BindExpression(statement->whereClause);

    //GROUP-BY
    if(statement->groupBy!= nullptr){
        for(auto& expression : *statement->groupBy->columns){
            bound->group_by.push_back(BindExpression(expression));
        }
    }
    //HAVING
    if(statement->groupBy && statement->groupBy->having != nullptr){
        bound->having = BindExpression(statement->groupBy->having);
    }

    // ORDER BY
    //BindOrderBy(statement);

    // LIMIT adn OFSSET
    //BindLimitOffset(statement);

    bound->PrintTree();
    return bound;
}

BoundExpression* Binder::BindExpression(hsql::Expr* expression){

    if (expression == nullptr) {
        return nullptr;
    }
    switch (expression->type) {
        cout<<"expr type : "<<expression->type<<endl;
        case hsql::kExprColumnRef:
            return BindColumnRef(expression);

        case hsql::kExprLiteralInt:
            return BindIntegerLiteral(expression);

        case hsql::kExprLiteralFloat:
            return BindFloatLiteral(expression);

        case hsql::kExprLiteralString:
            return BindStringLiteral(expression);

        case hsql::kExprOperator:
            return BindOperator(expression);

        case hsql::kExprFunctionRef:
            return BindFunction(expression);

        default:

            throw std::runtime_error(
                "Unsupported expression"
            );
    }
}


BoundExpression* Binder::BindColumnRef(const hsql::Expr* expression){
    //chech if null expression
    if(expression == nullptr)
        throw runtime_error("cannot bind null column expression");
    
    //check if type is col_ref
    if(!expression->isType(hsql::kExprColumnRef)){
        throw runtime_error("expression is not a column reference");
    }

    //extract column name from parser AST
    string column_name = expression->getName();
    cout<< "col name : "<< column_name<<endl;

    if (column_name.empty()) {
        throw runtime_error("column name is empty");
    }
    //get table_name
    string table_name = expression->table? expression->table : "NULL";
    cout<< "table name : "<< table_name<<endl;

    //resolve col_ref from the context of the binder
    BoundColumnRef* col_ref;
    col_ref = context->ResolveColumn(table_name, column_name);

    return col_ref;

}

// a helper function to convert from ast operator to my defined operators
BoundOperatorType Binder::BindBinaryOperator(hsql::OperatorType op){

    switch (op) {
        case hsql::kOpEquals:
            return BoundOperatorType::EQ;

        case hsql::kOpNotEquals:
            return BoundOperatorType::NE;

        case hsql::kOpGreater:
            return BoundOperatorType::GT;

        case hsql::kOpGreaterEq:
            return BoundOperatorType::GE;

        case hsql::kOpLess:
            return BoundOperatorType::LT;

        case hsql::kOpLessEq:
            return BoundOperatorType::LE;

        case hsql::kOpAnd:
            return BoundOperatorType::AND;

        case hsql::kOpOr:
            return BoundOperatorType::OR;

        case hsql::kOpPlus:
            return BoundOperatorType::ADD;

        case hsql::kOpMinus:
            return BoundOperatorType::SUB;

        default:
            throw runtime_error("undefined binary operator");
    }
}

//bind operator for where clause:
// to handle expressions like (where id=10 and age>30 or salary>10000)
BoundExpression* Binder::BindOperator(const hsql::Expr* expression){

    // check if null first
    if (expression == nullptr) {
        throw std::runtime_error(
            "cannot bind null operator"
        );
    }

    // operator has left, right and the operator
    BoundExpression* left = BindExpression(expression->expr);
    BoundExpression* right = BindExpression(expression->expr2);
    BoundOperatorType op = BindBinaryOperator(expression->opType);

    //return type might be :
    //1. bool : if it's like (a and b) for where clauses
    //2.might have a value like int or string
    FieldType return_type;
    switch (op) {

        //these types return bool result
        case BoundOperatorType::AND:
        case BoundOperatorType::OR:
        case BoundOperatorType::EQ:
        case BoundOperatorType::NE:
        case BoundOperatorType::GT:
        case BoundOperatorType::GE:
        case BoundOperatorType::LT:

        case BoundOperatorType::LE:{ return_type = TYPE_BOOL;
            break;
        }
        // these types return a value
        // example in order by clauses : order by (age+10)
        case BoundOperatorType::ADD:
        case BoundOperatorType::SUB:
        case BoundOperatorType::MUL:
        case BoundOperatorType::DIV:{
            return_type = left->return_type;
            break;
        }

        default:
            throw runtime_error(" unsupported operator");
    }

    return new BoundBinaryExpression(left, op, right, return_type);
}



void Binder::BindFrom( const hsql::SelectStatement* statement, BoundSelectStatement& bound){

    //check if it has from table first
    if(statement->fromTable == nullptr) {
        throw runtime_error("select statement has no FROM table");
    }
    //fetch the table to extract info
    const hsql::TableRef* table=statement->fromTable;

    //check it's a table first
    if (table->type != hsql::kTableName) {
        throw std::runtime_error("unsupported \"from\" table type");
    }
    //extract table name
    // then fetch from catalog
    string table_name = table->name;

    string alias;
    //if it has an alian name=
    if (table->alias != nullptr) {
        alias = table->alias->name;
    }

    // now extract the table from the catalog, 
    TableInfo* table_info = catalog->GetTable(table_name);

    // check if table name exists in the catalog
    if(!table_info){
        throw runtime_error(" table does not exist: " +table_name);
    }
    // now bound the table 
    BoundTable bound_table;

    bound_table.table_oid = table_info->table_id;
    bound_table.table_name = table_name;
    bound_table.alias = alias;
    bound_table.schema = table_info->schema;

    bound.from_table = bound_table;

    // last thing to add the table to the context
    context->AddTable(bound_table);
}



// join :
//type conversion from AST type and my defined type
JoinType Binder::BindJoinType(hsql::JoinType type){

    switch (type){
        case hsql::kJoinInner:
            return JoinType::INNER;

        case hsql::kJoinLeft:
            return JoinType::LEFT;

        case hsql::kJoinRight:
            return JoinType::RIGHT;

        default:
            throw runtime_error("unsupported join type");
    }
}



// binding join clause
BoundJoinClause Binder::BindJoin(const hsql::JoinDefinition* join){

    //join has left, right, condition, and join type
    // first get right table

    string alias = join->right->alias->name;
    string table_name = join->right->name;
    // now extract the table from the catalog, 
    TableInfo* table_info = catalog->GetTable(table_name);

    // check if table name exists in the catalog
    if(!table_info){
        throw runtime_error(" table does not exist: " +table_name);
    }
    // now bound the table 
    BoundTable bound_table;

    bound_table.table_oid = table_info->table_id;
    bound_table.table_name = table_name;
    bound_table.alias = alias;
    bound_table.schema = table_info->schema;


    // last thing to add the table to the context
    context->AddTable(bound_table);


    BoundExpression* condition = nullptr;
    // extract on operator
    if(join->condition != nullptr){
        //extract the condition
        condition = BindExpression(join->condition);
        //it has to be bool
        if(condition->return_type != TYPE_BOOL){
            throw runtime_error("JOIN condition must evaluate to BOOLEAN");
        }
    }

    JoinType type =BindJoinType(join->type);

    BoundJoinClause result(type, bound_table, condition);

    return result;
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

    catalog->CreateTable(table_name, user_schema);

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

    const std::string sql = "SELECT User.user_id FROM User;";

    hsql::SQLParserResult result;

    hsql::SQLParser::parse(sql, &result);

    BindContext* context = new BindContext();
    Binder* binder = new Binder(catalog, context);

    const hsql::SQLStatement* stmt = result.getStatement(0);
    binder->bind(stmt);

    cout<<context->tables.size()<<endl;

    for(auto&table:context->tables){
        table.printTable();
    }

    


}