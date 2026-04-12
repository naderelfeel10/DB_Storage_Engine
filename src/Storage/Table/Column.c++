#include<iostream>
#include"Column.h"
using namespace std;


Column::Column(const Field* field, std::string col_name,int col_max_size = -1 ){
    this->field = new Field(*field);
    this->col_name = col_name;
    this->col_max_size = col_max_size;
}

string Column::getColName(){return this->col_name;}
Field* Column::getField(){return this->field;}


void Column::serializeCol(char* buffer){
    int offset = 0;
    
    memcpy(buffer, &this->col_max_size, sizeof(col_max_size));
    offset+=sizeof(col_max_size);

    int col_name_size = col_name.length();
    memcpy(buffer+offset, &col_name_size, sizeof(int));
    offset +=sizeof(int);


    memcpy(buffer+offset, col_name.c_str(), col_name_size);
    offset += col_name_size;

    
    this->field->serialize(buffer+offset);
    offset+=field->getSerializedSize();


}

void Column::deSerializeCol(char* buffer){
    int offset = 0;
    
    memcpy(&this->col_max_size, buffer+offset, sizeof(col_max_size));
    offset+=sizeof(col_max_size);

    int col_name_size;
    memcpy(&col_name_size,buffer+offset, sizeof(col_name_size));
    offset += sizeof(col_name_size);

    this->col_name.assign(buffer + offset, col_name_size);
    offset += col_name_size;

    this->field->deserialize(buffer+offset);
}

int Column::getColSize(){
    return (sizeof(this->col_max_size) +  sizeof(int) + this->col_name.length() + this->field->getSerializedSize() );
}


Column& Column::operator=(const Column& other) {
    if (this == &other) return *this; 

    delete this->field;

    this->col_name = other.col_name;
    this->col_max_size = other.col_max_size;

    this->field = new Field(*other.field);

    return *this;
}

void Column::printCol(){
    cout<<"Column name : "<<this->col_name<<endl;
    cout<<"Column max size : "<<this->col_max_size;
    this->field->print();
}

/*
int main(){
    
}
*/