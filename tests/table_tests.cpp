#include <gtest/gtest.h>
#include "Table.h"

#include <iostream>
#include <string>
#include <variant>

class TableTest : public ::testing::Test {
protected:
};

// Тест вставки с указанием названий колонок
TEST_F(TableTest, InsertWithColumnNames) {
    std::vector<config::ColumnSchema> columns = {
        {"id", config::ColumnType::INT, 0},
        {"name", config::ColumnType::STRING, 255},
        {"age", config::ColumnType::INT, 0},
        {"active", config::ColumnType::BOOL, 0}
    };
    memdb::Table table(columns);

    std::unordered_map<std::string, std::string> insert_values = {
        {"id", "1"},
        {"name", "Alice"},
        {"age", "30"},
        {"active", "true"}
    };

    bool result = table.insertRecord(insert_values);
    EXPECT_TRUE(result);

    std::cout << "[InsertWithColumnNames] Результат вставки записи с id = 1: " << (result ? "успешно" : "неудачно") << std::endl;

    const auto& data = table.getData();
    bool record_found = false;

    // Ищем запись с id = 1
    for (const auto& [record_id, row] : data) {
        auto id_it = row.find("id");
        if (id_it != row.end()) {
            const auto& id_value = id_it->second;
            if (std::holds_alternative<int>(id_value) && std::get<int>(id_value) == 1) {
                record_found = true;

                std::cout << "[InsertWithColumnNames] Найдена запись с id = 1:" << std::endl;
                std::cout << "  name: " << std::get<std::string>(row.at("name")) << std::endl;
                std::cout << "  age: " << std::get<int>(row.at("age")) << std::endl;
                std::cout << "  active: " << (std::get<bool>(row.at("active")) ? "true" : "false") << std::endl;

                EXPECT_EQ(std::get<std::string>(row.at("name")), "Alice");
                EXPECT_EQ(std::get<int>(row.at("age")), 30);
                EXPECT_EQ(std::get<bool>(row.at("active")), true);
                break;
            }
        }
    }

    EXPECT_TRUE(record_found);
    if (!record_found) {
        std::cout << "[InsertWithColumnNames] Запись с id = 1 не найдена." << std::endl;
    }
}

// Тест вставки без указания названий колонок
TEST_F(TableTest, InsertWithoutColumnNames) {
    std::vector<config::ColumnSchema> columns = {
        {"id", config::ColumnType::INT, 0},
        {"name", config::ColumnType::STRING, 255},
        {"age", config::ColumnType::INT, 0},
        {"active", config::ColumnType::BOOL, 0}
    };
    memdb::Table table(columns);

    std::vector<std::string> insert_values = {
        "2",       // id
        "Bob",     // name
        "25",      // age
        "false"    // active
    };

    bool result = table.insertRecord(insert_values);
    EXPECT_TRUE(result);

    std::cout << "[InsertWithoutColumnNames] Результат вставки записи без указания названий колонок: " << (result ? "успешно" : "неудачно") << std::endl;

    const auto& data = table.getData();
    bool record_found = false;
    const config::RowType* found_row = nullptr;

    // Ищем запись с id = 2
    for (const auto& [record_id, row] : data) {
        auto id_it = row.find("id");
        if (id_it != row.end()) {
            const auto& id_value = id_it->second;
            if (std::holds_alternative<int>(id_value) && std::get<int>(id_value) == 2) {
                record_found = true;
                found_row = &row;
                break;
            }
        }
    }

    EXPECT_TRUE(record_found);
    if (record_found && found_row != nullptr) {
        std::cout << "[InsertWithoutColumnNames] Найдена запись с id = 2:" << std::endl;
        std::cout << "  name: " << std::get<std::string>(found_row->at("name")) << std::endl;
        std::cout << "  age: " << std::get<int>(found_row->at("age")) << std::endl;
        std::cout << "  active: " << (std::get<bool>(found_row->at("active")) ? "true" : "false") << std::endl;

        EXPECT_EQ(std::get<std::string>(found_row->at("name")), "Bob");
        EXPECT_EQ(std::get<int>(found_row->at("age")), 25);
        EXPECT_EQ(std::get<bool>(found_row->at("active")), false);
    } else {
        std::cout << "[InsertWithoutColumnNames] Запись с id = 2 не найдена." << std::endl;
    }
}

