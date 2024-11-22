#include "Database.h"
#include <iostream>

int main() {
    memdb::Database &db = memdb::Database::getInstance();

    std::string create_table_query = "create table users ({key, autoincrement} id : int32, {unique} login: string[32], password_hash: bytes[8], is_admin: bool = false)";
    db.execute(create_table_query);

    db.execute(R"(insert (login = "Alice", password_hash = 0xdeadbeefdeadbeef) to users)");
    db.execute(R"(insert (login = "Bob", password_hash = 0xdeadbeefdeadbeef) to users)");

    std::string select_query = "select id, login from users where id = 2";
    auto select_response = db.execute(select_query);

    std::cout << "status: "<< select_response->getStatus() << std::endl;

    const auto &data = select_response->getData();
    std::cout << "size: " << data.size() << std::endl ;

    auto row2 = data.at(0);
    std::cout << "login: " << std::get<std::string>(row2["login"]) << std::endl;
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

    std::string insert_query1 = "insert (id = 1, name = 'Alice', age = 30) to my_table";
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
        auto id = utils::get<int>(row, "id").value();
        auto name = utils::get<std::string>(row, "name").value();
        auto age = utils::get<int>(row, "age").value();

        std::cout << "ID: " << id << ", Name: " << name << ", Age: " << age << std::endl;
    }

    return 0;
}
*/