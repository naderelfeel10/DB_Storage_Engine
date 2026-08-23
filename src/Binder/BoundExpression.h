#ifndef BOUND_EXPRESSION_H
#define BOUND_EXPRESSION_H


#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Page\Field.h"
//#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\Expression.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Table\Column.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\parser\external\sql-parser\src\sql\Expr.h"

#include<iostream>
#include<vector>
using namespace std;


struct BoundExpression {
    FieldType return_type;
    ExpressionType exp_type;
    virtual ~BoundExpression() = default;
};

enum class ExpressionType {
    COLUMN_REF,
    CONSTANT,
    BINARY,
    UNARY,
    FUNCTION,
    STAR
};

// i want to represent the bounded col like this :
// ColRef("age") : 
    /*BoundColumnRef
        table_oid = 10
        column_oid = 2
        return_type = INT*/

class BoundColumnRef:  public BoundExpression{

public:
    int table_oid;
    int column_oid;

    string table_name;
    string column_name;

    BoundColumnRef();
    //this constuctor recieves input from the parser output, then creates the BoundedCol
    BoundColumnRef(int table_oid, int column_oid, const string& table_name, const string& column_name, FieldType return_type)
        :table_oid(table_oid), column_oid(column_oid), table_name(table_name), column_name(column_name){

        this->return_type = return_type;
    }
};

/*
BoundTable
    oid = 10
    table_name = users
    alias = u
*/
class BoundTable{
    public:
    int table_oid;

    string table_name;
    string alias;
    vector<Column> schema;
};


class BoundConstantExpression : public BoundExpression {

    // union of possible values the const might have:
    // int, double, cool, string
    union Value{
        int int_const;
        double float_const;
        bool bool_const;
        string str_const;

        Value();
        ~Value();
    }value;


    BoundConstantExpression(const int int_value){
        return_type = TYPE_INT;
        value.int_const = int_value;
        exp_type = ExpressionType::CONSTANT;
    }

    BoundConstantExpression(const double float_value){
        return_type = TYPE_FLOAT;
        value.float_const = float_value;
        exp_type = ExpressionType::CONSTANT;
    }

    BoundConstantExpression(const bool bool_value){
        return_type = TYPE_BOOL;
        value.bool_const = bool_value;
        exp_type = ExpressionType::CONSTANT;
    }

    BoundConstantExpression(const string& str_value) {
        return_type = TYPE_STRING;
        new (&value.str_const)string(str_value);
        exp_type = ExpressionType::CONSTANT;
    }

};

enum class BinaryOperator{
    EQ,
    NE, 
    GT, 
    GE, 
    LT, 
    LE 
};
enum class BoundOperatorType {
    EQ,
    NE,
    GT,
    GE,
    LT,
    LE,
    AND,
    OR,
    ADD,
    SUB,
    MUL,
    DIV
};


class BoundBinaryExpression : public BoundExpression{

public:
    // a simple representation of the predicate
    BoundExpression* left;
    BoundExpression* right;

    BoundOperatorType op;

    BoundBinaryExpression(BoundExpression* left, BoundOperatorType op,BoundExpression* right, FieldType return_type)
                        :left(left), right(right),op(op){
        exp_type = ExpressionType::BINARY;
        this->return_type = return_type;
    }

};

#endif