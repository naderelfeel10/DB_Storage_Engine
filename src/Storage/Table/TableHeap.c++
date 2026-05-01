#include<iostream>
#include"TableHeap.h"
using namespace std;


// upcomming updates :
/*
 1. split initiating new Table heap into a different function
 2. check first if the table exists before creating one
 3. loadMeta implementation 
*/
TableHeap::TableHeap(BufferPoolManager* BPM){
    this->BPM = BPM;
    // let first page for meta data
    first_page_id = BPM->newPage();

    last_page_id = BPM->newPage();
};



// insert a tuple into the table heap
RID TableHeap::insertTuple(Tuple tuple){
    
    // seleect last page in the table
    char* page_buffer = BPM->fetchPage(last_page_id);
    PageHeader* pageHeaer = reinterpret_cast<PageHeader*>(page_buffer);

    Page* page = reinterpret_cast<Page*>(page_buffer);
    //Page* page = new (page_buffer)Page(last_page_id);
    
    // this returns -1 if there is no enough space
    int slot_num  = page->insertTuple(tuple);

    cout<<pageHeaer->next_page_id<< " || " <<pageHeaer->page_id<<" || "<<pageHeaer->num_tuples<<endl;

    // we need to allocate new page
    if(slot_num == -1){
        cout<<"allocating new page "<<endl;
        
        // create it
        int new_page_id = BPM->newPage();
        //update last page next pointer to point to the new page 
        pageHeaer->next_page_id = new_page_id;
        // marking as dirty to be written on disk latter
        BPM->markAsDirty(last_page_id);

        // fetch the new page
        char* page_buffer = BPM->fetchPage(new_page_id);
        
        PageHeader* new_pageHeader = reinterpret_cast<PageHeader*>(page_buffer);
        Page* new_page = reinterpret_cast<Page*>(page_buffer);

        last_page_id = new_page_id;
        // insert the tuple .
        int new_slot_num  = new_page->insertTuple(tuple);
        BPM->markAsDirty(new_page_id);

        cout<<"tuple is inserted successfuly , "<< new_page_id<<" "<<new_slot_num<<endl;
        return RID(new_page_id, new_slot_num);

    }
    // else means there is enough space in this page
    BPM->markAsDirty(last_page_id); // just for the buffer pool to flush the page into the file 
    
    cout<<"tuple is inserted successfuly , "<< last_page_id<<" "<<slot_num<<endl;
    return RID(last_page_id, slot_num);
}


// return a tuple with this rid
Tuple* TableHeap::getTuple(RID rid){
    // fetch the page
    char* page_buffer = BPM->fetchPage(rid.getActualPair().getPageId());
    Page* page = reinterpret_cast<Page*>(page_buffer);

    // select this tuple from the page
    Tuple* tuple = new Tuple({});
    page->getTuple(rid.getActualPair().getSlotNum(),*tuple);
    
    return tuple;
}


// we need the old rid, and the new tuple
RID TableHeap::updateTuple(RID rid, Tuple tuple){

    // select the page
    char* page_buffer = BPM->fetchPage(rid.getPageId());
    Page* page = reinterpret_cast<Page*>(page_buffer);

    // get the old tuple
    Tuple* old_tuple = this->getTuple(rid);

    // if new tuple size > old one size
    // we need to delete the old one, insert the new one in another place with another RID
    if(old_tuple->getTupleSize() < tuple.getTupleSize()){
      
        page->deleteTuple(rid.getSlotNum());
        RID new_rid = insertTuple(tuple);
        rid.updateActualPair(new_rid);
        
    }
    else{
        page->updateTuple(rid.getSlotNum(), tuple);
    }

    return rid;

}


void TableHeap::deleteTuple(RID rid){

    // select the targted page from the buffer
    char* page_buffer = BPM->fetchPage(rid.getPageId());
    Page* page = reinterpret_cast<Page*>(page_buffer);

    // just delete the selected slot num
    bool res = page->deleteTuple(rid.getActualPair().getSlotNum());
}



void TableHeap::displayTablePages() {
    int next = first_page_id;
    std::cout << "\n--- Table Heap Structure ---\n";
    
    while (next != -1) {
        char* page_buffer = BPM->fetchPage(next);
        PageHeader* header = reinterpret_cast<PageHeader*>(page_buffer);
        
        // Print detailed info for each link in the chain
        std::cout << "[Page " << next << " | Tuples: " << header->num_tuples 
                  << " | Next: " << header->next_page_id << "]" << std::endl;
        
        next = header->next_page_id;
    }
    std::cout << "--- End of Table ---\n";
}


