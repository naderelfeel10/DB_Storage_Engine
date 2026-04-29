#ifndef STATIC_H_INDEX_WRAPPER_H
#define STATIC_H_INDEX_WRAPPER_H


#include"../Page/Tuple.h"
#include"../Table/RID.h"
#include"../Table/Column.h"
#include"Index.h"
#include"static_hash_index.h"



class StaticHashIndexWrapper: public Index{
    private:
        hashIndex* staticHashIndex;
        string col_name;
        FieldType field_type;
    
    public:
        StaticHashIndexWrapper(hashIndex* index, string col_name, FieldType field_type):
            staticHashIndex(index),col_name(col_name),field_type(field_type){}

        void Insert(Field&field,string col_name, vector<Column> tuple_cols, RID rid)override;
        void Delete(Field&field)override;
        RID Search(Field&field)const override;
};

#endif