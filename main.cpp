#include "Database.h"
#include "utils.h"
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

    std::cout << "Содержимое таблицы до удаления:" << std::endl;

    auto select_response_before = db.execute("select id, login, mom, is_parent, is_admin from users where true");
    if (!select_response_before->getStatus()) {
        std::cout << "sdgdsg" << std::endl;
    }
    const auto& data_before = select_response_before->getData();

    for (const auto& [key, row] : data_before) {
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

    auto delete_response = db.execute("delEte users where false = true");
    if (!delete_response->getStatus()) {
        std::cerr << "Ошибка при удалении: " << delete_response->getMessage() << std::endl;
    }

    std::cout << "\nСодержимое таблицы после удаления:" << std::endl;

    auto select_response_after = db.execute("select id, login, mom, is_parent, is_admin from users where true");
    const auto& data_after = select_response_after->getData();

    for (const auto& [key, row] : data_after) {
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

    auto dr = db.execute("delete users where is_admin = true");
    if (!dr->getStatus()) {
        std::cerr << "Ошибка при удалении: " << dr->getMessage() << std::endl;
    }

    auto s1 = db.execute("select id, login, mom, is_parent, is_admin from users where true");
    const auto& d1 = s1->getData();
    std::cout << std::endl;
    for (const auto& [key, row] : d1) {
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