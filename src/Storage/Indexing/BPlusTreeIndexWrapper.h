#ifndef BPlus_INDEX_WRAPPER_H
#define BPlus_INDEX_WRAPPER_H


#include"../Page/Tuple.h"
#include"../Table/RID.h"
#include"../Table/Column.h"
#include"Index.h"
#include"BPlusTreeIndex.h"



class BPlusTreeIndexWrapper: public Index{
    private:
        BPlusTree* BPlusTreeIndex;
        string col_name;
        FieldType field_type;
    
    public:
        BPlusTreeIndexWrapper(BPlusTree* index, string col_name, FieldType field_type):
            BPlusTreeIndex(index),col_name(col_name),field_type(field_type){}

        void Insert(Field&field,string col_name, vector<Column> tuple_cols, RID rid)override;
        void Delete(Field&field)override;
        RID Search(Field&field)const override;
        vector<RID> searchRange(Field lower, Field upper);


};

#endif