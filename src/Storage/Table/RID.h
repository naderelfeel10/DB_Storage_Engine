#ifndef RID_H
#define RID_H

#include<iostream>
#include<utility>
#include "../Page/page.h"

class RID{
    
    private:
        int page_id;
        int slot_num;
        std::pair<int, int> actual_pair;
    public:
        RID(int page_id, int slot_num);
        void setRID(int page_id, int slot_num);
        void updateActualPair(RID rid);
        RID getActualPair();
        int getPageId();
        int getSlotNum();


};
#endif