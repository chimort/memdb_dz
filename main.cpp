#include "Database.h"
#include <iostream>

int main() {
    memdb::Database& db = memdb::Database::getInstance();

    // Create a table to insert records into
    // std::string create_table_query = "create table my_table (id, name, age)";
    // db.execute(create_table_query);

    // Insert a record into the table
    std::string insert_query = "insert (id = 1, name = 'Alice', age = 30) to users";
    std::string dsag = "insert (,\"vasya\", 0xdeadbeefdeadbeef) to users";
    //std::string fdh = "insert (\nis_admin = true,\nlogin = \"admin\",\npassword_hash = 0x0000000000000000\t) to users";
    std::string sel = "select id, login from users where is_admin || id < 10";
    std::string del = "delete users where |login| % 2 = 0";
    std::string updt = "update users set login = login + \"_deleted\", is_admin = false where\npassword_hash < 0x00000000ffffffff";

    
    auto response = db.execute(dsag);
    db.execute(sel);
    db.execute(del);
    db.execute(updt);
    db.execute(insert_query);


    if (response->getStatus()) {
        std::cout << "Success: " << response->getMessage() << std::endl;
    } else {
        std::cout << "Error: " << response->getMessage() << std::endl;
    }

    return 0;
}