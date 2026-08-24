#ifndef BOUND_STATEMENT_H
#define BOUND_STATEMENT_H

#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Catalog\Catalog.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\parser\external\sql-parser\src\SQLParser.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\parser\external\sql-parser\src\SQLParserResult.h"

using namespace std;

enum class BoundStatementType {
    SELECT,
    INSERT,
    UPDATE,
    DELETE,
    CREATE_TABLE,
    DROP_TABLE
};


class BoundStatement {
public:
    virtual ~BoundStatement() = default;

    virtual BoundStatementType
    type() const = 0;
    // just printing
    //virtual void PrintTree(const string& prefix = "",
                           //bool isLast = true) const = 0;
};

#endif