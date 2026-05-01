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
    
    // save some meta data
    memcpy(buffer, &this->col_max_size, sizeof(col_max_size));
    offset+=sizeof(col_max_size);

    int col_name_size = col_name.length();
    memcpy(buffer+offset, &col_name_size, sizeof(int));
    offset +=sizeof(int);


    memcpy(buffer+offset, col_name.c_str(), col_name_size);
    offset += col_name_size;

    // then serialize the field itself
    this->field->serialize(buffer+offset);
    offset+=field->getSerializedSize();


}

void Column::deSerializeCol(char* buffer){
    int offset = 0;
    
    // get meta data
    memcpy(&this->col_max_size, buffer+offset, sizeof(col_max_size));
    offset+=sizeof(col_max_size);

    int col_name_size;
    memcpy(&col_name_size,buffer+offset, sizeof(col_name_size));
    offset += sizeof(col_name_size);

    this->col_name.assign(buffer + offset, col_name_size);
    offset += col_name_size;

    // get the actual field
    this->field->deserialize(buffer+offset);
}

int Column::getColSize(){
    return (sizeof(this->col_max_size) +  sizeof(int) + this->col_name.length() + this->field->getSerializedSize() );
}

// c1 = c2;
// operator overloading 
Column& Column::operator=(const Column& other) {
    if (this == &other) return *this; 

    delete this->field;

    // make a deep copy intp the new one
    this->col_name = other.col_name;
    this->col_max_size = other.col_max_size;

    this->field = new Field(*other.field);

    return *this;
}

// making printing more readable
void Column::printCol() {
    cout<<this->col_name <<endl;
    cout<<"------------" <<endl;
     
    auto val = this->field->getFieldValue();

    if (const int* i = get_if<int>(&val)) {
        cout << *i;
    } 
    else if(const double* f = get_if<double>(&val)) {
        cout << *f;
    } 
    else if(const bool* b = get_if<bool>(&val)) {
        cout << (*b ? "true" : "false");
    } 
    else if(const std::string* s = get_if<std::string>(&val)) {
        cout << *s;
    }

    cout << "\n------------" << endl;
}

/*
int main(){
    
}
*/