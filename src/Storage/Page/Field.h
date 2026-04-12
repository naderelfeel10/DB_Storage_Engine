#ifndef FIELD_H
#define FIELD_H

#include <iostream>
#include <variant>
#include <cstring>

enum FieldType { TYPE_INT, TYPE_STRING, TYPE_FLOAT, TYPE_BOOL };

class Field {
private:
    FieldType fieldType;
    int size;
    bool is_null{false};
    
    union {
        int VALUE_INT;
        const char* VALUE_STRING;
        double VALUE_FLOAT;
        bool VALUE_BOOL;
    };

public:
    Field(FieldType type, int value);
    Field(FieldType type, double value);
    Field(FieldType type, bool value);
    Field(FieldType type, const char* value);
    Field(const Field& other);
    Field(FieldType type);
    ~Field();

    void setValue(int value);
    void setValue(double value);
    void setValue(bool value);
    void setValue(const char* value);


    int getFieldValueInt();
    float getFieldValueFloat();
    bool getFieldValueBool();
    const char* getFieldValueStr();


    FieldType getFieldType() ;
    bool isNull() ;
    int getSize() ;
    int getSerializedSize() const ;

    void print() ;
    void serialize(char* buffer);
    void deserialize(char* buffer);
    Field& operator=(const Field& other);

};

#endif 