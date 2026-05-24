#ifndef COL_H
#define COL_H

#include <iostream>
#include <string>
#include "../Page/Field.h"
#include <variant>

class Column{

    private:
        Field* field;
        FieldType field_type;
        std::string col_name;
        int col_max_size;

    public:

        Column(const Field* field, string col_name,int col_max_size);
        Column(FieldType f_type, string col_name,int col_max_size):field(nullptr),field_type(f_type),col_name(col_name),col_max_size(col_max_size){};
        Column(){}
        std::string getColName();
        Field* getField();

        void serializeCol(char* data);
        void deSerializeCol(char* data);

        Column& operator=(const Column& other);

        int getColSize();
        FieldType getColType(){return this->field_type;}
        void printCol();

};

#endif