// Тест вставки с отсутствующими значениями
TEST_F(TableTest, InsertWithMissingValues) {
    std::vector<config::ColumnSchema> columns = {
        {"id", config::ColumnType::INT, 0},
        {"name", config::ColumnType::STRING, 255},
        {"age", config::ColumnType::INT, 0},
        {"active", config::ColumnType::BOOL, 0}
    };
    memdb::Table table(columns);

    std::unordered_map<std::string, std::string> insert_values = {
        {"id", "3"},
        {"name", "Charlie"}
        // Отсутствуют "age" и "active"
    };

    bool result = table.insertRecord(insert_values);
    EXPECT_TRUE(result);

    std::cout << "[InsertWithMissingValues] Результат вставки записи с отсутствующими значениями: " << (result ? "успешно" : "неудачно") << std::endl;

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
        std::cout << "[InsertWithMissingValues] Найдена запись с id = 3:" << std::endl;
        std::cout << "  name: " << std::get<std::string>(found_row->at("name")) << std::endl;
        std::cout << "  age (по умолчанию): " << std::get<int>(found_row->at("age")) << std::endl;
        std::cout << "  active (по умолчанию): " << (std::get<bool>(found_row->at("active")) ? "true" : "false") << std::endl;

        EXPECT_EQ(std::get<std::string>(found_row->at("name")), "Charlie");
        EXPECT_EQ(std::get<int>(found_row->at("age")), 0);          // Значение по умолчанию для INT
        EXPECT_EQ(std::get<bool>(found_row->at("active")), false);  // Значение по умолчанию для BOOL
    } else {
        std::cout << "[InsertWithMissingValues] Запись с id = 3 не найдена." << std::endl;
    }
}

// Тест вставки с лишними значениями без указания названий колонок
TEST_F(TableTest, InsertExcessValuesWithoutColumnNames) {
    std::vector<config::ColumnSchema> columns = {
        {"id", config::ColumnType::INT, 0},
        {"name", config::ColumnType::STRING, 255}
    };
    memdb::Table table(columns);

    std::vector<std::string> insert_values = {
        "4",          // id
        "David",      // name
        "ExtraValue"  // Лишнее значение
    };

    bool result = table.insertRecord(insert_values);
    EXPECT_FALSE(result);

    std::cout << "[InsertExcessValuesWithoutColumnNames] Результат вставки записи с лишними значениями: " << (result ? "успешно" : "неудачно") << std::endl;

    const auto& data = table.getData();
    bool record_found = false;

    for (const auto& [record_id, row] : data) {
        auto id_it = row.find("id");
        if (id_it != row.end()) {
            const auto& id_value = id_it->second;
            if (std::holds_alternative<int>(id_value) && std::get<int>(id_value) == 4) {
                record_found = true;
                break;
            }
        }
    }

    EXPECT_FALSE(record_found);
    if (record_found) {
        std::cout << "[InsertExcessValuesWithoutColumnNames] Запись с id = 4 была добавлена, что не соответствует ожиданиям." << std::endl;
    } else {
        std::cout << "[InsertExcessValuesWithoutColumnNames] Запись с id = 4 не найдена, как и ожидалось." << std::endl;
    }
}

