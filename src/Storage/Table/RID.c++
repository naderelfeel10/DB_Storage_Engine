#include<iostream>
#include"RID.h"
using namespace std;


RID::RID(int page_id, int slot_num){
    this->page_id = page_id;
    this->slot_num = slot_num;
    this->actual_pair = pair(page_id,slot_num);
}

void RID::setRID(int page_id, int slot_num){
    this->page_id = page_id;
    this->slot_num = slot_num;
}

//actual rids are needed when updating a tuple with into a new place
void RID::updateActualPair(RID rid){

    this->actual_pair = pair(rid.getPageId(),rid.getSlotNum());
}

RID RID::getActualPair(){
    return RID(actual_pair.first, actual_pair.second);
}

int RID::getPageId(){return this->page_id;}
int RID::getSlotNum(){return this->slot_num;}




/*
int 
main(){
    RID rid1 = RID(1,1);
    cout<<rid1.getPageId()<<endl;
    cout<<rid1.getSlotNum()<<endl;

}
*/