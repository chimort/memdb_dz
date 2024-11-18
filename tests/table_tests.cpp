#include <gtest/gtest.h>
#include "Table.h"

#include <iostream>
#include <string>
#include <variant>

class TableTest : public ::testing::Test {
protected:
    // Если необходимо, можно добавить настройки для всех тестов
};

// Тест вставки с указанием названий колонок
TEST_F(TableTest, InsertWithColumnNames) {
    std::vector<std::pair<std::string, config::ColumnType>> columns = {
        {"id", config::ColumnType::INT},
        {"name", config::ColumnType::STRING},
        {"age", config::ColumnType::INT},
        {"active", config::ColumnType::BOOL}
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
    std::vector<std::pair<std::string, config::ColumnType>> columns = {
        {"id", config::ColumnType::INT},
        {"name", config::ColumnType::STRING},
        {"age", config::ColumnType::INT},
        {"active", config::ColumnType::BOOL}
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
    std::vector<std::pair<std::string, config::ColumnType>> columns = {
        {"id", config::ColumnType::INT},
        {"name", config::ColumnType::STRING},
        {"age", config::ColumnType::INT},
        {"active", config::ColumnType::BOOL}
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
    std::vector<std::pair<std::string, config::ColumnType>> columns = {
        {"id", config::ColumnType::INT},
        {"name", config::ColumnType::STRING}
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
    std::vector<std::pair<std::string, config::ColumnType>> columns = {
        {"id", config::ColumnType::INT},
        {"name", config::ColumnType::STRING},
        {"age", config::ColumnType::INT},
        {"active", config::ColumnType::BOOL}
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
    std::vector<std::pair<std::string, config::ColumnType>> columns = {
        {"id", config::ColumnType::INT},
        {"name", config::ColumnType::STRING},
        {"age", config::ColumnType::INT},
        {"car", config::ColumnType::BOOL}
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
    std::vector<std::pair<std::string, config::ColumnType>> columns = {
        {"id", config::ColumnType::INT},
        {"data1", config::ColumnType::BITSTRING},
        {"data2", config::ColumnType::BITSTRING},
        {"description", config::ColumnType::STRING}
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
    std::vector<std::pair<std::string, config::ColumnType>> columns = {
        {"id", config::ColumnType::INT},
        {"name", config::ColumnType::STRING}
    };
    memdb::Table table(columns);

    // Вставляем записи без указания 'id'
    std::unordered_map<std::string, std::string> insert_values1 = {
        {"name", "Record1"}
    };
    EXPECT_TRUE(table.insertRecord(insert_values1));

    std::unordered_map<std::string, std::string> insert_values2 = {
        {"name", "Record2"}
    };
    EXPECT_TRUE(table.insertRecord(insert_values2));

    // Вставляем запись с 'id = 10'
    std::unordered_map<std::string, std::string> insert_values3 = {
        {"id", "10"},
        {"name", "Record10"}
    };
    EXPECT_TRUE(table.insertRecord(insert_values3));

    // Вставляем запись без указания 'id' (должен получить 'id = 11')
    std::unordered_map<std::string, std::string> insert_values4 = {
        {"name", "Record11"}
    };
    EXPECT_TRUE(table.insertRecord(insert_values4));

    // Проверяем, что 'next_id_' обновился до 12
    // Внутри класса 'Table' 'next_id_' приватный, поэтому прямо проверить не можем
    // Проверим по наличию записей с соответствующими 'id'

    const auto& data = table.getData();
    EXPECT_EQ(data.size(), 4);

    // Проверяем 'id' записей
    std::set<int> ids;
    for (const auto& [id, row] : data) {
        ids.insert(id);
    }
    EXPECT_TRUE(ids.count(0));
    EXPECT_TRUE(ids.count(1));
    EXPECT_TRUE(ids.count(10));
    EXPECT_TRUE(ids.count(11));

    // Пытаемся вставить запись с существующим 'id = 1'
    std::unordered_map<std::string, std::string> insert_values5 = {
        {"id", "1"},
        {"name", "DuplicateRecord"}
    };
    EXPECT_FALSE(table.insertRecord(insert_values5));

    // Проверяем, что запись не добавилась
    EXPECT_EQ(data.size(), 4);
}