#include <gtest/gtest.h>
#include "Database.h"

class WhereTests : public ::testing::Test{
};

TEST_F(WhereTests, Condition) {
    memdb::Database& db = memdb::Database::getInstance();
    auto response = db.execute("cReAtE tAbLe users ({key, autoincrement} id : int32, {unique} login: string[32], password_hash: bytes[8], is_admin: bool = false");
    EXPECT_TRUE(response->getStatus());
    db.execute("InSeRt (\n"
               "is_admin = false,\n"
               "login = \"vasya\",\n"
               "password_hash = 0x0000000000000001\n"
               ") to users");
    db.execute("InSeRt (\n"
               "is_admin = true,\n"
               "login = \"admin\",\n"
               "password_hash = 0x0000000000000000\n"
               ") to users");
    db.execute("InSeRt (\n"
               "is_admin = false,\n"
               "login = \"petya\",\n"
               ") to users");
    auto response_sel = db.execute("select login, is_admin from users wHerE password_hash > 0x0000000000000000");
    bool f = true;
    const auto& data = response->getData();
    for (const auto& [key, row] : data) {
        auto login = utils::get<std::string>(row, "login").value();
        auto is_admin = utils::get<bool>(row, "is_admin").value();

        std::cout << "login: " << login << ", is_admin: \n" << is_admin;
    }
}