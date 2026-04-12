#ifndef DM_H
#define DM_H

#include <iostream>
#include <cstring>
#include <fstream>
#include <unordered_map>
#include <vector>
#include "../Page/page.h"
#define FILE_CAPACITY 100


using namespace std;

struct __attribute__((packed)) DirectoryEntry{
    int page_id;
    size_t offset;
};

struct __attribute__((packed)) DBHeader {
    int capacity = FILE_CAPACITY;
    int map_size{0};        
    int deleted_size{0};    
};


class DiskManager{

    private:
        DBHeader header;
        string file_name;
        unordered_map<int, size_t> pages_table;
        vector<size_t> deleted_slots;

        void resizeFile();        
        auto getFileSize(const string&file_name);
    
        
    public:
        fstream DB_file;
        DiskManager(const string&file_name);
        ~DiskManager();
        void saveMetaData();
        void loadMetaData();
        void writePage(int page_id, const char*data);
        void readPage(int page_id, char*data);
        void deletePage(int page_id);
        size_t allocatePage();
        size_t getSize();
        



};

#endif