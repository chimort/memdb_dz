#include "Database.h"
#include <iostream>

int main() {

    memdb::Database &db = memdb::Database::getInstance();
    std::string create_table_query = "create table users ({key, autoincrement} id : int32, {unique} login: string[32], password_hash: bytes[8], is_admin: bool = false, age: int32, mom: string[32], is_parent: bool)";
    db.execute(create_table_query);
    db.execute(R"(insert (login = "Alice", password_hash = 0x1111111111111111, is_admin = true, age = 25, mom = "Eve") to users)");
    db.execute(R"(insert (login = "Bob", age = 30, mom = "Martha", is_parent = true) to users)");
    db.execute(R"(insert (login = "Carol", password_hash = 0x3333333333333333, is_admin = false, age = 22, is_parent = false) to users)");
    db.execute(R"(insert (login = "Dave", password_hash = 0x4444444444444444, is_admin = true, age = 35, mom = "Sara", is_parent = true) to users)");
    db.execute(R"(insert (login = "Eve", password_hash = 0x5555555555555555, is_admin = false, mom = "Nancy", is_parent = false) to users)");
    db.execute(R"(insert (login = "Frank", is_admin = true, age = 40) to users)");
    db.execute(R"(insert (login = "Grace", password_hash = 0x7777777777777777, is_admin = false, age = 29, mom = "Judy", is_parent = true) to users)");
    db.execute(R"(insert (password_hash = 0x8888888888888888, is_admin = true, mom = "Eve", is_parent = false) to users)");
    db.execute(R"(insert (login = "Ivy", password_hash = 0x9999999999999999, is_admin = false, age = 23, mom = "Helen", is_parent = true) to users)");
    db.execute(R"(insert (login = "Jack", password_hash = 0xAAAAAAAAAAAAAAAA, is_admin = true, age = 31, mom = "Diane") to users)");
    //std::string select_query_2 = "select id, login, mom, is_parent, is_admin from users where id % 2 = 1 && ( | login | < 7 || | mom | < 0 ) && ( is_admin = true )";
    auto res = db.execute("delete users where is_admin = true");
    if (!res) {
        std::cout << "not working" << std::endl;
    }

    auto res2 = db.execute("delete users where is_admin = true");
    if (!res2) {
        std::cout << "not working2" << std::endl;
    }
    
    std::string select_query_2 = "select id, login, mom, is_parent, is_admin from users where true = \ntrue";
    auto select_response_2 = db.execute(select_query_2);
    const auto &data_2 = select_response_2->getData();

    std::cout << data_2.size() << std::endl;

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
    }
    return 0;
}
/*
memdb::Database& db = memdb::Database::getInstance();

std::string create_table_query = "create table my_table (id : int32, name : string[32], age : int32)";
auto create_response = db.execute(create_table_query);
if (!create_response->getStatus()) {
    std::cerr << "Ошибка создания таблицы: " << create_response->getMessage() << std::endl;
    return 1;
}

std::string insert_query1 = "insert (, 'Alice', 30) to my_table";
auto insert_response1 = db.execute(insert_query1);
if (!insert_response1->getStatus()) {
    std::cerr << "Ошибка вставки данных: " << insert_response1->getMessage() << std::endl;
    return 1;
}

std::string insert_query2 = "insert (id = 2, name = 'Bob', age = 25) to my_table";
auto insert_response2 = db.execute(insert_query2);
if (!insert_response2->getStatus()) {
    std::cerr << "Ошибка вставки данных: " << insert_response2->getMessage() << std::endl;
    return 1;
}

std::string insert_query3 = "insert (id = 3, name = 'Charlie', age = 35) to my_table";
auto insert_response3 = db.execute(insert_query3);
if (!insert_response3->getStatus()) {
    std::cerr << "Ошибка вставки данных: " << insert_response3->getMessage() << std::endl;
    return 1;
}

auto table = db.getTable("my_table");
if (!table) {
    std::cerr << "Таблица 'my_table' не найдена." << std::endl;
    return 1;
}

const auto& data = table->getData();

for (const auto& [key, row] : data) {
    auto id_optional = utils::get<int>(row, "id");
    if (id_optional.has_value()){
        auto id = id_optional.value();
        std::cout << "ID: " << id << " ";
    } else {
        std::cout << "ID: NULL ";
    }
    auto name = utils::get<std::string>(row, "name").value();
    auto age = utils::get<int>(row, "age").value();

    std::cout << "Name: " << name << ", Age: " << age << std::endl;
}

return 0;
}
*/