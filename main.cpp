#include "Database.h"
#include <iostream>

int main() {
    memdb::Database& db = memdb::Database::getInstance();

    // Create a table to insert records into
    // std::string create_table_query = "create table my_table (id, name, age)";
    // db.execute(create_table_query);

    // Insert a record into the table
    std::string insert_query = "insert (id = 1, name = 'Alice', age = 30) to my_table";
    memdb::Response response = db.execute(insert_query);

    if (response.getStatus()) {
        std::cout << "Success: " << response.getMessage() << std::endl;
    } else {
        std::cout << "Error: " << response.getMessage() << std::endl;
    }

    return 0;
}