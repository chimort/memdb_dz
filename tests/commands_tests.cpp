#include <gtest/gtest.h>
#include "Database.h"

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
        std::cout << col_name << ": " << std::get<int>(col_value) << ", ";
    }
    else{

    }
}

void PrintData(const std::unordered_map<int, config::RowType> &data){
    for (const auto& [key, row] : data) {
        auto id_opt = utils::get<int>(row, "id");
        int id = id_opt.has_value() ? id_opt.value() : -1;

        auto login_opt = utils::get<std::string>(row, "login");
        std::string login = login_opt.has_value() ? login_opt.value() : "N/A";

        auto mom_opt = utils::get<std::string>(row, "mom");
        std::string mom = mom_opt.has_value() ? mom_opt.value() : "N/A";

        auto is_admin_opt = utils::get<bool>(row, "is_admin");
        bool is_admin = is_admin_opt.has_value() ? is_admin_opt.value() : false;

        auto is_parent_opt = utils::get<bool>(row, "is_parent");
        bool is_parent = is_parent_opt.has_value() ? is_parent_opt.value() : false;

        std::cout << "id: " << id
                  << ", login: " << login
                  << ", mom: " << mom
                  << ", is_admin: " << (is_admin ? "true" : "false")
                  << ", is_parent: " << (is_parent ? "true" : "false")
                  << std::endl;

    }
}

TEST_F(DatabaseTest, SelectData) {
    std::string create_table_query = "create table users ({key, autoincrement} id : int32, {unique} login: string[8], password_hash: bytes[8], is_admin: bool = false, age: int32, mom: string[32], is_parent: bool)";
    db.execute(create_table_query);
    db.execute(R"(insert (login = "Alice", password_hash = 0x1111111111111111, is_admin = true, age = 25, mom = "Eve") to users)");
    db.execute(R"(insert (login = "Bob", age = 30, mom = "Martha", is_parent = true) to users)");
    db.execute(R"(insert (login = "Carol", password_hash = 0x3333333333333333, is_admin = false, age = 22, is_parent = false) to users)");
    db.execute(R"(insert (login = "Dave", password_hash = 0x4444444444444444, is_admin = true, age = 15, mom = "Sara", is_parent = true) to users)");
    db.execute(R"(insert (login = "Eve", password_hash = 0x5555555555555555, is_admin = false, mom = "Nancy", is_parent = false) to users)");
    db.execute(R"(insert (login = "Frank", is_admin = true, age = 40) to users)");
    db.execute(R"(insert (login = "Grace", password_hash = 0x7777777777777777, is_admin = false, age = 29, mom = "Judy", is_parent = true) to users)");
    db.execute(R"(insert (password_hash = 0x8888888888888888, is_admin = true, mom = "Eve", is_parent = false) to users)");
    db.execute(R"(insert (login = "Ivy", password_hash = 0x9999999999999999, is_admin = false, age = 23, mom = "Helen", is_parent = true) to users)");
    db.execute(R"(insert (login = "Jack", password_hash = 0xAAAAAAAAAAAAAAAA, is_admin = true, age = 31, mom = "Diane") to users)");

    //std::string select_query_2 = "select id, login, mom, is_parent, is_admin from users where id % 2 = 1 && ( | login | < 7 || | mom | < 0 ) && ( is_admin = true )";
    //std::string select_query_2 = "select id, login, mom, is_parent, is_admin from users where ";

    std::string check_query = "select id, login, mom, is_parent, is_admin from users where true";
    auto check_response = db.execute(check_query);
    const auto &check_data = check_response->getData();
    PrintData(check_data);

    std::string select_query = "select login from users where | login | < 5";
    auto select_response = db.execute(select_query);
    const auto &select_data = select_response->getData();
    PrintData(select_data);
}