#include "Database.h"
#include <iostream>

int main() {
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