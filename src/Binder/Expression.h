#ifndef EXPR_H
#define EXPR_H


#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Page\Field.h"
#include<iostream>
using namespace std;

enum class ExpressionType {
    COLUMN_REF,
    CONSTANT,
    BINARY,
    UNARY,
    FUNCTION,
    STAR
};


struct Expression {
    ExpressionType type;
    virtual ~Expression() = default;
};

struct ColumnRefExpression : Expression {
    //col representation as table_name and col_name
    string table_name;     
    string column_name;

    //2 constructors
    ColumnRefExpression(const string& column):column_name(column){
        type = ExpressionType::COLUMN_REF;
    }

    ColumnRefExpression(const string& table, const string& column): table_name(table), column_name(column){
        type = ExpressionType::COLUMN_REF;
    }
};


struct ConstantExpression : Expression {

    FieldType val_type;
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

    ConstantExpression(const int int_value){
        val_type = TYPE_INT;
        value.int_const = int_value;
        type = ExpressionType::CONSTANT;
    }

    ConstantExpression(const double float_value){
        val_type = TYPE_FLOAT;
        value.float_const = float_value;
        type = ExpressionType::CONSTANT;
    }

    ConstantExpression(const bool bool_value){
        val_type = TYPE_BOOL;
        value.bool_const = bool_value;
        type = ExpressionType::CONSTANT;
    }

    ConstantExpression(const string& str_value) {
        val_type = TYPE_STRING;
        new (&value.str_const)string(str_value);
        type = ExpressionType::CONSTANT;
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

struct BinaryExpression : Expression{

    // a simple representation of the predicate
    Expression* left;
    Expression* right;

    BinaryOperator op;

    BinaryExpression(Expression* left, BinaryOperator op, Expression* right):left(left), right(right), op(op){
        type = ExpressionType::BINARY;
    }

};
#endif