// Тест вставки с невалидным значением для INT
TEST_F(TableTest, InsertInvalidIntValue) {
    std::vector<config::ColumnSchema> columns = {
        {"id", config::ColumnType::INT, 0},
        {"name", config::ColumnType::STRING, 255},
        {"age", config::ColumnType::INT, 0},
        {"active", config::ColumnType::BOOL, 0}
    };
    memdb::Table table(columns);

    std::unordered_map<std::string, std::string> insert_values = {
        {"id", "not_an_integer"}, // Невалидное значение для INT
        {"name", "Eve"},
        {"age", "30"},
        {"active", "true"}
    };

    bool result = table.insertRecord(insert_values);
    EXPECT_FALSE(result);

    std::cout << "[InsertInvalidIntValue] Результат вставки записи с невалидным значением для int: " << (result ? "успешно" : "неудачно") << std::endl;

    const auto& data = table.getData();
    bool record_found = false;

    for (const auto& [record_id, row] : data) {
        auto name_it = row.find("name");
        if (name_it != row.end()) {
            const auto& name_value = name_it->second;
            if (std::holds_alternative<std::string>(name_value) && std::get<std::string>(name_value) == "Eve") {
                record_found = true;
                break;
            }
        }
    }

    EXPECT_FALSE(record_found);
    if (record_found) {
        std::cout << "[InsertInvalidIntValue] Запись с именем 'Eve' была добавлена, что не соответствует ожиданиям." << std::endl;
    } else {
        std::cout << "[InsertInvalidIntValue] Запись с именем 'Eve' не найдена, как и ожидалось." << std::endl;
    }
}

// Тест вставки значений в конец схемы без указания названий колонок
TEST_F(TableTest, InsertValuesAtEndOfSchema) {
    std::vector<config::ColumnSchema> columns = {
        {"id", config::ColumnType::INT, 0},
        {"name", config::ColumnType::STRING, 255},
        {"age", config::ColumnType::INT, 0},
        {"car", config::ColumnType::BOOL, 0}
    };
    memdb::Table table(columns);

    // Вставляем значения для 'age' и 'car' без указания названий колонок
    std::vector<std::string> insert_values = {
        "30",     // Значение для 'age'
        "true"    // Значение для 'car'
    };

    bool result = table.insertRecord(insert_values);
    EXPECT_TRUE(result);

    std::cout << "[InsertValuesAtEndOfSchema] Результат вставки записи: " << (result ? "успешно" : "неудачно") << std::endl;

    const auto& data = table.getData();
    bool record_found = false;
    const config::RowType* found_row = nullptr;

    // Ожидаемый ID записи (начинается с 0)
    int expected_id = 0;

    auto it = data.find(expected_id);
    if (it != data.end()) {
        record_found = true;
        found_row = &(it->second);
    }

    EXPECT_TRUE(record_found);
    if (record_found && found_row != nullptr) {
        std::cout << "[InsertValuesAtEndOfSchema] Найдена запись с id = " << expected_id << ":" << std::endl;

        // Проверяем значения колонок
        // 'id' и 'name' должны иметь значения по умолчанию
        const auto& id_value = found_row->at("id");
        EXPECT_EQ(std::get<int>(id_value), 0); // Значение по умолчанию для INT

        const auto& name_value = found_row->at("name");
        EXPECT_EQ(std::get<std::string>(name_value), ""); // Значение по умолчанию для STRING

        // Проверяем установленные значения для 'age' и 'car'
        const auto& age_value = found_row->at("age");
        EXPECT_EQ(std::get<int>(age_value), 30);

        const auto& car_value = found_row->at("car");
        EXPECT_EQ(std::get<bool>(car_value), true);

        std::cout << "  id (по умолчанию): " << std::get<int>(id_value) << std::endl;
        std::cout << "  name (по умолчанию): '" << std::get<std::string>(name_value) << "'" << std::endl;
        std::cout << "  age: " << std::get<int>(age_value) << std::endl;
        std::cout << "  car: " << (std::get<bool>(car_value) ? "true" : "false") << std::endl;
    } else {
        std::cout << "[InsertValuesAtEndOfSchema] Запись с id = " << expected_id << " не найдена." << std::endl;
    }
}

