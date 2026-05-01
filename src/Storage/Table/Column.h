#ifndef COL_H
#define COL_H

#include <iostream>
#include <string>
#include "../Page/Field.h"
#include <variant>

class Column{

    private:
        Field* field;
        std::string col_name;
        int col_max_size;

    public:

        Column(const Field* field, std::string col_name,int col_max_size);
        std::string getColName();
        Field* getField();

        void serializeCol(char* data);
        void deSerializeCol(char* data);

        Column& operator=(const Column& other);

        int getColSize();

        void printCol();

};

#endif