#include <gtest/gtest.h>
#include "Database.h"

#include <iostream>

class DatabaseTest : public ::testing::Test {
protected:
    memdb::Database& db = memdb::Database::getInstance();
};

void PrintVariant(const std::string& col_name, const config::ColumnValue& col_value){
    if(std::holds_alternative<int>(col_value)){
        std::cout << col_name << ": " << std::get<int>(col_value) << ", ";
    }
    else if(std::holds_alternative<bool>(col_value)){
        if(std::get<bool>(col_value)){
            std::cout << col_name << ": true, ";
        }else{
            std::cout << col_name << ": false, ";
        }
    }
    else if(std::holds_alternative<std::string>(col_value)){
        std::cout << col_name << ": " << std::get<std::string>(col_value) << ", ";
    }
    else if(std::holds_alternative<config::BitString>(col_value)){
        auto temp = std::get<config::BitString>(col_value);
        std::cout << col_name << ": ";
        for(const auto& te: temp){
            std::cout << te;
        }
        std::cout << ", ";
    }
    else{
        std::cout << col_name << ": NULL, ";
    }
}

void PrintData(const std::unordered_map<int, config::RowType> &data, const std::string& query){
    std::cout << " << " << query << " >>\n";
    for (const auto& [key, row] : data) {
        for(const auto& [col_name, col_value] : row){
            PrintVariant(col_name, col_value);
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

TEST_F(DatabaseTest, BigUpdate){
    std::string create_table_query = "create table medicine ({key, autoincrement} id : int32, doctors : string[32], equipment : bool, age : int32)";
    auto res = db.execute(create_table_query);

    std::string create_index_query = "create ordered index on medicine by id";
    auto res1 = db.execute(create_index_query);

    for (int i = 0; i <= 100; ++i) {
        std::string insert_query = R"(insert (doctors = "Strange)" + std::to_string(i) + R"(", age =)" + std::to_string(i) + R"() to medicine)";
        auto res2 = db.execute(insert_query);
    }

    PrintData(db.getTable("medicine")->getData(), "all Table");

    std::unique_ptr<memdb::Response> res5;
    std::string select_query = R"(update medicine set doctors = doctors + "1" where ( ( id % 2 = 1 && id > 200 ) || ( doctors <= "Strange5" ) ) && ( id = age ) && ( id < 400 ) && id >= 99)";
    res5 = db.execute(select_query);
    auto data = res5->getData();
    std::cout << res5->getMessage();
    PrintData(db.getTable("medicine")->getData(), select_query);
}
