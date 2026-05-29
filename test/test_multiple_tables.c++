#include<iostream>
#include<vector>
#include<algorithm>
#include"../src/Storage/Table/TableIterator.h"
#include"../src/Storage/Table/Column.h"
#include"../src/Storage/Indexing/Index.h"
#include"../src/Storage/Indexing/StaticHashIndexWrapper.h"
#include<chrono>
#include<cassert>
#include"../src/Storage/Indexing/BPlusTreeIndexWrapper.h"

using namespace std;

vector<string> tables;
map<string, TableHeap*>tables_map;
map<string, vector<RID>> tables_rids;
map<string, StaticHashIndexWrapper*>tables_indexes;
map<string, BPlusTreeIndexWrapper*>tables_B_indexes;


void createUserTable(BufferPoolManager* BPM, string table_name){
    tables.push_back(table_name);

    Column t1_col1 = Column(TYPE_INT, "user_id", sizeof(int));
    Column t1_col2 = Column(TYPE_STRING, "firstName", 30);
    Column t1_col3 = Column(TYPE_STRING, "lastName", 30);
    Column t1_col4 = Column(TYPE_INT, "age", sizeof(int));
    vector<Column> t1_cols = {t1_col1, t1_col2, t1_col3, t1_col4};

    int t1_first_page_id{-1};
    if(BPM->disk_manager->tables_names.find(table_name) != BPM->disk_manager->tables_names.end()){
        cout << table_name << "found in the database" << endl;
        t1_first_page_id = BPM->disk_manager->tables_names[table_name];
    }

    TableHeap* user_table = new TableHeap(BPM, t1_first_page_id, -1);
    BPM->disk_manager->addTable(table_name, user_table->get_first_page_id());
    user_table->setCols(t1_cols);
    user_table->setTableName(table_name);
    tables_map["User"] = user_table;
}


void insertIntoUserTable(){
    cout << "--- Inserting 10 records into User table ---" << endl;
    for (int i = 0; i < 1000; i++) {

        Field u1 = Field(TYPE_INT, 100 + i);                
        Field u2 = Field(TYPE_STRING, ("First_" + to_string(i)).c_str());
        Field u3 = Field(TYPE_STRING, ("Last_" + to_string(i)).c_str()); 
        Field u4 = Field(TYPE_INT, 20 + i);               

        Tuple t_user = Tuple({u1, u2, u3, u4});
        RID rid = tables_map["User"]->insertTuple(t_user);
        tables_rids["User"].push_back(rid);
    }
}



void createProductTable(BufferPoolManager* BPM, string table_name){
    tables.push_back(table_name);

    Column t2_col1 = Column(TYPE_INT, "product_id", sizeof(int));
    Column t2_col2 = Column(TYPE_STRING, "productName", 50); 
    Column t2_col3 = Column(TYPE_FLOAT, "price", sizeof(float)); 
    Column t2_col4 = Column(TYPE_INT, "stock", sizeof(int));
    vector<Column> t2_cols = {t2_col1, t2_col2, t2_col3, t2_col4};

    int t2_first_page_id{-1};
    if(BPM->disk_manager->tables_names.find(table_name) != BPM->disk_manager->tables_names.end()){
        cout << table_name << " found in the database" << endl;
        t2_first_page_id = BPM->disk_manager->tables_names[table_name];
    }

    TableHeap* product_table = new TableHeap(BPM, t2_first_page_id, -1);
    BPM->disk_manager->addTable(table_name, product_table->get_first_page_id());
    product_table->setCols(t2_cols);
    product_table->setTableName(table_name);
    tables_map["Product"] = product_table;
}

void insertIntoProductTable(){
    cout << "\n--- Inserting 10 records into Product table ---" << endl;
    for (int i = 0; i < 1000; i++) {
        Field p1 = Field(TYPE_INT, 500 + i);                
        Field p2 = Field(TYPE_STRING, ("Product_" + to_string(i)).c_str());
        Field p3 = Field(TYPE_FLOAT, static_cast<float>(10.5 * (i + 1)));
        Field p4 = Field(TYPE_INT, 10 * (i + 1));

        Tuple t_product = Tuple({p1, p2, p3, p4});
        RID rid = tables_map["Product"]->insertTuple(t_product);
        tables_rids["Product"].push_back(rid);

    }
}

void createOrderTable(BufferPoolManager* BPM, string table_name){
    tables.push_back(table_name);

    Column t3_col1 = Column(TYPE_INT, "order_id", sizeof(int));
    Column t3_col2 = Column(TYPE_INT, "user_id", sizeof(int));
    Column t3_col3 = Column(TYPE_FLOAT, "totalAmount", sizeof(float));
    Column t3_col4 = Column(TYPE_BOOL, "isShipped", sizeof(bool));
    vector<Column> t3_cols = {t3_col1, t3_col2, t3_col3, t3_col4};

    int t3_first_page_id{-1};
    if(BPM->disk_manager->tables_names.find(table_name) != BPM->disk_manager->tables_names.end()){
        cout << table_name << " found in the database" << endl;
        t3_first_page_id = BPM->disk_manager->tables_names[table_name];
    }

    TableHeap* order_table = new TableHeap(BPM, t3_first_page_id, -1);
    BPM->disk_manager->addTable(table_name, order_table->get_first_page_id());
    order_table->setCols(t3_cols);
    order_table->setTableName(table_name);
    tables_map["Order"] = order_table;
}

