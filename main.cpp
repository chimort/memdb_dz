#include "Database.h"
#include "utils.h"
#include <iostream>

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

int main() {
    memdb::Database &db = memdb::Database::getInstance();

    std::string create_table_query = "create tABle family ({unique} id : int32, name : string[32], isParent : bool, code : bytes[3])";

    auto res = db.execute(create_table_query);

    auto res1 = db.execute(R"(insert (id = 1, name = "Anna", isParent = false, code = 0x0ffee3) to family)");
    auto res2 = db.execute(R"(insert (id = 2, name = "Ivan", isParent = true, code = 0x0e572f) to family)");

    std::string delete_query = R"(DELETE family where code = 0x0ffee3)";
    auto res3 = db.execute(delete_query);

    auto table = db.getTable("family");
    auto data_after = table->getData();

    PrintData(data_after, delete_query);
}
