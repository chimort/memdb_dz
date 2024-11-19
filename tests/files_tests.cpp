#include <gtest/gtest.h>
#include "Table.h"

#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>

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

TEST_F(TableSaveLoadTest, SaveAndLoadTableDataIntegrity) {
    // Шаг 1: Создаем таблицу с определенной схемой
    std::vector<config::ColumnSchema> columns = {
        {"id", config::ColumnType::INT, 0},
        {"name", config::ColumnType::STRING, 255},
        {"age", config::ColumnType::INT, 0},
        {"active", config::ColumnType::BOOL, 0},
        {"data", config::ColumnType::BITSTRING, 4}
    };
    memdb::Table original_table(columns);

    // Шаг 2: Вставляем данные в таблицу
    std::unordered_map<std::string, std::string> record1 = {
        {"id", "1"},
        {"name", "Alice"},
        {"age", "30"},
        {"active", "true"},
        {"data", "0xDEADBEEF"}
    };
    EXPECT_TRUE(original_table.insertRecord(record1));

    std::unordered_map<std::string, std::string> record2 = {
        {"id", "2"},
        {"name", "Bob, the \"Builder\"\nNew Line"},
        {"age", "25"},
        {"active", "false"},
        {"data", "0xBAADF00D"}
    };
    EXPECT_TRUE(original_table.insertRecord(record2));

    // Шаг 3: Сохраняем таблицу в CSV-файл
    std::ofstream ofs(test_file_path);
    ASSERT_TRUE(ofs.is_open());
    EXPECT_TRUE(original_table.saveToCSV(ofs));
    ofs.close();

    // Шаг 4: Загружаем данные из CSV-файла в новую таблицу
    memdb::Table loaded_table(columns);
    std::ifstream ifs(test_file_path);
    ASSERT_TRUE(ifs.is_open());
    EXPECT_TRUE(loaded_table.loadFromCSV(ifs));
    ifs.close();

    // Шаг 5: Сравниваем содержимое data_ исходной и загруженной таблиц
    const auto& original_data = original_table.getData();
    const auto& loaded_data = loaded_table.getData();

    EXPECT_EQ(original_data.size(), loaded_data.size());

    for (const auto& [id, original_row] : original_data) {
        auto it = loaded_data.find(id);
        ASSERT_NE(it, loaded_data.end()) << "Запись с id = " << id << " не найдена в загруженной таблице.";
        const auto& loaded_row = it->second;

        // Сравниваем значения в строках
        for (const auto& column : columns) {
            const auto& column_name = column.name;

            const auto& original_value = original_row.at(column_name);
            const auto& loaded_value = loaded_row.at(column_name);

            EXPECT_EQ(original_value.index(), loaded_value.index()) << "Несовпадение типов в колонке '" << column_name << "' для id = " << id;

            // Сравниваем значения в зависимости от типа
            std::visit([&](const auto& original_val) {
                using T = std::decay_t<decltype(original_val)>;
                const T* loaded_val = std::get_if<T>(&loaded_value);
                ASSERT_NE(loaded_val, nullptr) << "Тип значения в колонке '" << column_name << "' не соответствует ожидаемому для id = " << id;
                EXPECT_EQ(original_val, *loaded_val) << "Несовпадение значений в колонке '" << column_name << "' для id = " << id;
            }, original_value);
        }
    }
}


