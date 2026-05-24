#ifndef BPlus_INDEX_WRAPPER_H
#define BPlus_INDEX_WRAPPER_H


#include"../Page/Tuple.h"
#include"../Table/RID.h"
#include"../Table/Column.h"
#include"Index.h"
#include"BPlusTreeIndex.h"



class BPlusTreeIndexWrapper:public Index{
    private:
        string col_name;
        FieldType field_type;
    
    public:
        BPlusTree* BPlusTreeIndex;
        BPlusTreeIndexWrapper(BPlusTree* index, string col_name, FieldType field_type):
            BPlusTreeIndex(index),col_name(col_name),field_type(field_type){}

        void Insert(Field&field,string col_name, vector<Column> tuple_cols, RID rid)override;
        void Delete(Field&field)override;

        vector<RID> Search(Field&field)const override;
        //RID Search(Field&field)const;

        vector<RID> searchRange(Field lower, Field upper);
        void displayIndexPages(){
            this->BPlusTreeIndex->printTree();
        };

    ~BPlusTreeIndexWrapper() {
        this->BPlusTreeIndex->saveBPlusTree();
        cout<<"saving B+ tree"<<endl;
        if (this->BPlusTreeIndex->root != nullptr) {
            BPlusTreeIndex->clear(this->BPlusTreeIndex->root);
            this->BPlusTreeIndex->root = nullptr;
    }
    
}

};

#endif