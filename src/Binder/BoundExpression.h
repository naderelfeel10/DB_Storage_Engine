#ifndef BOUND_EXPRESSION_H
#define BOUND_EXPRESSION_H


#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Page\Field.h"
//#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Binder\Expression.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\src\Storage\Table\Column.h"
#include"D:\SWE\DB\CMU\MY_DB_ENGINE\Minimal_DB_ENGINE\parser\external\sql-parser\src\sql\Expr.h"

#include<iostream>
#include<vector>
using namespace std;


enum class BoundExpressionType {
    COLUMN_REF,
    CONSTANT,
    BINARY,
    UNARY,
    FUNCTION,
    STAR
};

class BoundExpression {
public:
    FieldType return_type;
    BoundExpressionType exp_type;
    //virtual ~BoundExpression() = default;
    //just printing
    virtual void PrintTree(const string& prefix = "",
                           bool isLast = true) const {
        cout << prefix
             << (isLast ? "|-- " : "|-- ")
             << "BoundExpression\n";
    }
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

    // just printing
    void PrintTree(const string& prefix,
                               bool isLast) const override{
    cout << prefix
         << (isLast ? "|-- " : "|-- ")
         << "BoundColumnRef\n";

    string childPrefix = prefix + (isLast ? "    " : "|   ");

    cout << childPrefix << "|-- table: "
         << table_name << '\n';

    cout << childPrefix << "|-- column: "
         << column_name << '\n';

    cout << childPrefix << "|-- table_oid: "
         << table_oid << '\n';

    cout << childPrefix << "|-- column_oid: "
         << column_oid << '\n';

    cout << childPrefix << "|-- type: "
         << return_type << '\n';
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
    /*
    void printTable(){
        cout<<"table oid : "<<table_oid;
    }*/
    // a better printing
    void printTable() const {
        std::cout << "Table: " << table_name;
        if (!alias.empty()) {
            std::cout << " AS " << alias;
        }
        std::cout << " (OID: " << table_oid << ")\n";
        
        std::cout << "Schema:\n";
        for(Column col:schema){
            col.printCol();
        }
    }
};


class BoundConstantExpression : public BoundExpression {

public:
    // union of possible values the const might have:
    // int, double, cool, string
    union Value{
        int64_t int_const;
        double float_const;
        bool bool_const;
        string str_const;

        Value(){};
        ~Value(){};
    }value;


    BoundConstantExpression(const int64_t int_value){
        return_type = TYPE_INT;
        value.int_const = int_value;
        exp_type = BoundExpressionType::CONSTANT;
    }

    BoundConstantExpression(const double float_value){
        return_type = TYPE_FLOAT;
        value.float_const = float_value;
        exp_type = BoundExpressionType::CONSTANT;
    }

    BoundConstantExpression(const bool bool_value){
        return_type = TYPE_BOOL;
        value.bool_const = bool_value;
        exp_type = BoundExpressionType::CONSTANT;
    }

    BoundConstantExpression(const string& str_value) {
        return_type = TYPE_STRING;
        new (&value.str_const)string(str_value);
        exp_type = BoundExpressionType::CONSTANT;
    }

    //just printing
    void PrintTree(const string& prefix="", bool isLast=true) const override;
};
/*
enum class BinaryOperator{
    EQ,
    NE, 
    GT, 
    GE, 
    LT, 
    LE 
};*/
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



static string OperatorToString(BoundOperatorType op) {
    switch (op) {
        case BoundOperatorType::EQ:  return "=";
        case BoundOperatorType::NE:  return "!=";
        case BoundOperatorType::GT:  return ">";
        case BoundOperatorType::GE:  return ">=";
        case BoundOperatorType::LT:  return "<";
        case BoundOperatorType::LE:  return "<=";
        case BoundOperatorType::AND: return "AND";
        case BoundOperatorType::OR:  return "OR";
        case BoundOperatorType::ADD: return "+";
        case BoundOperatorType::SUB: return "-";
        case BoundOperatorType::MUL: return "*";
        case BoundOperatorType::DIV: return "/";
    }

    return "?";
}

class BoundBinaryExpression : public BoundExpression{

public:
    // a simple representation of the predicate
    BoundExpression* left;
    BoundExpression* right;

    BoundOperatorType op;

    BoundBinaryExpression(BoundExpression* left, BoundOperatorType op,BoundExpression* right, FieldType return_type)
                        :left(left), right(right),op(op){
        exp_type = BoundExpressionType::BINARY;
        this->return_type = return_type;
    }

    void PrintTree(const string& prefix = "",
                   bool isLast = true) const override {

        cout << prefix
             << (isLast ? "|-- " : "|-- ")
             << "BoundBinaryExpression\n";


        string childPrefix =
            prefix + (isLast ? "    " : "|   ");

        cout << childPrefix
             << "|-- operator: "
             << OperatorToString(op) << '\n';

        cout << childPrefix
             << "|-- left: ";

        if (left)
            cout << "NOT NULL\n";
        else
            cout << "NULL\n";

        if (left) {
            left->PrintTree(childPrefix + "|   ", true);
        }

        cout << childPrefix
             << "`-- right: ";

        if (right)
            cout << "NOT NULL\n";
        else
            cout << "NULL\n";

        if (right) {
            right->PrintTree(childPrefix + "    ", true);
        }

        
    }
};

#endif