void insertIntoOrderTable(){
cout << "\n--- Inserting 10 records into Order table ---" << endl;
    for (int i = 0; i < 1000; i++) {
        Field o1 = Field(TYPE_INT, 9000 + i);          
        Field o2 = Field(TYPE_INT, 100 + i);                
        Field o3 = Field(TYPE_FLOAT, static_cast<float>(150.75 + (i * 20.5))); 
        Field o4 = Field(TYPE_BOOL, (i % 2 == 0));          

        Tuple t_order = Tuple({o1, o2, o3, o4});
        RID rid = tables_map["Order"]->insertTuple(t_order);
        tables_rids["Order"].push_back(rid);
    }
}


////////////////////////

void buildUserIndex(TableHeap* user_table,const vector<RID>& user_rids, BufferPoolManager* BPM) {
    if (user_rids.empty()) return;

    string index_col_name = "user_id";
    user_table->createIndex(STATIC_HASH_INDEX, index_col_name, 300);
    
    StaticHashIndexWrapper* s_hash_wrapper = static_cast<StaticHashIndexWrapper*>(
        user_table->getIndex(index_col_name, STATIC_HASH_INDEX)
    );

    TableIterator table_iterator(user_table, user_rids.front(), user_rids.back());

    for (; !table_iterator.end(); ++table_iterator) {
        Tuple tuple = *table_iterator;
        int counter = 0;
        
        for (auto& col : user_table->getCols()) {
            if (col.getColName() == index_col_name) {
                s_hash_wrapper->Insert(
                    tuple.fields[counter], 
                    index_col_name, 
                    user_table->getCols(), 
                    table_iterator.getCurrRIDPointer()
                );
            }
            counter++;
        }
    }
    cout << "Successfully generated Static Hash Index on [User." << index_col_name << "]" << endl;
    tables_indexes["User"] = s_hash_wrapper;
}


void buildBPLUSIndex(string type,TableHeap* table,const vector<RID>& rids, BufferPoolManager* BPM) {
    if (rids.empty()) return;


    string index_col_name;
    if(type=="User")
        index_col_name = "user_id";
    else if(type=="Product")
        index_col_name = "product_id";
    else if(type=="Order")
        index_col_name = "order_id";

    table->createIndex(BPLUS_TREE_INDEX, index_col_name, 16);
    
    BPlusTreeIndexWrapper* B_tree_wrapper = static_cast<BPlusTreeIndexWrapper*>(
        table->getIndex(index_col_name, BPLUS_TREE_INDEX)
    );

    TableIterator table_iterator(table, rids.front(), rids.back());

    for (; !table_iterator.end(); ++table_iterator) {
        Tuple tuple = *table_iterator;
        int counter = 0;
        
        for (auto& col : table->getCols()) {
            if (col.getColName() == index_col_name) {
                B_tree_wrapper->Insert(
                    tuple.fields[counter], 
                    index_col_name, 
                    table->getCols(), 
                    table_iterator.getCurrRIDPointer()
                );
            }
            counter++;
        }
    }
    cout << "Successfully generated Static Hash Index on [User." << index_col_name << "]" << endl;
    if(type=="User")
        tables_B_indexes["User"] = B_tree_wrapper;
    else if(type=="Product")
        tables_B_indexes["Product"] = B_tree_wrapper;
    else if(type=="Order")
        tables_B_indexes["Order"] = B_tree_wrapper;
}



void buildProductIndex(TableHeap* product_table, const vector<RID>& product_rids, BufferPoolManager* BPM) {
    if (product_rids.empty()) return;

    string index_col_name = "product_id";
    product_table->createIndex(STATIC_HASH_INDEX, index_col_name, 300);
    
    StaticHashIndexWrapper* s_hash_wrapper = static_cast<StaticHashIndexWrapper*>(
        product_table->getIndex(index_col_name, STATIC_HASH_INDEX)
    );

    TableIterator table_iterator(product_table, product_rids.front(), product_rids.back());

    for (; !table_iterator.end(); ++table_iterator) {
        Tuple tuple = *table_iterator;
        int counter = 0;
        
        for (auto& col : product_table->getCols()) {
            if (tuple.get_is_deleted() || tuple.fields.empty()) {
                continue; 
            }
            if (col.getColName() == index_col_name) {
                s_hash_wrapper->Insert(
                    tuple.fields[counter], 
                    index_col_name, 
                    product_table->getCols(), 
                    table_iterator.getCurrRIDPointer()
                );
            }
            counter++;
        }
    }
    cout << "Successfully generated Static Hash Index on [Product." << index_col_name << "]" << endl;
    tables_indexes["Product"] = s_hash_wrapper;
}



