#include <gtest/gtest.h>
#include "Database.h"

class DatabaseTest : public ::testing::Test {
protected:
    memdb::Database& db = memdb::Database::getInstance();
};

TEST_F(DatabaseTest, SelectData) {
    std::string create_table_query = "create table users ({key, autoincrement} id : int32, {unique} login: string[32], password_hash: bytes[8], is_admin: bool = false, age: int32, mom: string[32], is_parent: bool)";
    db.execute(create_table_query);
    db.execute(R"(insert (login = "Alice", password_hash = 0x1111111111111111, is_admin = true, age = 25, mom = "Eve") to users)");
    db.execute(R"(insert (login = "Bob" age = 30, mom = "Martha", is_parent = true) to users)");
    db.execute(R"(insert (login = "Carol", password_hash = 0x3333333333333333, is_admin = false, age = 22, is_parent = false) to users)");
    db.execute(R"(insert (login = "Dave", password_hash = 0x4444444444444444, is_admin = true, age = 35, mom = "Sara", is_parent = true) to users)");
    db.execute(R"(insert (login = "Eve", password_hash = 0x5555555555555555, is_admin = false, mom = "Nancy", is_parent = false) to users)");
    db.execute(R"(insert (login = "Frank", is_admin = true, age = 40) to users)");
    db.execute(R"(insert (login = "Grace", password_hash = 0x7777777777777777, is_admin = false, age = 29, mom = "Judy", is_parent = true) to users)");
    db.execute(R"(insert (password_hash = 0x8888888888888888, is_admin = true, mom = "Eve", is_parent = false) to users)");
    db.execute(R"(insert (login = "Ivy", password_hash = 0x9999999999999999, is_admin = false, age = 23, mom = "Helen", is_parent = true) to users)");
    db.execute(R"(insert (login = "Jack", password_hash = 0xAAAAAAAAAAAAAAAA, is_admin = true, age = 31, mom = "Diane") to users)");

    // Первый select запрос с условием `is_admin || id < 10`
    std::string select_query_1 = "select id, login from users where is_admin || id < 10";
    auto select_response_1 = db.execute(select_query_1);

    EXPECT_TRUE(select_response_1->getStatus());
    const auto& data_1 = select_response_1->getData();

    // Проверка, что результат содержит записи только для пользователей с is_admin = true или id < 10
    for (const auto& [key, row] : data_1) {
        int id = std::get<int>(row.at("id"));
        bool is_admin = std::get<bool>(row.at("is_admin"));
        EXPECT_TRUE(is_admin || id < 10);
    }

    // Второй select запрос с более сложным условием
    std::string select_query_2 = "select id, login from users where id % 2 = 1 && ( | login | < 5 || | mom | < 5 ) ^ ( is_admin > is_parent )";
    auto select_response_2 = db.execute(select_query_2);

    EXPECT_TRUE(select_response_2->getStatus());
    const auto& data_2 = select_response_2->getData();

    // Проверка, что результат соответствует сложному условию
    for (const auto& [key, row] : data_2) {
        int id = std::get<int>(row.at("id"));
        std::string login = std::get<std::string>(row.at("login"));
        std::string mom = std::get<std::string>(row.at("mom"));
        bool is_admin = std::get<bool>(row.at("is_admin"));
        bool is_parent = std::get<bool>(row.at("is_parent"));

        bool condition = (id % 2 == 1) && ((login.size() < 5 || mom.size() < 5) ^ (is_admin > is_parent));
        EXPECT_TRUE(condition);
    }
}