TEST_F(TableSaveLoadTest, SaveAndLoadTableWithVariousDataTypes) {
    // Создаем таблицу с разными типами колонок
    std::vector<config::ColumnSchema> columns = {
        {"id", config::ColumnType::INT, 0},
        {"description", config::ColumnType::STRING, 50},
        {"is_valid", config::ColumnType::BOOL, 0},
        {"raw_data", config::ColumnType::BITSTRING, 8}
    };
    memdb::Table original_table(columns);

    // Вставляем записи с разными данными
    std::unordered_map<std::string, std::string> record1 = {
        {"id", "100"},
        {"description", "Test record"},
        {"is_valid", "true"},
        {"raw_data", "0x1234567890ABCDEF"}
    };
    EXPECT_TRUE(original_table.insertRecord(record1));

    std::unordered_map<std::string, std::string> record2 = {
        {"id", "101"},
        {"description", "Another record with a long description exceeding max size"},
        {"is_valid", "false"},
        {"raw_data", "0xFEDCBA0987654321"}
    };
    // Ожидаем, что вставка не удалась из-за превышения max_size для description
    EXPECT_FALSE(original_table.insertRecord(record2));

    // Сохраняем таблицу в CSV-файл
    std::ofstream ofs(test_file_path);
    ASSERT_TRUE(ofs.is_open());
    EXPECT_TRUE(original_table.saveToCSV(ofs));
    ofs.close();

    // Загружаем данные из CSV-файла в новую таблицу
    memdb::Table loaded_table(columns);
    std::ifstream ifs(test_file_path);
    ASSERT_TRUE(ifs.is_open());
    EXPECT_TRUE(loaded_table.loadFromCSV(ifs));
    ifs.close();

    // Сравниваем содержимое data_
    const auto& original_data = original_table.getData();
    const auto& loaded_data = loaded_table.getData();

    EXPECT_EQ(original_data.size(), loaded_data.size());

    // Сравниваем записи аналогично первому тесту
    for (const auto& [id, original_row] : original_data) {
        auto it = loaded_data.find(id);
        ASSERT_NE(it, loaded_data.end()) << "Запись с id = " << id << " не найдена в загруженной таблице.";
        const auto& loaded_row = it->second;

        for (const auto& column : columns) {
            const auto& column_name = column.name;

            const auto& original_value = original_row.at(column_name);
            const auto& loaded_value = loaded_row.at(column_name);

            EXPECT_EQ(original_value.index(), loaded_value.index()) << "Несовпадение типов в колонке '" << column_name << "' для id = " << id;

            std::visit([&](const auto& original_val) {
                using T = std::decay_t<decltype(original_val)>;
                const T* loaded_val = std::get_if<T>(&loaded_value);
                ASSERT_NE(loaded_val, nullptr) << "Тип значения в колонке '" << column_name << "' не соответствует ожидаемому для id = " << id;
                EXPECT_EQ(original_val, *loaded_val) << "Несовпадение значений в колонке '" << column_name << "' для id = " << id;
            }, original_value);
        }
    }
}


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
    std::vector<config::ColumnSchema> columns = {
        {"id", config::ColumnType::INT, 0, {1, 1, 1}}, // unique, autoincrement, key
        {"name", config::ColumnType::STRING, 255, {0, 0, 0}},
        {"age", config::ColumnType::INT, 0, {0, 0, 0}},
        {"active", config::ColumnType::BOOL, 0, {0, 0, 0}}
    };
    memdb::Table table(columns);

    // Вставляем запись без указания 'id', 'age' и 'active'
    std::unordered_map<std::string, std::string> insert_values = {
        {"name", "Charlie"}
    };

    bool result = table.insertRecord(insert_values);
    EXPECT_TRUE(result);

    // Проверяем, что запись добавлена с автоприращенным 'id'
    EXPECT_EQ(table.getData().size(), 1);
    const auto& data = table.getData();
    const auto& row = data.begin()->second;

    int id = std::get<int>(row.at("id"));
    std::string name = std::get<std::string>(row.at("name"));
    int age = std::get<int>(row.at("age"));
    bool active = std::get<bool>(row.at("active"));

    EXPECT_EQ(name, "Charlie");
    EXPECT_EQ(age, 0);      // Значение по умолчанию
    EXPECT_EQ(active, false); // Значение по умолчанию
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

TEST_F(TableCSVInsertTest, LoadCSVAndInsertRecord) {
    // Шаг 1: Определяем схему таблицы
    std::vector<config::ColumnSchema> columns = {
        // Атрибуты: {unique, autoincrement, key}
        {"id", config::ColumnType::INT, 0, {1, 1, 1}},
        {"name", config::ColumnType::STRING, 50, {0, 0, 0}},
        {"age", config::ColumnType::INT, 0, {0, 0, 0}},
        {"active", config::ColumnType::BOOL, 0, {0, 0, 0}}
    };
    memdb::Table table(columns);

    // Шаг 2: Загружаем CSV-файл в таблицу
    std::ifstream ifs(test_file_path);
    ASSERT_TRUE(ifs.is_open());
    std::cout << "[LoadCSVAndInsertRecord] Загрузка данных из CSV-файла\n";
    bool load_result = table.loadFromCSV(ifs);
    EXPECT_TRUE(load_result);
    ifs.close();

    if (load_result) {
        std::cout << "[LoadCSVAndInsertRecord] Данные успешно загружены\n";
    } else {
        std::cout << "[LoadCSVAndInsertRecord] Не удалось загрузить данные из CSV\n";
    }

    // Шаг 3: Вставляем новую запись без указания 'id'
    std::unordered_map<std::string, std::string> new_record = {
        {"name", "Charlie"},
        {"age", "28"},
        {"active", "true"}
    };
    bool insert_result = table.insertRecord(new_record);
    EXPECT_TRUE(insert_result);

    // Проверяем, что новая запись добавлена с корректным 'id'
    const auto& data = table.getData();
    EXPECT_EQ(data.size(), 3);

    // Выводим содержимое таблицы для визуализации
    std::cout << "Содержимое таблицы после загрузки и вставки новой записи:" << std::endl;
    for (const auto& [record_id, row] : data) {
        int id = std::get<int>(row.at("id"));
        std::string name = std::get<std::string>(row.at("name"));
        int age = std::get<int>(row.at("age"));
        bool active = std::get<bool>(row.at("active"));
        std::cout << "ID: " << id << ", Name: " << name << ", Age: " << age << ", Active: " << (active ? "true" : "false") << std::endl;
    }

    // Проверяем, что 'id' новой записи корректно автоприращен
    // Ожидаем, что новый 'id' равен 3 (если данные в CSV имели 'id' 1 и 2)
    bool id3_found = false;
    for (const auto& [record_id, row] : data) {
        int id = std::get<int>(row.at("id"));
        if (id == 3) {
            id3_found = true;
            EXPECT_EQ(std::get<std::string>(row.at("name")), "Charlie");
            break;
        }
    }
    EXPECT_TRUE(id3_found);
}

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