// Тест вставки с использованием BitString и значений по умолчанию
TEST_F(TableTest, InsertWithBitStringValues) {
    // Определяем таблицу с колонками разных типов, включая две колонки типа BitString
    std::vector<config::ColumnSchema> columns = {
        {"id", config::ColumnType::INT, 0},
        {"data1", config::ColumnType::BITSTRING, 8},   // Предполагаемый max_size = 8 байт
        {"data2", config::ColumnType::BITSTRING, 8},
        {"description", config::ColumnType::STRING, 255}
    };
    memdb::Table table(columns);

    // Вставляем запись, указывая значение только для одной из колонок типа BitString
    std::unordered_map<std::string, std::string> insert_values = {
        {"id", "10"},
        {"data1", "0xDEADBEEF"},
        {"description", "Test description"}
        // Колонка "data2" не указана и должна получить значение по умолчанию
    };

    bool result = table.insertRecord(insert_values);
    EXPECT_TRUE(result);

    std::cout << "[InsertWithBitStringValues] Результат вставки записи с BitString: " << (result ? "успешно" : "неудачно") << std::endl;

    const auto& data = table.getData();
    bool record_found = false;
    const config::RowType* found_row = nullptr;

    // Ищем запись с id = 10
    for (const auto& [record_id, row] : data) {
        auto id_it = row.find("id");
        if (id_it != row.end()) {
            const auto& id_value = id_it->second;
            if (std::holds_alternative<int>(id_value) && std::get<int>(id_value) == 10) {
                record_found = true;
                found_row = &row;
                break;
            }
        }
    }

    EXPECT_TRUE(record_found);
    if (record_found && found_row != nullptr) {
        std::cout << "[InsertWithBitStringValues] Найдена запись с id = 10:" << std::endl;

        // Проверяем значение "data1"
        const auto& data1_value = found_row->at("data1");
        EXPECT_TRUE(std::holds_alternative<config::BitString>(data1_value));

        const config::BitString expected_data1 = {0xDE, 0xAD, 0xBE, 0xEF};
        EXPECT_EQ(std::get<config::BitString>(data1_value), expected_data1);

        // Выводим значение "data1"
        std::cout << "  data1: ";
        for (uint8_t byte : std::get<config::BitString>(data1_value)) {
            std::cout << std::hex << static_cast<int>(byte);
        }
        std::cout << std::dec << std::endl;

        // Проверяем значение "data2" (должно быть значением по умолчанию)
        const auto& data2_value = found_row->at("data2");
        EXPECT_TRUE(std::holds_alternative<config::BitString>(data2_value));
        EXPECT_TRUE(std::get<config::BitString>(data2_value).empty());

        // Выводим значение "data2"
        std::cout << "  data2 (по умолчанию): ";
        if (std::get<config::BitString>(data2_value).empty()) {
            std::cout << "(пусто)" << std::endl;
        } else {
            for (uint8_t byte : std::get<config::BitString>(data2_value)) {
                std::cout << std::hex << static_cast<int>(byte);
            }
            std::cout << std::dec << std::endl;
        }

        // Проверяем значение "description"
        const auto& description_value = found_row->at("description");
        EXPECT_EQ(std::get<std::string>(description_value), "Test description");

        std::cout << "  description: " << std::get<std::string>(description_value) << std::endl;

        // Проверяем значение "id"
        const auto& id_value = found_row->at("id");
        EXPECT_EQ(std::get<int>(id_value), 10);
    } else {
        std::cout << "[InsertWithBitStringValues] Запись с id = 10 не найдена." << std::endl;
    }
}