void buildOrderIndex(TableHeap* order_table, const vector<RID>& order_rids, BufferPoolManager* BPM) {
    if (order_rids.empty()) return;

    string index_col_name = "order_id";
    order_table->createIndex(STATIC_HASH_INDEX, index_col_name, 300);
    
    StaticHashIndexWrapper* s_hash_wrapper = static_cast<StaticHashIndexWrapper*>(
        order_table->getIndex(index_col_name, STATIC_HASH_INDEX)
    );

    TableIterator table_iterator(order_table, order_rids.front(), order_rids.back());

    for (; !table_iterator.end(); ++table_iterator) {
        Tuple tuple = *table_iterator;
        int counter = 0;
        
        for (auto& col : order_table->getCols()) {
            if (col.getColName() == index_col_name) {
                s_hash_wrapper->Insert(
                    tuple.fields[counter], 
                    index_col_name, 
                    order_table->getCols(), 
                    table_iterator.getCurrRIDPointer()
                );
            }
            counter++;
        }
    }
    cout << "Successfully generated Static Hash Index on [Order." << index_col_name << "]" << endl;
    tables_indexes["Order"] = s_hash_wrapper;
}


int main(){

    string query1 = "create database StoreDB";
    string DB_name = "StoreDB";
    
    DiskManager* dm = new DiskManager(DB_name);
    BufferPoolManager* BPM = new BufferPoolManager(dm);

    // 1. User (user_id int, firstName varchar(30), lastName varchar(30), age int)
    createUserTable(BPM, "User");

    // 2. TABLE DEFINITION: Product (product_id int, productName varchar(50), price float, stock int)
    createProductTable(BPM, "Product");

    // 3. TABLE DEFINITION: Order (order_id int, user_id int, totalAmount float, isShipped bool)
    createOrderTable(BPM, "Order");

    cout << "\n================= SYSTEM STORAGE REPORT =================" << endl;
    cout << " File name         : " << dm->file_name << endl;
    cout << " Registered Tables : " << dm->tables_names.size() << endl;
    cout << " Managed Frames    : " << dm->pages_table.size() << endl;
    cout << "--------------------------------------------------------" << endl;

    for(auto& e : dm->tables_names){
        cout << "Table: " << left << setw(12) << e.first 
             << " -> Root Page ID: " << e.second << endl;
    }
    cout << "========================================================\n" << endl;

    tables_map["User"]->printColumns();
    tables_map["Product"]->printColumns();
    tables_map["Order"]->printColumns();




    // Some insertions:

    // insert into User
    insertIntoUserTable();
    // insert into Product
    insertIntoProductTable();
    // insert into Order
    insertIntoOrderTable();

    // display table pages :
    cout<<"USER table"<<endl;
    tables_map["User"]->displayTablePages();
    cout<<"PRODUCT table"<<endl;
    tables_map["Product"]->displayTablePages();
    cout<<"ORDER table"<<endl;
    tables_map["Order"]->displayTablePages();


    // create hash index for each table
    buildUserIndex(tables_map["User"],tables_rids["User"], BPM);
    buildProductIndex(tables_map["Product"],tables_rids["Product"], BPM);
    buildOrderIndex(tables_map["Order"],tables_rids["Order"], BPM);

    // B plus
    buildBPLUSIndex("User",tables_map["User"],tables_rids["User"], BPM);
    buildBPLUSIndex("Product",tables_map["Product"],tables_rids["Product"], BPM);
    buildBPLUSIndex("Order",tables_map["Order"],tables_rids["Order"], BPM);


    // display pages :
    cout<<"USER index"<<endl;
    tables_indexes["User"]->displayIndexPages();
    cout<<"PRODUCT index"<<endl;
    tables_indexes["Product"]->displayIndexPages();
    cout<<"ORDER index"<<endl;
    tables_indexes["Order"]->displayIndexPages();


    cout<<"Bplus User index"<<endl;
    tables_B_indexes["User"]->displayIndexPages();



    delete tables_map["User"];
    delete tables_map["Product"];
    delete tables_map["Order"];

    delete tables_indexes["User"];
    delete tables_indexes["Product"];
    delete tables_indexes["Order"];

    //delete tables_B_indexes["User"];

    tables_B_indexes["User"]->BPlusTreeIndex->saveBPlusTree();
    tables_B_indexes["Product"]->BPlusTreeIndex->saveBPlusTree();
    tables_B_indexes["Order"]->BPlusTreeIndex->saveBPlusTree();



    delete BPM; 
    delete dm;

}
