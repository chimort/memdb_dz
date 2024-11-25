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
        auto temp = std::get<config::BitString>(col_value); 
        std::cout << col_name << ": "; 
        for(const auto& te: temp){ 
            std::cout << te; 
        } 
        std::cout << col_name << ", "; 
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
    memdb::Database& db = memdb::Database::getInstance(); 
 
    std::string create_table_query = "create table users ({key, autoincrement} id : int32, {unique} login: string[8], password_hash: bytes[8], is_admin: bool = false, age: int32, mom: string[32], is_parent: bool)"; 
    db.execute(create_table_query); 
    db.execute(R"(insert (id = 1, login = "Alice", password_hash = 0x1111111111111111, is_admin = true, age = 25, mom = "Eve") to users)"); 
    db.execute(R"(insert (id = 2, login = "Bob", age = 30, mom = "Martha", is_parent = true) to users)"); 
    db.execute(R"(insert (id = 3, login = "Carol", password_hash = 0x3333333333333333, is_admin = false, age = 22, is_parent = false) to users)"); 
    db.execute(R"(insert (id = 4, login = "Dave", password_hash = 0x4444444444444444, is_admin = true, age = 15, mom = "Sara", is_parent = true) to users)"); 
    db.execute(R"(insert (id = 5, login = "Eve", password_hash = 0x5555555555555555, is_admin = false, mom = "Nancy", is_parent = false) to users)"); 
    db.execute(R"(insert (id = 6, login = "Frank", is_admin = true, age = 40) to users)"); 
    db.execute(R"(insert (id = 7, login = "Grace", password_hash = 0x7777777777777777, is_admin = false, age = 29, mom = "Judy", is_parent = true) to users)"); 
    db.execute(R"(insert (id = 8, password_hash = 0x8888888888888888, is_admin = true, mom = "Eve", is_parent = false) to users)"); 
    db.execute(R"(insert (id = 9, login = "Ivy", password_hash = 0x9999999999999999, is_admin = false, age = 23, mom = "Helen", is_parent = true) to users)"); 
    db.execute(R"(insert (id = 10, login = "Jack", password_hash = 0xAAAAAAAAAAAAAAAA, is_admin = true, age = 31, mom = "Diane") to users)"); 
 
//std::string select_query_2 = "select id, login, mom, is_parent, is_admin from users where id % 2 = 1 && ( | login | < 7 || | mom | < 0 ) && ( is_admin = true )"; 
//std::string select_query_2 = "select id, login, mom, is_parent, is_admin from users where "; 
 
    std::string check_query = "select id, login, mom, is_parent, is_admin from users where true"; 
    auto check_response = db.execute(check_query); 
    const auto &check_data = check_response->getData(); 
    PrintData(check_data, check_query); 
 
 
    std::string select_query = "select login, id, is_parent from users where password_hash = 0x1234 && age = 5 && ( ( mom <= \"alice\" && age > 10 ) || ( id < 5 && login >= \"vasya\" && is_parent = true ) ) && is_admin = false"; 
    auto select_response = db.execute(select_query); 
    const auto &select_data = select_response->getData(); 
    PrintData(select_data, select_query); 
}



/*
TEST_F(DatabaseTest, SelectData) {
    std::string create_table_query = "create table users ({key, autoincrement} id : int32, {unique} login: string[32], password_hash: bytes[8], is_admin: bool = false, age: int32, mom: string[32], is_parent: bool)";
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
    std::string select_query_2 = "select id, login, mom, is_parent, is_admin from users where ";
    auto select_response_2 = db.execute(select_query_2);
    const auto &data_2 = select_response_2->getData();

    std::vector<int> ans;
    for (const auto &[key, row]: data_2) {
        int id;
        std::string login;
        std::string mom;
        bool is_admin;
        bool is_parent;

        if (std::holds_alternative<int>(row.at("id"))) {
            id = std::get<int>(row.at("id"));
        } else {
            id = -1000;
        }
        if (std::holds_alternative<std::string>(row.at("login"))) {
            login = std::get<std::string>(row.at("login"));
        } else {
            login = "Non";
        }
        if (std::holds_alternative<std::string>(row.at("mom"))) {
            mom = std::get<std::string>(row.at("mom"));
        } else {
            mom = "Non";
        }
        if (std::holds_alternative<bool>(row.at("is_admin"))) {
            is_admin = std::get<bool>(row.at("is_admin"));
        } else {
            is_admin = false;
        }
        if (std::holds_alternative<bool>(row.at("is_parent"))) {
            is_parent = std::get<bool>(row.at("is_parent"));
        } else {
            is_parent = false;
        }
        ans.push_back(id);
    }
    std::sort(ans.begin(),ans.end());
    std::vector<int> temp = {3,6,7,8,9};
    EXPECT_EQ(ans, temp);
} */

