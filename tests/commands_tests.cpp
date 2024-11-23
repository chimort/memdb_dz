#include <gtest/gtest.h>
#include "Database.h"

class DatabaseTest : public ::testing::Test {
protected:
    memdb::Database& db = memdb::Database::getInstance();
};

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

TEST_F(DatabaseTest, SELECT){
    std::string create_table_query = "create tABle table_name ( {} col1 : int32 = 31, col2: bytes[2] = 0x1234)";
    auto res1 = db.execute(create_table_query);

    EXPECT_FALSE(res1  -> getStatus());

    auto res = db.execute("iNsErt (col1= 123, col2 =0x0456) tO table_name");

    EXPECT_TRUE(res->getStatus());
    /*
    //
    "seLeCt col1,col2 "
    "fRom table_name "
    "whERe col1 < 23 && (true || false);"
    //
    "UPDATe table_name "
    "sEt col1 =321 "
    "WHeRE col2 < 0x0500;"
    //
    "DELETE table_name "
    "where col = 321 and col2 = 0x0456;"*/
}