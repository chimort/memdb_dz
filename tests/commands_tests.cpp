#include <gtest/gtest.h>
#include "Database.h"

class DatabaseTest : public ::testing::Test {
protected:
    memdb::Database& db = memdb::Database::getInstance();
};

TEST_F(DatabaseTest, CreateTable) {
    std::string create_table_query = "create table users ({key, autoincrement} id : int32, {unique} login: string[32], password_hash: bytes[8], is_admin: bool = false)";
    auto response = db.execute(create_table_query);

    EXPECT_TRUE(response->getStatus());
    EXPECT_EQ(response->getMessage(), "Table created successfully");

    auto table = db.getTable("users");
    EXPECT_NE(table, nullptr);
}

TEST_F(DatabaseTest, InsertRecords) {
    std::string create_table_query = "create table users ({key, autoincrement} id : int32, {unique} login: string[32], password_hash: bytes[8], is_admin: bool = false)";
    db.execute(create_table_query);

    std::string insert_query1 = R"(insert (id = 1, login = "Alice", password_hash = 0xdeadbeefdeadbeef) to users)";
    auto insert_response1 = db.execute(insert_query1);
    EXPECT_TRUE(insert_response1->getStatus());

    std::string insert_query2 = R"(insert (id = 2, login = "Bob", password_hash = 0xdeadbeefdeadbeef) to users)";
    auto insert_response2 = db.execute(insert_query2);
    EXPECT_TRUE(insert_response2->getStatus());
}

TEST_F(DatabaseTest, SelectData) {
    std::string create_table_query = "create table users ({key, autoincrement} id : int32, {unique} login: string[32], password_hash: bytes[8], is_admin: bool = false)";
    db.execute(create_table_query);

    db.execute(R"(insert (id = 1, login = "Alice", password_hash = 0xdeadbeefdeadbeef) to users)");
    db.execute(R"(insert (id = 2, login = "Bob", password_hash = 0xdeadbeefdeadbeef) to users)");

    std::string select_query = "select id, login from users where id = 2";
    auto select_response = db.execute(select_query);

    EXPECT_TRUE(select_response->getStatus());

    const auto& data = select_response->getData();
    ASSERT_EQ(data.size(), 1);
    //auto row1 = data.at(1);
    //EXPECT_EQ(std::get<int>(row1["id"]), 1);
   // EXPECT_EQ(std::get<std::string>(row1["login"]), R"("Alice")");
    auto row2 = data.at(0);
    EXPECT_EQ(std::get<std::string>(row2["login"]), R"("Bob")");
}