TEST_F(DatabaseTest, INSERT_implicit) {
    std::string create_table_query = "create table technique ({unique} cars : string[32], {unique, autoincrement} cars_id : int32, {unique} phone_id : int32, {unique} phone : string[32])";
    auto res = db.execute(create_table_query);
    EXPECT_TRUE(res->getStatus());

    std::string insert_query = R"(insert (cars = "Lada", phone = "vivo") to technique)";
    auto res1 = db.execute(insert_query);
    EXPECT_TRUE(res1->getStatus());

    std::string insert_query1 = R"(insert (, 5, "IPhone") to technique)";
    auto res2 = db.execute(insert_query1);
    EXPECT_TRUE(res2->getStatus());

    auto table = db.getTable("technique");
    auto data_after = table->getData();
    for (const auto& [key, row] : data_after) {
        auto id_opt = utils::get<std::string>(row, "cars"); 
        auto id = id_opt.has_value() ? id_opt.value() : "N/A"; 
 
        auto login_opt = utils::get<int>(row, "cars_id");
        int login = login_opt.has_value() ? login_opt.value() : -1; 
 
        auto is_parent_opt = utils::get<int>(row, "phone_id"); 
        int is_parent = is_parent_opt.has_value() ? is_parent_opt.value() : -1;

        auto is_code = utils::get<std::string>(row, "phone");
        auto code = is_code.has_value() ? is_code.value() : "N/A";
 
        std::cout << "cars: " << id 
                  << ", id_cars: " << login 
                  << ", phone_id: " << is_parent
                  << ", phone " << code
                  << std::endl;
    }

    std::cout << std::endl;
}

TEST_F(DatabaseTest, SELECT) {
    std::string create_table_query = "create tABle table_name ( {  } col1 : int32, col2 : bytes[2])";
    auto res = db.execute(create_table_query);

    EXPECT_TRUE(res  -> getStatus());

    auto res1 = db.execute("iNsErt (col1= 123, col2 =0x0456) tO table_name");

    EXPECT_TRUE(res1->getStatus());

    std::string select_query = "Select col1, col2 from table_name where col1 = 123";
    auto res2 = db.execute(select_query);

    EXPECT_TRUE(res2 -> getStatus());

    auto new_table = res2->getData();
    for (auto& [key, rows] : new_table) {
        for (auto& [column_name, row] : rows) {
            if (std::get_if<int>(&row)) {
                std::cout << column_name << " " << std::get<int>(row) << std::endl;
            } else {
                auto vec = std::get<std::vector<uint8_t>>(row);

                std::cout << column_name << " ";
                for (auto& it : vec) {
                    std::cout << it;
                }
            }
            std::cout << " ";
        }
    }

    std::cout << std::endl;
}

TEST_F(DatabaseTest, UPDATE) {
    std::string create_table_query = "create table room ({key, autoincrement} id : int32, occupants : string[5], beds : int32 = 3, beds_code : bytes[2] = 0x12)";
    auto res = db.execute(create_table_query);

    
}

TEST_F(DatabaseTest, CREATEINDEXS) {
    std::string create_table_query = "create table medicine ({key, autoincrement} id : int32, doctors : string[32], equipment : bool)";
    auto res = db.execute(create_table_query);

    EXPECT_TRUE(res->getStatus());

    std::string create_index_query = "create ordered index on medicine by id, doctors";
    auto res1 = db.execute(create_index_query);

    EXPECT_TRUE(res1 -> getStatus());

    
}


TEST_F(DatabaseTest, DELETE) {
    std::string create_table_query = "create tABle family ({unique} id : int32, name : string[32], isParent : bool, code : bytes[3])";

    auto res = db.execute(create_table_query);
    EXPECT_TRUE(res->getStatus());

    auto res1 = db.execute(R"(insert (id = 1, name = "Anna", isParent = false, code = 0x0ffee3) to family)");
    auto res2 = db.execute(R"(insert (id = 2, name = "Ivan", isParent = true, code = 0x0e572f) to family)");

    EXPECT_TRUE(res1->getStatus());
    EXPECT_TRUE(res2->getStatus());

    std::string delete_query = R"(DELETE family where code = 0x0ffee3)";
    auto res3 = db.execute(delete_query);
    EXPECT_TRUE(res3->getStatus());

    auto table = db.getTable("family");
    auto data_after = table->getData();

    for (const auto& [key, row] : data_after) { 
        auto id_opt = utils::get<int>(row, "id"); 
        int id = id_opt.has_value() ? id_opt.value() : -1; 
 
        auto login_opt = utils::get<std::string>(row, "name"); 
        std::string login = login_opt.has_value() ? login_opt.value() : "N/A"; 
 
        auto is_parent_opt = utils::get<bool>(row, "is_parent"); 
        bool is_parent = is_parent_opt.has_value() ? is_parent_opt.value() : false;
        auto is_code = utils::get<std::vector<uint8_t>>(row, "code");
        std::vector<uint8_t> code = is_code.has_value() ? is_code.value() : std::vector<uint8_t>{0};
 
        std::cout << "id: " << id 
                  << ", name: " << login 
                  << ", is_parent: " << (is_parent ? "true" : "false") 
                  << " code: ";
        for (auto& element_vector : code) {
            std::cout << element_vector << " ";
        }
        std::cout << std::endl;
    }
}