//saves table meta data at first_page of the table 
void TableHeap::saveMetaData(){

    char buffer[PAGE_SIZE];
    int offset =0;
    memset(buffer,0 ,PAGE_SIZE);
    

    //1. save table name size
    int table_name_size = table_name.length();
    memcpy(buffer + offset , &table_name_size, sizeof(table_name_size));
    offset += sizeof(table_name_size);


    // save table name itself
    memcpy(buffer + offset , table_name.c_str(), table_name_size);
    offset += table_name_size;


    // save first , last page ids
    memcpy(buffer+offset , &this->first_page_id, sizeof(first_page_id));
    offset += sizeof(this->first_page_id);

    memcpy(buffer+offset , &this->last_page_id, sizeof(last_page_id));
    offset += sizeof(this->last_page_id);

    // save cols size
    int num_cols = this->cols.size();
    memcpy(buffer+offset , &num_cols, sizeof(num_cols));
    offset += sizeof(num_cols);


    // save each col
    for(auto& c : this->cols){
        c.serializeCol(buffer+ offset);
        offset += c.getColSize();
    }

    // fetch this page and write this data into it , then mark as dirty to be written on disk later
    char* page_buffer = BPM->fetchPage(first_page_id);
    memcpy(page_buffer, buffer, PAGE_SIZE);
    BPM->markAsDirty(first_page_id);

}


// to do
void TableHeap::loadMetaData(){}



vector<Column> TableHeap::getCols(){
    return this->cols;
}


void TableHeap::setCols(vector<Column> cols){
    this->cols = std::move(cols);
}


string TableHeap::getTableName(){
    return this->table_name;
}


void TableHeap::setTableName(string table_name){
    this->table_name = table_name;
}


void TableHeap::printColumns(){
    for(auto& col : cols){
        col.printCol();
        cout<<"----"<<endl;
    }
}


//indexes :
void TableHeap::createIndex(indexes_t index_type, string col_name,int index_size){
    Index* index;
    // switch case for all kinds of indexes (for now it's just static hash index)
    switch (index_type)
    {
        // first index
    case STATIC_HASH_INDEX:{
        //now it's just int , i need to change this into FieldType and check the specific type then create the propper index

        //get col field type
        FieldType field_tye;
        for(auto& col: this->cols){
            if(col_name == col.getColName()){
                field_tye = col.getField()->getFieldType();
            }
        }

        // create a static hash index based on this field type, with needed size
        hashIndex* h_index = new hashIndex(index_size);
        Index* index = new StaticHashIndexWrapper(h_index , col_name,field_tye); 
        indexes_map[col_name].push_back(index);

        Index_pages_struct* pages_struct = new Index_pages_struct();
        indexes_pages_ids[pair(col_name, STATIC_HASH_INDEX)] = pages_struct;
        
        break;
    }
    case BPLUS_TREE_INDEX:{

        FieldType fieldype;
        for(auto& col:this->cols){
            if(col.getColName() == col_name){
                fieldype = col.getField()->getFieldType();
            }
        }

        BPlusTree* b_index = new BPlusTree(index_size);
        index = new BPlusTreeIndexWrapper(b_index, col_name, fieldype);
        this->indexes_map[col_name].push_back(index);

        Index_pages_struct* pages_struct = new Index_pages_struct();
        indexes_pages_ids[pair(col_name, BPLUS_TREE_INDEX)] = pages_struct;
        
        break;
    }

    
    default:

        break;
    }
}
TableHeap::~TableHeap(){
    saveMetaData();
}


/*
int
main(){
    
    DiskManager* dm = new DiskManager("tableHeapDB");
    BufferPoolManager* BPM = new BufferPoolManager(dm);
    
    TableHeap table_heap = TableHeap(BPM);

    Field f1(TYPE_STRING,"nadermohamedelfeelisthebestevernadermohamedelfeelisthebestever");
    Tuple t1({f1,f1});
    
    for (int i = 1; i <= 100; ++i) {
        RID rid = table_heap.insertTuple(t1).getActualPair();
        if (rid.getPageId() != -1) {
            std::cout << "rid" << i << " : " 
                  << rid.getPageId() << " , " 
                  << rid.getSlotNum() << std::endl;
        } else {
        std::cerr << "Failed to insert tuple " << i << std::endl;
        }
    }
    
    table_heap.displayTablePages();

    Tuple* t2 = table_heap.getTuple(RID(1,1));
    t2->print();
    Tuple t3({f1,f1,f1});
    RID rid3 =  table_heap.updateTuple(RID(1,1),t3);
    t2 = table_heap.getTuple(rid3.getActualPair());
    t2->print();

    table_heap.deleteTuple(rid3.getActualPair());
    t2 = table_heap.getTuple(rid3.getActualPair());
    t2->print();
    
    dm->~DiskManager();
    BPM->~BufferPoolManager();   

}
*/