// Тест автоинкремента 'id' и проверки на существующий 'id'
TEST_F(TableTest, AutoIncrementIdAndDuplicateCheck) {
    // Создаем таблицу с 'id' как autoincrement и unique
    std::vector<config::ColumnSchema> columns = {
        {"id", config::ColumnType::INT, 0, {1, 1, 1}},
        {"name", config::ColumnType::STRING, 50, {0, 0, 0}}
    };
    memdb::Table table(columns);

    // Вставляем первую запись без указания 'id'
    std::unordered_map<std::string, std::string> record1 = {
        {"name", "Alice"}
    };
    EXPECT_TRUE(table.insertRecord(record1));

    // Вставляем вторую запись с указанием дублирующегося 'id'
    std::unordered_map<std::string, std::string> record2 = {
        {"id", "0"}, // 'id' совпадает с автоприращенным значением предыдущей записи
        {"name", "Bob"}
    };
    EXPECT_FALSE(table.insertRecord(record2));

    // Проверяем, что в таблице только одна запись
    EXPECT_EQ(table.getData().size(), 1);
}

// Тест вставки строки и битовой последовательности, превышающих максимальный размер
TEST_F(TableTest, InsertValueExceedsMaxSize) {
    std::vector<config::ColumnSchema> columns = {
        {"id", config::ColumnType::INT, 0},
        {"name", config::ColumnType::STRING, 10}, // Максимальный размер строки - 10 символов
        {"data", config::ColumnType::BITSTRING, 4} // Максимальный размер битовой строки - 4 байта
    };
    memdb::Table table(columns);

    // Строка, превышающая максимальный размер
    std::string long_string = "This string is definitely longer than 10 characters";

    // Битовая строка, превышающая максимальный размер (например, 5 байт при max_size = 4)
    std::string long_bitstring = "0xDEADBEEFCAFE";

    // Попытка вставить запись с длинной строкой
    std::unordered_map<std::string, std::string> insert_values_string = {
        {"id", "1"},
        {"name", long_string}
    };

    bool result_string = table.insertRecord(insert_values_string);
    EXPECT_FALSE(result_string);

    std::cout << "[InsertValueExceedsMaxSize] Попытка вставить длинную строку: " << (result_string ? "успешно" : "неудачно") << std::endl;

    // Попытка вставить запись с длинной битовой строкой
    std::unordered_map<std::string, std::string> insert_values_bitstring = {
        {"id", "2"},
        {"data", long_bitstring}
    };

    bool result_bitstring = table.insertRecord(insert_values_bitstring);
    EXPECT_FALSE(result_bitstring);

    std::cout << "[InsertValueExceedsMaxSize] Попытка вставить длинную битовую строку: " << (result_bitstring ? "успешно" : "неудачно") << std::endl;

    // Убеждаемся, что записи не вставились
    const auto& data = table.getData();
    EXPECT_EQ(data.size(), 0);

    if (data.empty()) {
        std::cout << "[InsertValueExceedsMaxSize] Таблица пуста, как и ожидалось." << std::endl;
    } else {
        std::cout << "[InsertValueExceedsMaxSize] Таблица содержит данные, что не соответствует ожиданиям." << std::endl;
    }

    // Строка ровно максимального размера
    std::string max_size_string = "1234567890"; // 10 символов

    // Битовая строка ровно максимального размера (4 байта)
    std::string max_size_bitstring = "0xDEADBEEF"; // 4 байта

    // Попытка вставить запись с допустимыми значениями
    std::unordered_map<std::string, std::string> insert_values_valid = {
        {"id", "3"},
        {"name", max_size_string},
        {"data", max_size_bitstring}
    };

    bool result_valid = table.insertRecord(insert_values_valid);
    EXPECT_TRUE(result_valid);

    std::cout << "[InsertValueExceedsMaxSize] Вставка допустимых значений: " << (result_valid ? "успешно" : "неудачно") << std::endl;

    // Убеждаемся, что запись вставилась
    EXPECT_EQ(table.getData().size(), 1);
}


// tests/table_tests.cpp

