#include"BoundExpression.h"
#include<iostream>
using namespace std;


void BoundConstantExpression::PrintTree(const string& prefix ,bool isLast ) const {

        cout << prefix
             << (isLast ? "|-- " : "|-- ")
             << "BoundConstant\n";

        string childPrefix =
            prefix + (isLast ? "    " : "|   ");

        switch (return_type) {

            case FieldType::TYPE_INT:
                cout << childPrefix
                     << "|-- value: "
                     << value.int_const << '\n';
                break;

            case FieldType::TYPE_FLOAT:
                cout << childPrefix
                     << "|-- value: "
                     << value.float_const << '\n';
                break;

            case FieldType::TYPE_BOOL:
                cout << childPrefix
                     << "|-- value: "
                     << (value.bool_const ? "true" : "false") << '\n';
                break;

            case FieldType::TYPE_STRING:
                cout << childPrefix
                     << "|-- value: "
                     << value.str_const << '\n';
                break;
            
            case FieldType::TYPE_NULL:
                cout << "NULL";
                break;
        }
    }

/*int
main(){

}*/