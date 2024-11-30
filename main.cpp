#include "Database.h"
#include "utils.h"
#include <iostream>

int main() {

    memdb::Database& db = memdb::Database::getInstance();

    // Создаем таблицу "users"
    std::string create_table_query = "create table medicine ({key, autoincrement} id : int32, doctors : string[32], equipment : bool, age : int32)";
    auto res = db.execute(create_table_query);

    std::string create_index_query = "create ordered index on medicine by age, doctors";
    auto res1 = db.execute(create_index_query);

    for (int i = 0; i <= 2; ++i) {
        std::string insert_query = R"(insert (doctors = "Strange)" + std::to_string(i) + R"(", age =)" + std::to_string(i) + R"() to medicine)";
        auto res2 = db.execute(insert_query);
    }

    std::string filename = "users_table.csv";
    std::string table_name = "medicine";
    if (db.saveToFile(filename, table_name)) {
        std::cout << "Таблица '" << table_name << "' успешно сохранена в файл '" << filename << "'.\n";
    } else {
        std::cerr << "Ошибка при сохранении таблицы '" << table_name << "' в файл '" << filename << "'.\n";
        return 1;
    }

    std::string new_table = "test";
    if (!db.loadFromFile(filename, new_table)) {
        std::cerr << "ОШибка" << std::endl;
    }

    auto test_table = db.getTable(new_table);
    if (!test_table) {
        std::cerr << "Таблица '" << new_table << "' не найдена.\n";
        return 0;
    }

    const auto& data = test_table->getData();
    for (const auto& [id, row] : data) {
        std::cout << "ID записи: " << id << "\n";

        auto name_opt = utils::get<std::string>(row, "doctors");
        std::cout << "name: " << (name_opt.has_value() ? name_opt.value() : "NULL") << "\n";

        auto description_opt = utils::get<std::string>(row, "equipment");
        std::cout << "equipment: " << (description_opt.has_value() ? description_opt.value() : "NULL") << "\n";

        auto age_opt = utils::get<int>(row, "age");
        std::cout << "age: " << (age_opt.has_value() ? std::to_string(age_opt.value()) : "NULL") << "\n";

        std::cout << "-------------------\n";
    }

    auto responce = db.execute("insert (doctors = \"best doctor\", age = 1488) to test");
    auto r = db.execute("insert (id = 0) to test");
    std::cout << r->getStatus();

    const auto& new_data = test_table->getData();

    for (const auto& [id, row] : new_data) {
        std::cout << "ID записи: " << (utils::get<int>(row, "id").has_value() ? std::to_string(utils::get<int>(row, "id").value()) : "NULL") << "\n";

        auto name_opt = utils::get<std::string>(row, "doctors");
        std::cout << "name: " << (name_opt.has_value() ? name_opt.value() : "NULL") << "\n";

        auto description_opt = utils::get<std::string>(row, "equipment");
        std::cout << "equipment: " << (description_opt.has_value() ? description_opt.value() : "NULL") << "\n";

        auto age_opt = utils::get<int>(row, "age");
        std::cout << "age: " << (age_opt.has_value() ? std::to_string(age_opt.value()) : "NULL") << "\n";

        std::cout << "-------------------\n";
    }

    std::string filename1 = "users_table2.csv";
    if (db.saveToFile(filename1, table_name)) {
        std::cout << "Таблица '" << table_name << "' успешно сохранена в файл '" << filename1 << "'.\n";
    } else {
        std::cerr << "Ошибка при сохранении таблицы '" << table_name << "' в файл '" << filename1 << "'.\n";
        return 1;
    }

    return 0;
}