TEST_F(TableTest, ExpandedUniqueInsertTest) {
    // Определяем схему таблицы с тремя колонками
    // Массив атрибутов: {unique, autoincrement, key}
    std::vector<config::ColumnSchema> columns = {
        {"id", config::ColumnType::INT, 0, {1, 1, 0}},        // autoincrement, key
        {"email", config::ColumnType::STRING, 100, {1, 0, 0}}, // unique
        {"username", config::ColumnType::STRING, 50, {0, 0, 0}} // не уникальный столбец
    };
    memdb::Table table(columns);

    // Вставляем первую запись с уникальными 'email' и 'username'
    std::unordered_map<std::string, std::string> record1 = {
        {"email", "user1@example.com"},
        {"username", "user1"}
    };
    EXPECT_TRUE(table.insertRecord(record1));

    // Вставляем вторую запись с другими 'email' и 'username'
    std::unordered_map<std::string, std::string> record2 = {
        {"email", "user2@example.com"},
        {"username", "user2"}
    };
    EXPECT_TRUE(table.insertRecord(record2));

    // Попытка вставить запись с дублирующимся 'email' и другим 'username'
    std::unordered_map<std::string, std::string> duplicate_email_record = {
        {"email", "user1@example.com"},
        {"username", "user3"}
    };
    EXPECT_FALSE(table.insertRecord(duplicate_email_record));

    // Попытка вставить запись с дублирующимся 'username' и уникальным 'email'
    std::unordered_map<std::string, std::string> duplicate_username_record = {
        {"email", "user3@example.com"},
        {"username", "user2"}
    };
    EXPECT_TRUE(table.insertRecord(duplicate_username_record));

    // Вставляем запись с уникальным 'email' и дублирующимся 'username'
    std::unordered_map<std::string, std::string> record3 = {
        {"email", "user4@example.com"},
        {"username", "user2"}
    };
    EXPECT_TRUE(table.insertRecord(record3));

    // Попытка вставить запись с дублирующимися 'email' и 'username'
    std::unordered_map<std::string, std::string> duplicate_both_record = {
        {"email", "user2@example.com"},
        {"username", "user2"}
    };
    EXPECT_FALSE(table.insertRecord(duplicate_both_record));

    // Проверяем, что таблица содержит только четыре успешные записи
    const auto& data = table.getData();
    EXPECT_EQ(data.size(), 4);

    // Выводим содержимое таблицы для визуализации
    std::cout << "Содержимое таблицы после вставок:" << std::endl;
    for (const auto& [record_id, row] : data) {
        int id = std::get<int>(row.at("id"));
        std::string email = std::get<std::string>(row.at("email"));
        std::string username = std::get<std::string>(row.at("username"));
        std::cout << "ID: " << id << ", Email: " << email << ", Username: " << username << std::endl;
    }

    // Проверяем, что значения 'id' назначены корректно
    std::set<int> ids;
    for (const auto& [record_id, row] : data) {
        int id = std::get<int>(row.at("id"));
        ids.insert(id);
    }
    EXPECT_TRUE(ids.count(0));
    EXPECT_TRUE(ids.count(1));
    EXPECT_TRUE(ids.count(2));
    EXPECT_TRUE(ids.count(3));
}

TEST_F(TableTest, CombinedAttributesTest) {
    // Обновленная схема с уникальным autoincrement 'id'
    std::vector<config::ColumnSchema> columns = {
        {"id", config::ColumnType::INT, 0, {1, 1, 1}},
        {"email", config::ColumnType::STRING, 100, {1, 0, 0}},
        {"username", config::ColumnType::STRING, 50, {0, 0, 0}}
    };
    memdb::Table table(columns);

    // Вставляем запись с уникальным 'email'
    std::unordered_map<std::string, std::string> record1 = {
        {"email", "user@example.com"},
        {"username", "user1"}
    };
    EXPECT_TRUE(table.insertRecord(record1));

    // Попытка вставить запись с тем же 'email'
    std::unordered_map<std::string, std::string> record2 = {
        {"email", "user@example.com"},
        {"username", "user2"}
    };
    EXPECT_FALSE(table.insertRecord(record2));

    // Проверяем количество записей в таблице
    EXPECT_EQ(table.getData().size(), 1);
}

