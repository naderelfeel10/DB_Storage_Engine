#ifndef BPM_H
#define BPM_H

#include <iostream>
#include <cstring>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <utility>
#include "DiskManager.h"
#define BUFFER_SIZE 100
using namespace std;

/*
    A high-performance C++ DB storage manager implementing a Slotted-Page architecture, Disk manager
    and a Buffer Pool Manager (BPM) to bridge the gap between volatile
    memory and persistent disk storage.
*/
class BufferPoolManager{
    private:

        DiskManager* disk_manager;
        

        char* frames[BUFFER_SIZE];

        unordered_map<int, int>page_table;

        int pages_ids[BUFFER_SIZE];
        int pin_count[BUFFER_SIZE];
        bool is_dirty[BUFFER_SIZE];
        vector<int> free_frames_ids;

        size_t num_frames{BUFFER_SIZE};
       
    public:
        BufferPoolManager(DiskManager* dm);
        ~BufferPoolManager();
        char* fetchPage(int page_id);
        void deletePage(int page_id);
        void unpinPage(int page_id, bool is_dirty_flag);
        void markAsDirty(int page_id);




        
};

#endif 