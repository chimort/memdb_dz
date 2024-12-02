#include "Database.h"
#include "Print_tests.h"
#include "utils.h"
#include <iostream>


int main() {
    memdb::Database &db = memdb::Database::getInstance();
    std::string create_table_query = "create table medicine ({key, autoincrement} id : int32, doctors : string[32], equipment : bool, age : int32)";
    auto res = db.execute(create_table_query);

    std::string create_index_query = "create ordered index on medicine by id";
    auto res1 = db.execute(create_index_query);

    for (int i = 0; i <= 10; ++i) {
        std::string insert_query = R"(insert (doctors = "Strange)" + std::to_string(i) + R"(", age =)" + std::to_string(i) + R"() to medicine)";
        auto res2 = db.execute(insert_query);
    }

    PrintData(db.getTable("medicine")->getData(), "all Table");

    std::unique_ptr<memdb::Response> res5;
    std::string select_query = R"(update medicine set doctors = doctors + "1" where ( ( id % 2 = 1 && id > 200 ) || ( doctors <= "Strange5" ) ) && ( id = age ) && ( id < 40 ) && id >= 9)";
    res5 = db.execute(select_query);
    auto data = res5->getData();
    std::cout << res5->getMessage();
    PrintData(db.getTable("medicine")->getData(), select_query);
}
