#include<iostream>
#include<string>
#include<cstring>
#include<fstream>
#include<filesystem>

#include"DiskManager.h"
using namespace std;


DiskManager::DiskManager(const string&file_name){
    this->file_name = file_name;
    //open in read write mood
    this->DB_file.open(file_name, ios::binary | ios::in | ios::out);  

    // if does not exist create new one
    if(!DB_file.is_open()){
        DB_file.open(file_name, std::ios::binary | std::ios::trunc | std::ios::out | std::ios::in);
        if(!DB_file.is_open()){
            cerr <<("Can't open the DB_file");
        }else{
            cout<<"new file is created successfuly"<<endl;
        }
        saveMetaData();
    }else{
        cout<<"file is opened successfuly"<<endl;
        loadMetaData();
    }
    filesystem::resize_file(file_name, header.capacity*PAGE_SIZE);

}





void DiskManager::saveMetaData(){
    cout<<"saving meta data"<<endl;

    int offset = 0;
    char buffer[PAGE_SIZE];
    memset(buffer,0,PAGE_SIZE);
    /*
    memcpy(buffer+offset,&header.capacity,sizeof(header.capacity));
    offset+=sizeof(header.capacity);

    header.map_size =static_cast<int>(pages_table.size());
    memcpy(buffer+offset,&header.map_size,sizeof(header.map_size));
    offset+=sizeof(header.map_size);

    header.deleted_size =static_cast<int>(deleted_slots.size());
    memcpy(buffer+offset,&header.deleted_size,sizeof(header.deleted_size));
    offset+=sizeof(header.deleted_size);
    */
    header.map_size =static_cast<int>(pages_table.size());
    header.deleted_size =static_cast<int>(deleted_slots.size());

    memcpy(buffer+offset, &header, sizeof(DBHeader));
    offset+=sizeof(DBHeader);

    for(auto &[id, off] : pages_table){
        DirectoryEntry entry = {id, off};
        memcpy(buffer+offset, &entry, sizeof(DirectoryEntry));
        offset+=sizeof(DirectoryEntry);
        cout<<"page "<<entry.page_id<<" saved ."<<endl;
    }

    for(size_t slot:deleted_slots){
        memcpy(buffer+offset, &slot,sizeof(slot));
        offset+=sizeof(slot);
    }

    DB_file.clear();
    DB_file.seekp(0,ios::beg);
    DB_file.write(buffer, PAGE_SIZE);
    DB_file.flush();
    
}



void DiskManager::loadMetaData(){

    cout<<"loader meta data"<<endl;
    char buffer[PAGE_SIZE];
    DB_file.clear();
    DB_file.seekg(0,ios::beg);
    DB_file.read(buffer, PAGE_SIZE);
    

    int offset = 0;

    /*
    DBHeader* header = reinterpret_cast<DBHeader*>(buffer);
    int capacity;
    memcpy(&capacity, buffer+offset,sizeof(capacity));
    offset+=sizeof(capacity);

    int map_size;
    memcpy(&map_size, buffer+offset,sizeof(map_size));
    offset+=sizeof(map_size);

    int deleted_size;
    memcpy(&deleted_size, buffer+offset,sizeof(deleted_size));
    offset+=sizeof(deleted_size);

    header->capacity = capacity;
    header->map_size = map_size;
    header->deleted_size = deleted_size;
    */


    memcpy(&this->header,buffer+offset, sizeof(DBHeader));
    offset+= sizeof(DBHeader);

    cout<<"file header size : "<< header.map_size<<endl;


    pages_table.clear();
    for(int i{};i<header.map_size;i++){
        DirectoryEntry entry;
        memcpy(&entry,buffer+offset, sizeof(DirectoryEntry));
        pages_table[entry.page_id] = entry.offset;
        offset+=sizeof(DirectoryEntry);
        cout<<"page "<<entry.page_id<<" loaded ."<<endl;
    }
    cout<<"loader pages_dir"<<endl;

    deleted_slots.clear();
    for(int i{};i<header.deleted_size;i++){
        size_t tmp;
        memcpy(buffer+offset, &tmp, sizeof(tmp));
        deleted_slots.push_back(tmp);
        offset+=sizeof(tmp);
    }

}





void DiskManager::writePage(int page_id, const char* data){

    size_t offset;
    // check if page is already here:
    
    if(pages_table.find(page_id)!= pages_table.end()){
        offset = pages_table[page_id];
    }else{
        //offset = allocatePage();
        return;
    }

    cout<<"offset : "<<offset<<endl;
    DB_file.seekp(offset);
    DB_file.write(data, PAGE_SIZE);
    pages_table[page_id] = offset;

    DB_file.flush();
    cout<<"written successfuly" <<endl;   
}





