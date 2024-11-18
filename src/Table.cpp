#include "Table.h"

#include <functional>
#include <string>
#include <charconv>

namespace memdb 
{

bool Table::insertRecord(const std::unordered_map<std::string, std::string>& insert_values) 
{
    config::RowType row;

    if (insert_values.empty()) {
        return false;
    }

    int id_value = -1; // это нужно, чтобы проверить был ли id в запросе

    for (const auto& [column_name, value_str] : insert_values) {
        auto schema_it = std::find_if(schema_.begin(), schema_.end(),
            [&column_name](const auto& pair) { return pair.first == column_name; });
        if (schema_it == schema_.end()) {
            return false;
        }
        config::ColumnType expected_type = schema_it->second;

        config::ColumnValue value;
        if (!convertValue(value_str, expected_type, value)) {
            return false;
        }
        row[column_name] = value;

        if (column_name == "id") {
            if (std::holds_alternative<int>(value)) {
                id_value = std::get<int>(value);
            } else {
                return false;
            }
        }
    }

    int id;
    if (id_value == -1) {
        id = next_id_++;
    } else {
        id = id_value;

        if (data_.find(id) != data_.end()) {
            return false; // Запись с данным 'id' уже существует
        }

        if (id >= next_id_) {
            next_id_ = id + 1;
        }
    }

    for (const auto& [column_name, column_type] : schema_) {
        if (row.find(column_name) == row.end()) {
            row[column_name] = getDefaultValue(column_type);
        }
    }

    data_[id] = row;
    indexRow(id, row);

    return true;
}

bool Table::insertRecord(const std::vector<std::string>& insert_values) 
{
    if (insert_values.empty() || insert_values.size() > schema_.size()) {
        return false;
    }

    config::RowType row;
    auto schema_it = schema_.rbegin();
    auto value_it = insert_values.rbegin();

    int id_value = -1; // Тоже самое, что и в методе выше

    while (value_it != insert_values.rend() && schema_it != schema_.rend()) {
        const auto& [column_name, column_type] = *schema_it;
        config::ColumnValue value;
        if (!convertValue(*value_it, column_type, value)) {
            return false; 
        }
        row[column_name] = value;

        if (column_name == "id") {
            if (std::holds_alternative<int>(value)) {
                id_value = std::get<int>(value);
            } else {
                return false; 
            }
        }

        ++value_it;
        ++schema_it;
    }

    while (schema_it != schema_.rend()) {
        const auto& [column_name, column_type] = *schema_it;
        row[column_name] = getDefaultValue(column_type);
        ++schema_it;
    }

    int id;
    if (id_value == -1) {
        id = next_id_++;
    } else {
        id = id_value;

        if (data_.find(id) != data_.end()) {
            return false; // Запись с данным 'id' уже существует
        }

        if (id >= next_id_) {
            next_id_ = id + 1;
        }
    }

    data_[id] = row;
    indexRow(id, row);

    return true;
}

bool Table::convertValue(const std::string& value_str, 
    config::ColumnType expected_type, config::ColumnValue& out_value)
{
    switch (expected_type) {
        case config::ColumnType::INT: {
            int int_value;
            auto [ptr, ec] = std::from_chars(value_str.data(), value_str.data() + value_str.size(), int_value);
            if (ec == std::errc() && ptr == value_str.data() + value_str.size()) {
                out_value = int_value;
                return true;
            }
            return false;
        }
        case config::ColumnType::BOOL: {
            if (value_str == "true") {
                out_value = true;
                return true;
            } else if (value_str == "false") {
                out_value = false;
                return true;
            }
            return false;
        }
        case config::ColumnType::BITSTRING: {
            if (value_str.size() > 2 && value_str[0] == '0' && (value_str[1] == 'x' || value_str[1] == 'X')) {
                std::string hex_str = value_str.substr(2);
                if (hex_str.empty() || hex_str.size() % 2 != 0) {
                    return false;
                }
                config::BitString bit_string;
                for (size_t i = 0; i < hex_str.length(); i += 2) {
                    std::string byte_str = hex_str.substr(i, 2);
                    uint8_t byte_value;
                    auto [ptr_byte, ec_byte] = std::from_chars(byte_str.data(), byte_str.data() + byte_str.size(), byte_value, 16);
                    if (ec_byte != std::errc()) {
                        return false;
                    }
                    bit_string.push_back(byte_value);
                }
                out_value = bit_string;
                return true;
            }
            return false;
        }
        case config::ColumnType::STRING: {
            out_value = value_str;
            return true;
        }
        default:
            return false;
    }
}

config::ColumnValue Table::getDefaultValue(config::ColumnType column_type) 
{
    switch (column_type) {
        case config::ColumnType::INT:
            return int(0);
        case config::ColumnType::STRING:
            return std::string("");
        case config::ColumnType::BOOL:
            return bool(false);
        case config::ColumnType::BITSTRING:
            return config::BitString{};
        default:
            return std::string("");
    }
}

void Table::indexRow(const int& id, const config::RowType& row) 
{
    for (const auto& [column_name, _] : schema_) {
        auto it = row.find(column_name);
        if (it != row.end()) {
            const auto& value = it->second;
            size_t hash_value = makeHashKey(value);
            indices_[column_name].emplace(hash_value, id);
        }
    }
}

size_t Table::makeHashKey(const config::ColumnValue& value) const 
{
    return std::visit([](const auto& val) -> size_t {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, config::BitString>) {
            std::hash<std::string> hasher;
            std::string bitstring_str(val.begin(), val.end());
            return hasher(bitstring_str);
        } else {
            return std::hash<T>{}(val);
        }
    }, value);
}


}
