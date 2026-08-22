#include"BindContext.h"
using namespace std;


BoundColumnRef* BindContext::ResolveColumn(const string& table_name, const string& column_name){ 
    
    // using table_name, and col_name 
    // find the actual ColRef and return it, else return not found or repeated col
    BoundColumnRef* result;
    bool found = false;

    for(const auto& table : tables){

        // if table_name is not equal to the targted, continue to the next table
        if (!table_name.empty() &&table.alias != table_name && table.table_name != table_name){
            continue;
        }

        // if the table is found, then check if it has the targted col
        int col_id = 0;
        FieldType col_type;
        bool col_found{false};

        for(Column col: table.schema){
            if(col.getColName() == column_name){
                col_found=true;
                col_type = col.getColType();
                continue;
            }
            col_id++;
        }

        if(!col_found)continue;

        //check if col with same name and same table name is fouond
        if(found){
            cerr<<"Ambiguous column: " + column_name<<endl;
        }

        // construct the rsult
        result->table_oid = table.table_oid;
        result->column_oid = col_id;

        result->table_name = table.alias.empty()? table.table_name : table.alias;
        result->column_name = column_name;
        result->return_type = col_type;

        found = true;
    }

    if(!found){
        cerr<<"Column does not exist: " + column_name<<endl;
    }

    return result;
}


int
main(){

}