TEST_F(TableTest, AutoIncrementAttributeInsertTest) {
    using namespace memdb;

    // Определяем схему таблицы
    std::vector<config::ColumnSchema> columns = {
        // Атрибуты: {unique, autoincrement, key}
        {"id", config::ColumnType::INT, 0, {1, 1, 1}}, // Автоинкремент для 'id'
        {"name", config::ColumnType::STRING, 255, {0, 0, 0}}
    };

    Table table(columns);

    // Вставляем первую запись
    std::unordered_map<std::string, std::string> insert_values1 = {
        {"name", "First Record"}
    };
    EXPECT_TRUE(table.insertRecord(insert_values1));
    std::cout << "Inserted: {name: \"First Record\"}" << std::endl;

    // Вставляем вторую запись
    std::unordered_map<std::string, std::string> insert_values2 = {
        {"name", "Second Record"}
    };
    EXPECT_TRUE(table.insertRecord(insert_values2));
    std::cout << "Inserted: {name: \"Second Record\"}" << std::endl;

    // Получаем все данные из таблицы
    const auto& data = table.getData();

    // Проверяем количество записей
    EXPECT_EQ(data.size(), 2);
    std::cout << "Total records: " << data.size() << std::endl;

    // Выводим записи на консоль
    std::cout << "Table contents:" << std::endl;
    for (const auto& [record_id, row] : data) {
        int id = std::get<int>(row.at("id"));
        std::string name = std::get<std::string>(row.at("name"));
        std::cout << "Record ID: " << id << ", Name: " << name << std::endl;
    }

    // Проверяем значения 'id' в записях
    EXPECT_EQ(std::get<int>(data.at(0).at("id")), 0);
    EXPECT_EQ(std::get<std::string>(data.at(0).at("name")), "First Record");
    EXPECT_EQ(std::get<int>(data.at(1).at("id")), 1);
    EXPECT_EQ(std::get<std::string>(data.at(1).at("name")), "Second Record");
}

TEST_F(TableTest, UserDefaultValueTest) {
    // Определяем схему таблицы с default_value
    std::vector<config::ColumnSchema> columns = {
        {"id", config::ColumnType::INT, 0, {1, 1, 1}},               // Поле с autoincrement, unique, key
        {"status", config::ColumnType::STRING, 20, {0, 0, 0}, "active"}, // Поле с default_value "active"
        {"priority", config::ColumnType::INT, 0, {0, 0, 0}, "5"},        // Поле с default_value 5
        {"description", config::ColumnType::STRING, 255, {0, 0, 0}}  // Поле без default_value
    };
    memdb::Table table(columns);

    // Вставляем запись без указания 'status' и 'priority'
    std::unordered_map<std::string, std::string> record = {
        {"description", "Test record"}
    };
    EXPECT_TRUE(table.insertRecord(record));

    // Получаем данные из таблицы
    const auto& data = table.getData();
    EXPECT_EQ(data.size(), 1);

    const config::RowType& row = data.at(0); // 'id' первой записи — 0

    // Проверяем, что 'status' соответствует "active"
    EXPECT_EQ(std::get<std::string>(row.at("status")), "active");

    // Проверяем, что 'priority' соответствует 5
    EXPECT_EQ(std::get<int>(row.at("priority")), 5);

    // Проверяем, что 'description' соответствует "Test record"
    EXPECT_EQ(std::get<std::string>(row.at("description")), "Test record");

    // Проверяем, что 'id' соответствует 0
    EXPECT_EQ(std::get<int>(row.at("id")), 0);

    // Вставляем вторую запись без указания 'description'
    std::unordered_map<std::string, std::string> record2 = {
        {"status", "inactive"}
    };
    EXPECT_TRUE(table.insertRecord(record2));

    // Проверяем, что 'description' установлено через getDefaultValue
    EXPECT_EQ(data.size(), 2);

    const config::RowType& row2 = data.at(1); // 'id' второй записи — 1

    // 'description' должно быть пустой строкой из getDefaultValue для STRING
    EXPECT_EQ(std::get<std::string>(row2.at("description")), "");

    // Проверяем, что 'status' соответствует "inactive"
    EXPECT_EQ(std::get<std::string>(row2.at("status")), "inactive");

    // Проверяем, что 'priority' соответствует 5 (default_value)
    EXPECT_EQ(std::get<int>(row2.at("priority")), 5);

    // Проверяем, что 'id' соответствует 1
    EXPECT_EQ(std::get<int>(row2.at("id")), 1);
}

