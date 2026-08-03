#include<iostream>
#include<vector>
#include<algorithm>
#include"src/Storage/Table/TableIterator.h"
#include"src/Storage/Table/Column.h"
#include"src/Storage/Indexing/Index.h"
#include"src/Storage/Indexing/StaticHashIndexWrapper.h"
#include<chrono>
#include<cassert>
#include"src/Storage/Indexing/BPlusTreeIndexWrapper.h"
#include"src/Buffer/BufferPoolManager.h";
using namespace std;

DiskManager* dm = new DiskManager("MVP_DB");
BufferPoolManager* BPM = new BufferPoolManager(dm);

int
main(){
    //load catalog


}