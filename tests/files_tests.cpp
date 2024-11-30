#include <gtest/gtest.h>
#include "Table.h"
#include "utils.h"

#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <variant>
#include <optional>

class TableSaveLoadTest : public ::testing::Test {
protected:
    std::string test_file_path = "test_table.csv";

    void TearDown() override {
        // Удаляем тестовый файл после каждого теста
        if (std::filesystem::exists(test_file_path)) {
            std::filesystem::remove(test_file_path);
        }
    }
};


class TableInsertMissingValuesTest : public ::testing::Test {
protected:
    std::string test_file_path = "test_table_missing_values.csv";

    void TearDown() override {
        if (std::filesystem::exists(test_file_path)) {
            std::filesystem::remove(test_file_path);
        }
    }
};

TEST_F(TableInsertMissingValuesTest, InsertRecordWithMissingValues) {
    // Определяем схему таблицы с колонками разных типов
    std::vector<config::ColumnSchema> columns = {
        {"id", config::ColumnType::INT, 0},
        {"name", config::ColumnType::STRING, 255},
        {"age", config::ColumnType::INT, 0},
        {"active", config::ColumnType::BOOL, 0}
    };
    memdb::Table table(columns);

    // Вставляем запись с отсутствующими значениями для 'age' и 'active'
    std::unordered_map<std::string, std::string> insert_values = {
        {"id", "3"},
        {"name", "\"Charlie\""}
        // Поля 'age' и 'active' отсутствуют и должны получить значение NULL
    };

    bool result = table.insertRecord(insert_values);
    EXPECT_TRUE(result);

    std::cout << "[InsertRecordWithMissingValues] Результат вставки записи с отсутствующими значениями: "
              << (result ? "успешно" : "неудачно") << std::endl;

    const auto& data = table.getData();
    bool record_found = false;
    const config::RowType* found_row = nullptr;

    // Ищем запись с id = 3
    for (const auto& [record_id, row] : data) {
        auto id_it = row.find("id");
        if (id_it != row.end()) {
            const auto& id_value = id_it->second;
            if (std::holds_alternative<int>(id_value) && std::get<int>(id_value) == 3) {
                record_found = true;
                found_row = &row;
                break;
            }
        }
    }

    EXPECT_TRUE(record_found);

    if (record_found && found_row != nullptr) {
        std::cout << "[InsertRecordWithMissingValues] Найдена запись с id = 3:" << std::endl;

        // Проверяем 'name'
        auto name_opt = utils::get<std::string>(*found_row, "name");
        EXPECT_TRUE(name_opt.has_value());
        EXPECT_EQ(name_opt.value(), "Charlie");
        std::cout << "  name: " << name_opt.value() << std::endl;

        // Проверяем 'age' (ожидаем NULL)
        auto age_opt = utils::get<int>(*found_row, "age");
        EXPECT_FALSE(age_opt.has_value());
        std::cout << "  age: NULL" << std::endl;

        // Проверяем 'active' (ожидаем NULL)
        auto active_opt = utils::get<bool>(*found_row, "active");
        EXPECT_FALSE(active_opt.has_value());
        std::cout << "  active: NULL" << std::endl;
    } else {
        std::cout << "[InsertRecordWithMissingValues] Запись с id = 3 не найдена." << std::endl;
    }
}

class TableCSVInsertTest : public ::testing::Test {
protected:
    std::string test_file_path = "test_table_insert.csv";

    void SetUp() override {
        // Подготавливаем CSV-файл с начальными данными
        std::string csv_data =
            "id,name,age,active\n"
            "1,Alice,30,true\n"
            "2,Bob,25,false\n";
        std::ofstream ofs(test_file_path);
        ASSERT_TRUE(ofs.is_open());
        ofs << csv_data;
        ofs.close();
    }

    void TearDown() override {
        // Удаляем тестовый CSV-файл после завершения теста
        if (std::filesystem::exists(test_file_path)) {
            std::filesystem::remove(test_file_path);
        }
    }
};

TEST_F(TableCSVInsertTest, InsertDuplicateIDRecord) {
    // (Предполагая тот же setup, что и ранее)
    std::vector<config::ColumnSchema> columns = {
        {"id", config::ColumnType::INT, 0, {1, 1, 1}},  // Добавлены атрибуты
        {"name", config::ColumnType::STRING, 50},
        {"age", config::ColumnType::INT, 0},
        {"active", config::ColumnType::BOOL, 0}
    };
    memdb::Table table(columns);
    
    // Загружаем данные CSV
    std::ifstream ifs(test_file_path);
    ASSERT_TRUE(ifs.is_open());
    bool load_result = table.loadFromCSV(ifs);
    EXPECT_TRUE(load_result);
    ifs.close();

    std::unordered_map<std::string, std::string> duplicate_record = {
        {"id", "1"},
        {"name", "Duplicate Alice"},
        {"age", "31"},
        {"active", "false"}
    };
    bool insert_result = table.insertRecord(duplicate_record);
    EXPECT_FALSE(insert_result);

    // Проверяем, что запись не добавилась
    EXPECT_EQ(table.getData().size(), 2);
}