TEST_F(TableTest, AutoincrementUniqueFieldsTest) {
    // Определяем схему таблицы
    std::vector<config::ColumnSchema> columns = {
        // {имя, тип, размер, {unique, autoincrement, key}, default_value}
        {"id", config::ColumnType::INT, 0, {1, 1, 1}},          // autoincrement, unique, key
        {"auto_field1", config::ColumnType::INT, 0, {0, 1, 0}}, // autoincrement
        {"regular_field", config::ColumnType::STRING, 255, {0, 0, 0}}, // обычное поле
        {"unique_field", config::ColumnType::STRING, 100, {1, 0, 0}},   // unique
        {"auto_field2", config::ColumnType::INT, 0, {0, 1, 0}}  // еще одно autoincrement поле
    };
    memdb::Table table(columns);

    // Вставляем первую запись
    std::unordered_map<std::string, std::string> record1 = {
        {"regular_field", "Value1"},
        {"unique_field", "Unique1"}
    };
    EXPECT_TRUE(table.insertRecord(record1));

    std::unordered_map<std::string, std::string> record3 = {
        {"regular_field", "Value2"},
        {"auto_field1", "1234"},
        {"unique_field", "Unique3"}
    };
    EXPECT_TRUE(table.insertRecord(record3));

    // Вставляем вторую запись
    std::unordered_map<std::string, std::string> record2 = {
        {"regular_field", "Value2"},
        {"unique_field", "Unique2"}
    };
    EXPECT_TRUE(table.insertRecord(record2));


    // Получаем данные из таблицы
    const auto& data = table.getData();
    EXPECT_EQ(data.size(), 3);

    // Выводим содержимое таблицы для визуализации
    std::cout << "Содержимое таблицы после вставок:" << std::endl;
    for (const auto& [record_id, row] : data) {
        int id = std::get<int>(row.at("id"));
        int auto_field1 = std::get<int>(row.at("auto_field1"));
        int auto_field2 = std::get<int>(row.at("auto_field2"));
        std::string regular_field = std::get<std::string>(row.at("regular_field"));
        std::string unique_field = std::get<std::string>(row.at("unique_field"));

        std::cout << "Record ID: " << record_id << std::endl;
        std::cout << "  id: " << id << std::endl;
        std::cout << "  auto_field1: " << auto_field1 << std::endl;
        std::cout << "  auto_field2: " << auto_field2 << std::endl;
        std::cout << "  regular_field: " << regular_field << std::endl;
        std::cout << "  unique_field: " << unique_field << std::endl;
    }

    // Проверяем значения autoincrement полей на корректность
    // Так как data — это unordered_map, сначала собираем id и сортируем их
    std::vector<int> ids;
    for (const auto& [record_id, row] : data) {
        ids.push_back(std::get<int>(row.at("id")));
    }
    std::sort(ids.begin(), ids.end());

}