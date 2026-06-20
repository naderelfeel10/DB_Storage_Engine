#include<iostream>
#include"Nested_loop_join.h"
#include<chrono>
using namespace std;

void NestedLoopJoin::open(){
    this->has_inner = false;
    this->outer_table->open();
    this->inner_table->open();
    this->get_output_schema();
}

void NestedLoopJoin::close(){
    this->has_inner = false;
    this->outer_table->close();
    this->inner_table->close();
}

vector<Column> NestedLoopJoin::get_output_schema(){
    vector<Column> outer_schema = this->outer_table->get_output_schema();
    vector<Column> inner_schema = this->inner_table->get_output_schema();

    // change col name to be table_name.col_name
    // that's how i can differentiate between cols with same names in diff tables
    string table_name = outer_table->getTableHeap()->getTableName();
    for(int i=0; i<outer_schema.size();i++){
        string col_name = outer_schema[i].getColName();
        //string table_name = outer_table->getTableHeap()->getTableName();
        outer_schema[i].setColName(table_name+'.'+col_name);
    }

    table_name = inner_table->getTableHeap()->getTableName();
    for(int i=0; i<inner_schema.size();i++){
        string col_name = inner_schema[i].getColName();
        //string table_name = inner_table->getTableHeap()->getTableName();
        inner_schema[i].setColName(table_name+'.'+col_name);
    }
    
    // concat
    outer_schema.insert(outer_schema.end(), inner_schema.begin(), inner_schema.end());

    this->output_schema = outer_schema;

    return output_schema;
}

bool NestedLoopJoin::getNext(Tuple*tuple){
    Tuple tmp_tuple2({});

    while (true) {
    // if the inner table does not have any more data for the curr outer tuple:
    // then check if outer table still have tuples
    // if yes then update to pointer of inner table to start from the begininig 
    // else return false as outer table has been finished
    if(!has_inner){
        if(this->outer_table->getNext(&this->curr_tuple)){
            has_inner = true;
            inner_table->open();
        }else{
            return false;
        }

    }

    //reaching this means we have to loop through inner table
    while(inner_table->getNext(&tmp_tuple2)){
        // i need to pass 2 2 tuples concatinatd together
        // i will extract the fields, concat then create new tuple with them
        vector<Field> fields1 = this->curr_tuple.fields;
        vector<Field> fields2 = tmp_tuple2.fields;

        fields1.insert(fields1.end(), fields2.begin(), fields2.end());
        Tuple res_tuple = Tuple(fields1);
        /*
        for(auto&o:this->output_schema){
            o.printCol();
        }
        cout<<"----"<<endl;
        for(auto&o:res_tuple.fields){
            o.print();
        }*/

        if (this->join_condition->evaluate(&res_tuple, this->output_schema)) {
            *tuple = res_tuple;
            return true;   
        }
    }
    // inner table is finished:
    // mark has inner as false
    this->has_inner = false;
    }
}



////////////////////////////////////////////
///////////////////////////////////////////

