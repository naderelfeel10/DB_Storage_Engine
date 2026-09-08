#ifndef BINDER_H
#define BINDER_H

#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Catalog\Catalog.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\parser\external\sql-parser\src\SQLParser.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\parser\external\sql-parser\src\SQLParserResult.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\BoundStatement.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\parser\external\sql-parser\src\sql\SelectStatement.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\BoundExpression.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\BoundSelectStatement.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\BindContext.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\BoundSelectStatement.h"
#include "D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\parser\external\sql-parser\src\SQLParser.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\parser\external\sql-parser\src\sql\SQLStatement.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Q_Execution\SortAggregateExecuter.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\BoundInsertStatement.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\parser\external\sql-parser\src\sql\InsertStatement.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\BoundUpdateStatement.h"

using namespace std;


class Binder {
private:
    Catalog* catalog;
    BindContext* context;

public:
    Binder(Catalog* catalog, BindContext* context){
        this->catalog = catalog;
        this->context = context;
    }
    
    std::unique_ptr<BoundStatement> bind(const hsql::SQLStatement* statement);
    
    //bind selectr statement
    BoundSelectStatement* BindSelect(const hsql::SelectStatement* statement);
    BoundInsertStatement* BindInsert(const hsql::InsertStatement* statement);
    BoundUpdateStatement* BindUpdate(const hsql::UpdateStatement* statement);


    //sub functions used in main ones
    BoundExpression* BindExpression( hsql::Expr* expression);
    //expression types thaat i need to bind
    BoundExpression* BindColumnRef(const hsql::Expr* expression);

    BoundExpression* BindIntegerLiteral(hsql::Expr* expression);
    BoundExpression* BindFloatLiteral(hsql::Expr* expression);
    BoundExpression* BindStringLiteral(hsql::Expr* expression);
    BoundExpression* BindOperator(const hsql::Expr* expression);
    BoundExpression* BindFunction(hsql::Expr* expression);

    BoundOperatorType BindBinaryOperator(hsql::OperatorType op);
    //BoundExpression* BindOperator(const hsql::Expr* expression);
    //select statement bind sub-functions
    void BindFrom( const hsql::SelectStatement* statement, BoundSelectStatement& bound);

    BoundJoinClause BindJoin(const hsql::JoinDefinition* join);
    JoinType BindJoinType(hsql::JoinType type);

    BoundTable BindTable(const hsql::TableRef* table);

    void BindOrderBy(const hsql::SelectStatement* statement, BoundSelectStatement& bound);
    void BindLimitOffset(const hsql::SelectStatement* statement, BoundSelectStatement& bound);
};


#endif