#ifndef PAGE_H
#define PAGE_H


#include<iostream>
#include<iostream>
#include<vector>
#include <cstdint>
#include"Tuple.h"
#define PAGE_SIZE 4096

struct __attribute__((packed)) PageHeader {
    uint32_t page_id;
    int32_t  next_page_id;
    uint16_t num_tuples = 0;
    uint16_t num_deleted_tuples = 0;
    uint16_t free_space_pointer = PAGE_SIZE;
};


struct Slot{
    uint16_t offset;
    uint16_t size;
    bool id_deleted{false};
};

class Page{
    private:
        char data[PAGE_SIZE];     
    public:
        Page(int page_id);
        int insertTuple(const Tuple tuple);
        bool getTuple(int slot_num,Tuple& tuple);
        bool deleteTuple(int slot_num);
        int updateTuple(int slot_num, Tuple new_tuple);
        //void serializePage(char*buffer);
        //void deserializePage(char*buffer);
        char* getData();

};

#endif 