/*
vector<string> tables1;
map<string, TableHeap*>tables_map1;
map<string, vector<RID>> tables_rids1;
map<string, StaticHashIndexWrapper*>tables_indexes1;
map<string, BPlusTreeIndexWrapper*>tables_B_indexes1;


void createUserTable(BufferPoolManager* BPM, string table_name){
    tables1.push_back(table_name);

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
    tables_map1["User"] = user_table;
}


void insertIntoUserTable(){
    cout << "--- Inserting 10 records into User table ---" << endl;
    for (int i = 0; i < 5000; i++) {

        Field u1 = Field(TYPE_INT, 100 + i);                
        Field u2 = Field(TYPE_STRING, ("First_" + to_string(i)).c_str());
        Field u3 = Field(TYPE_STRING, ("Last_" + to_string(i)).c_str()); 
        Field u4 = Field(TYPE_INT, 20 + i);               

        Tuple t_user = Tuple({u1, u2, u3, u4});
        RID rid = tables_map1["User"]->insertTuple(t_user);
        tables_rids1["User"].push_back(rid);
    }
}




void createOrderTable(BufferPoolManager* BPM, string table_name){
    tables1.push_back(table_name);

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
    tables_map1["Order"] = order_table;
}

void insertIntoOrderTable(){
cout << "\n--- Inserting 10 records into Order table ---" << endl;
    for (int i = 0; i < 100000; i++) {
        Field o1 = Field(TYPE_INT, 9000 + i);          
        Field o2 = Field(TYPE_INT, 100 + (i%5000));                
        Field o3 = Field(TYPE_FLOAT, static_cast<float>(150.75 + (i * 20.5))); 
        Field o4 = Field(TYPE_BOOL, (i % 2 == 0));          

        Tuple t_order = Tuple({o1, o2, o3, o4});
        RID rid = tables_map1["Order"]->insertTuple(t_order);
        tables_rids1["Order"].push_back(rid);
    }
}



int main() {
    string DB_name = "testSeqScanDB";
    DiskManager* dm = new DiskManager(DB_name);
    BufferPoolManager* BPM = new BufferPoolManager(dm);

    createUserTable(BPM, "User");
    insertIntoUserTable(); 


    AbstractExecuter* seq_scan = new SeqScan(tables_map1["User"]);

    Column* col_id = new Column(TYPE_INT, "user_id", 4);    //TUPLE[0]
    Column* col_first = new Column(TYPE_STRING, "firstName", 30); //TUPLE[1]
    Column* col_last = new Column(TYPE_STRING, "lastName", 30);   //TUPLE[2]

    //(TUPLE[0] > 102) AND (TUPLE[0] <= 107)
    Column* const_102 = new Column(new Field(TYPE_INT, 102), "C102",4);
    AbstractPredicate* leaf1 = new Predicate(col_id, const_102, PredicateType::GT);

    Column* const_107 = new Column(new Field(TYPE_INT, 107), "C107",4);
    AbstractPredicate* leaf2 = new Predicate(col_id, const_107, PredicateType::LE);

    AbstractPredicate* and_gate = new ComplexPredicate(leaf1, leaf2, ComplexPredicateType::AND);

    AbstractPredicate* leaf4 = new Predicate(col_last, col_first, PredicateType::EQ);

    AbstractPredicate* root_expression = new ComplexPredicate(and_gate, leaf4, ComplexPredicateType::OR);



    AbstractExecuter* select = new Select(seq_scan, root_expression);

    vector<string>projection_cols = {"user_id","firstName"};
    Projection* projection = new Projection(select, projection_cols);


    createOrderTable(BPM, "Order");
    insertIntoOrderTable();

    AbstractExecuter* order_seq_scan = new SeqScan(tables_map1["Order"]);

    Column* order_col_id = new Column(TYPE_INT, "Order.user_id", 4);
    Column* user_col_id = new Column(TYPE_INT, "User.user_id", 4);

    AbstractPredicate* join_pred = new Predicate(order_col_id, user_col_id, PredicateType::EQ);

    AbstractExecuter* nested_loop_join = new NestedLoopJoin(projection, order_seq_scan,join_pred);


    auto st1 = chrono::high_resolution_clock::now();

    nested_loop_join->open();

    cout<<"executing the query"<<endl;
    Tuple* result_row = new Tuple({});

    for(auto&col:projection_cols)cout<<col<<"           |";
    cout<<endl;
    while (nested_loop_join->getNext(result_row)) {
        result_row->print();
    }
    cout<<"done"<<endl;

    nested_loop_join->close();

    auto end1 = chrono::high_resolution_clock::now();
    auto duration1 = chrono::duration_cast<chrono::microseconds>(end1 - st1);
    cout << "searching using  nested loop join duration: " << duration1.count() << " microseconds" << endl;



    return 0;
}
*/