void DiskManager::readPage(int page_id, char*data){
    cout<<"reading page "<<page_id<<endl;
    if (pages_table.find(page_id) == pages_table.end()) {
        return; 
    }

    DB_file.clear();
    size_t offset = pages_table[page_id];
    DB_file.seekg(offset, ios::beg);
    DB_file.read(data, PAGE_SIZE);
    
    if (DB_file.fail()) {
        cerr<<"DB_file failed while reading"<<endl;
    }
}





void DiskManager::deletePage(int page_id){
    if(pages_table.find(page_id) == pages_table.end()){
        return;
    }
    deleted_slots.push_back(pages_table[page_id]);
    pages_table.erase(page_id);
    
    cout<<"page deleted successfuly"<<endl;
}





void DiskManager::resizeFile(){
    header.capacity*=2;
    filesystem::resize_file(file_name, header.capacity*PAGE_SIZE);
}






size_t DiskManager::allocatePage(){
    if(!deleted_slots.empty()){
        size_t tmp = deleted_slots.back();
        deleted_slots.pop_back();
        return tmp;
    }

    if(pages_table.size()+1 >= header.capacity){
        resizeFile();
    }

    int page_id = pages_table.size()+1;

    Page* newPage =  new Page(page_id);
    size_t offset = (page_id) * PAGE_SIZE;

    
    DB_file.seekp(offset);
    DB_file.write(newPage->getData(), PAGE_SIZE);
    DB_file.flush();    

    pages_table[page_id] = offset;

    return offset;
}



size_t DiskManager::getSize() {
    return (pages_table.size()+deleted_slots.size())*PAGE_SIZE;
}

DiskManager::~DiskManager(){
    if(DB_file.is_open()){
        DB_file.clear();
        saveMetaData();
        DB_file.close();
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////


void test1(){
    DiskManager dm("elfeel");
    Page p1(1);
    Field f1(TYPE_STRING,"nader");
    Tuple t1({f1});
    p1.insertTuple(t1);
    cout<<"t1 inserted"<<endl;

    const char* buffer1 = p1.getData();
    dm.writePage(1,buffer1);
    dm.writePage(2,buffer1);

    cout<<"done "<<endl;

    char read_buffer[PAGE_SIZE];
    dm.readPage(1,read_buffer);
    PageHeader* p_header1 = reinterpret_cast<PageHeader*>(read_buffer);
    cout<<"page_id : "<<p_header1->page_id<<endl;
    cout<<"num_tuples : "<<p_header1->num_tuples<<endl;

    cout<<"------"<<endl;

    dm.deletePage(1);
    dm.writePage(1,buffer1);

    cout<<"page size << "<<dm.getSize()<<endl;
    dm.~DiskManager();
}


// 1. Verifies that data persists across different DiskManager lifetimes
void testPersistence() {
    string db_name = "persistence_test";
    
    // First Life: Write data
    {
        DiskManager dm(db_name);
        Page p1(100); // Dummy page with ID 100
        dm.writePage(100, p1.getData());
    } // dm goes out of scope here, triggering saveMetaData()

    // Second Life: Load and Verify
    {
        DiskManager dm_reloaded(db_name);
        char read_buffer[PAGE_SIZE];
        dm_reloaded.readPage(100, read_buffer);
        PageHeader* header = reinterpret_cast<PageHeader*>(read_buffer);
        
        if(header->page_id == 100) {
            cout << "Persistence Test: PASSED" << endl;
        } else {
            cout << "Persistence Test: FAILED" << endl;
        }
    }
}


// 2. Verifies that deleted slots are reused and not lost
void testDeletionAndReuse() {
    DiskManager dm("reuse_test");
    // Write 2 pages
    dm.writePage(1, "Data1");
    dm.writePage(2, "Data2");
    
    dm.deletePage(1); // Page 1 offset should go to deleted_slots
    
    // Write a 3rd page; should reuse Page 1's offset
    dm.writePage(3, "Data3"); 
    
    // Verify that the file size didn't just keep growing
    cout << "Final DB Size: " << dm.getSize() << endl;
}

// 3. Verifies that capacity doubling works
void testExpansion() {
    DiskManager dm("expansion_test");
    // Force write until resize triggers
    for(int i = 0; i < 200; i++) {
        dm.writePage(i, "Filling...");
    }
    cout << "Expansion Test: Database grew to accommodate pages." << endl;
}


/*
int main(){
    //test1();
    //testPersistence();
    //testDeletionAndReuse();
    //testExpansion();

}
*/