#include "Table.h"
#include "Config.h"

#include <fstream>
#include <string>
#include <charconv>
#include <functional>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <numeric>

namespace memdb
{

bool Table::insertRecord(const std::unordered_map<std::string, std::string>& insert_values)
{
    if (insert_values.empty()) {
        return false;
    }

    config::RowType row;

    for (const auto& [column_name, value_str] : insert_values) {
        auto schema_it = std::find_if(schema_.begin(), schema_.end(),
            [&column_name](const auto& col) { return col.name == column_name; });
        if (schema_it == schema_.end()) {
            return false;
        }
        const config::ColumnSchema& column_schema = *schema_it;

        config::ColumnValue value;
        if (!convertValue(value_str, column_schema, value)) {
            return false;
        }

        if (column_schema.attributes[0] || column_schema.attributes[2]) {
            if (indices_.find(column_name) != indices_.end()) {
                if (has_index_ && indices_[column_name].find(makeHashKey(value)) != indices_[column_name].end()) {
                    return false;
                }
            } else {
                for (auto& [key, every_col_value] : data_) {
                    if (every_col_value[column_name] == value) {
                        return false;
                    }
                }
            }
        }

        row[column_name] = value;
      
        if (column_schema.attributes[1]) { 
            if (!std::holds_alternative<int>(value)) {
                return false;
            }
            autoincrement_counters_[column_name].insert(std::get<int>(value));
        }
    }

    for (const auto& column_schema : schema_) {
        const std::string& column_name = column_schema.name;
        if (row.find(column_name) == row.end()) {
            if (column_schema.attributes[1]) { // Если autoincrement
                int min_v = findMinUnusedId(column_name);
                autoincrement_counters_[column_name].insert(min_v);
                row[column_name] = min_v;
            } else if (!column_schema.default_value.empty()) {
                config::ColumnValue default_value;
                if (!convertValue(column_schema.default_value, column_schema, default_value)) {
                    return false;
                }
                
                if (column_schema.attributes[0]) {
                    row[column_name] = std::monostate{};
                } else {
                    row[column_name] = default_value;
                }
            } else {
                if (column_schema.attributes[0] || column_schema.attributes[2]) {
                    if (unique_null_value_[column_name]) {
                        return false;
                    }
                    unique_null_value_[column_name] = true;
                }
                row[column_name] = std::monostate{}; // Значение NULL
            }
        }
    }

    // Обновляем id
    int id;
    id = next_id_++;
    data_[id] = row;

    if (has_index_) {
        insertIndices(id, row);
    }

    return true;
}

bool Table::insertRecord(const std::vector<std::string>& insert_values)
{
    if (insert_values.empty() || insert_values.size() > schema_.size()) {
        return false;
    }

    config::RowType row;
    size_t num_columns = schema_.size();
    size_t num_values = insert_values.size();

    for (size_t i = 0; i < num_values; ++i) {
        size_t schema_index = num_columns - num_values + i;
        const config::ColumnSchema& column_schema = schema_[schema_index];
        const std::string& column_name = column_schema.name;
        config::ColumnValue value;

        if (!convertValue(insert_values[i], column_schema, value)) {
            return false;
        }

        if (column_schema.attributes[0] || column_schema.attributes[2]) {
            if (indices_.find(column_name) != indices_.end()) {
                if (has_index_ && indices_[column_name].find(makeHashKey(value)) != indices_[column_name].end()) {
                    return false;
                }
            } else {
                for (auto& [key, every_col_value] : data_) {
                    if (every_col_value[column_name] == value) {
                        return false;
                    }
                }
            }
        }

        row[column_name] = value;

        if (column_schema.attributes[1]) { 
            if (!std::holds_alternative<int>(value)) {
                return false;
            }
            autoincrement_counters_[column_name].insert(std::get<int>(value));
        }
    }

    for (size_t i = 0; i < num_columns - num_values; ++i) {
        const config::ColumnSchema& column_schema = schema_[i];
        const std::string& column_name = column_schema.name;
        if (column_schema.attributes[1]) { // Если autoincrement    
            int min_v = findMinUnusedId(column_name);
            autoincrement_counters_[column_name].insert(min_v);
            row[column_name] = min_v;
        } else if (!column_schema.default_value.empty()) {
            config::ColumnValue default_value;
            if (!convertValue(column_schema.default_value, column_schema, default_value)) {
                return false;
            }

            if (column_schema.attributes[0]) {
                row[column_name] = std::monostate{};
            } else {
                row[column_name] = default_value;
            }

        } else {
            if (column_schema.attributes[0] || column_schema.attributes[2]) {
                if (unique_null_value_[column_name]) {
                    return false;
                }
                unique_null_value_[column_name] = true;
            }
            row[column_name] = std::monostate{}; // Значение NULL
        }
    }

    int id;
    id = next_id_++;
  
    if (has_index_) {
        insertIndices(id, row);
    }
    data_[id] = row;
    return true;
}

bool Table::insertRowType(const config::RowType& row) {
    int id;
    id = next_id_++;

    data_[id] = row;

    return true;
}

bool Table::updateRowType(int record_id, const config::RowType& new_row) {
    for(const auto &temp: new_row){
        data_[record_id][temp.first] = temp.second;
    }
    return true;
}

bool Table::deleteRow(const int& row_id)
{
    auto it = data_.find(row_id);
    if (it != data_.end()) {
        removeFromIndices(row_id);
        data_.erase(it);
        config::RowType& row = it->second;

        for (const auto &column : schema_) {
            const std::string& column_name = column.name;
            if (column.attributes[1]) {
                unique_null_value_[column_name] = false;
                auto value_it = row.find(column_name);
                if (value_it != row.end()) {
                    const config::ColumnValue& value = value_it->second;
                    if (std::holds_alternative<int>(value)) {
                        int int_value = std::get<int>(value);
                        autoincrement_counters_[column_name].erase(int_value);
                    }
                }
            }
        }
        return true;
    }
    return false;
}

void Table::removeFromIndices(const int& row_id)
{
    for (const auto& column_schema : schema_) {
        const std::string &column_name = column_schema.name;
        const auto &value = data_[row_id][column_name];

        if (column_schema.ordering.unordered) {
            size_t hash_value = makeHashKey(value);
            auto &index = indices_[column_name];
            auto range = index.equal_range(hash_value);
            for (auto iter = range.first; iter != range.second;) {
                if (iter->second == row_id) {
                    iter = index.erase(iter);
                } else {
                    ++iter;
                }
            }
        }

        if (column_schema.ordering.ordered) {
            auto &ordered_index = ordered_indices_[column_name];
            auto range = ordered_index.equal_range(value);
            for (auto iter = range.first; iter != range.second;) {
                if (iter->second == row_id) {
                    iter = ordered_index.erase(iter);
                } else {
                    ++iter;
                }
            }
        }
    }
}

void Table::updateIndices(const int& row_id, const config::RowType& new_row)
{
    for (const auto& column_schema : schema_) {
        const std::string &column_name = column_schema.name;
        if(new_row.find(column_name) == new_row.end()){
            continue;
        }
        const auto &value = data_[row_id][column_name];
        if (column_schema.ordering.unordered) {
            size_t hash_value = makeHashKey(value);
            auto &index = indices_[column_name];
            auto range = index.equal_range(hash_value);
            for (auto iter = range.first; iter != range.second;) {
                if (iter->second == row_id) {
                    index.erase(iter);
                    index.insert({makeHashKey(new_row.at(column_name)), row_id});
                    break;
                } else {
                    ++iter;
                }
            }
        }

        if (column_schema.ordering.ordered) {
            auto &ordered_index = ordered_indices_[column_name];
            auto range = ordered_index.equal_range(value);
            for (auto iter = range.first; iter != range.second;) {
                if (iter->second == row_id) {
                    ordered_index.erase(iter);
                    ordered_index.insert({new_row.at(column_name), row_id});
                    break;
                } else {
                    ++iter;
                }
            }
        }
    }
}

bool Table::convertValue(const std::string& value_str, const config::ColumnSchema& column_schema,
    config::ColumnValue& out_value)
{
    if (value_str == "NULL") {
        out_value = std::monostate{};
        return true;
    }
    switch (column_schema.type) {
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
        case config::ColumnType::STRING: {
            std::string temp_value = value_str;

            if (temp_value.size() >= 2 && temp_value.front() == '"' && temp_value.back() == '"') {
                temp_value = temp_value.substr(1, temp_value.size() - 2);

                size_t pos = 0;
                while ((pos = temp_value.find("\\\"", pos)) != std::string::npos) {
                    temp_value.replace(pos, 2, "\"");
                    pos += 1;
                }
            }

            if (temp_value.size() > column_schema.max_size) {
                return false;
            }

            out_value = temp_value;
            return true;
        }
        case config::ColumnType::BITSTRING: {
            if (value_str.size() > 2 && value_str[0] == '0' && (value_str[1] == 'x' || value_str[1] == 'X')) {
                std::string hex_str = value_str.substr(2);
                if (hex_str.size() > column_schema.max_size * 2 || hex_str.size()%2 == 1) {
                    return false;
                }
                config::BitString bit_string;
                auto te = column_schema.max_size * 2 - hex_str.length();
                for(int i = 0; i < te; ++i){
                    bit_string.push_back('0');
                }
                for (size_t i = 0; i < hex_str.length(); ++i) {
                    uint8_t symb = hex_str[i];
                    if((symb - '0' >= 0 && symb - '9' <= 0) || (std::tolower(symb) - 'a' >= 0 && std::tolower(symb) - 'f' <= 0)){
                        bit_string.push_back(std::tolower(hex_str[i]));
                    }
                    else{
                        return false;
                    }
                }
                out_value = bit_string;
                return true;
            }
            return false;
        }
        default:
            return false;
    }
}

int Table::findMinUnusedId(const std::string& column_name)
{
    int number = 0;
    for (int i : autoincrement_counters_[column_name]) {
        if (number == i) {
            number++;
        } else if (number < i) {
            break;
        }
    }
    return number;
}

bool Table::createIndex(const std::vector<std::string>& columns_name, config::IndexType index_type) {
    for (const auto& column_name : columns_name) {
        bool found = false;
        for (auto& column_schema : schema_) {
            if (column_schema.name == column_name) {
                column_schema.ordering = index_type;
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }

        if (index_type.unordered) {
            if (indices_.find(column_name) == indices_.end()) {
                std::unordered_multimap<std::size_t, int> index;
                indices_[column_name] = index;
                for (const auto& [id, row] : data_) {
                    auto it = row.find(column_name);
                    if (it != row.end()) {
                        const auto& value = it->second;
                        size_t hash_value = makeHashKey(value);
                        indices_[column_name].emplace(hash_value, id);
                    }
                }
            }
        } else if (index_type.ordered) {
            if (ordered_indices_.find(column_name) == ordered_indices_.end()) {
                std::multimap<config::ColumnValue, int> index;
                ordered_indices_[column_name] = index;
                for (const auto& [id, row] : data_) {
                    auto it = row.find(column_name);
                    if (it != row.end()) {
                        const auto& value = it->second;
                        ordered_indices_[column_name].emplace(value, id);
                    }
                }
            }
        }
    }
    has_index_ = true;
    return true;
}

void Table::insertIndices(const int& id, const config::RowType& row)
{
    for (const auto& column_schema : schema_) {
        const std::string& column_name = column_schema.name;
        auto it = row.find(column_name);
        if (it != row.end()) {
            const auto& value = it->second;

            if (column_schema.ordering.unordered) {
                size_t hash_value = makeHashKey(value);
                indices_[column_name].emplace(hash_value, id);
            }

            if (column_schema.ordering.ordered) {
                ordered_indices_[column_name].emplace(value, id);
            }
        }
    }
}

size_t Table::makeHashKey(const config::ColumnValue& value) const {
    return std::visit([](const auto& val) -> size_t {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
            // Возвращаем фиксированный хеш для NULL значений
            return std::hash<std::string>{}("NULL_VALUE");
        } else if constexpr (std::is_same_v<T, config::BitString>) {
            std::hash<std::string> hasher;
            std::string bitstring_str(val.begin(), val.end());
            return hasher(bitstring_str);
        } else {
            return std::hash<T>{}(val);
        }
    }, value);
}

std::string Table::columnTypeToString(config::ColumnType type) const 
{
    switch (type) {
        case config::ColumnType::INT: return "INT";
        case config::ColumnType::BOOL: return "BOOL";
        case config::ColumnType::STRING: return "STRING";
        default: return "UNKNOWN";
    }
}

std::string Table::serializeAttributes(const bool attributes[3]) const 
{
    std::vector<std::string> attr_names;

    // attributes[0] - unique
    // attributes[1] - autoincrement
    // attributes[2] - key

    if (attributes[0]) attr_names.push_back("unique");
    if (attributes[1]) attr_names.push_back("autoincrement");
    if (attributes[2]) attr_names.push_back("key");

    std::string result;
    for (size_t i = 0; i < attr_names.size(); ++i) {
        result += attr_names[i];
        if (i != attr_names.size() - 1) {
            result += ",";
        }
    }

    return result;
}

bool Table::saveToCSV(std::ofstream& ofs) const {
    ofs << "#SCHEMA#";

    for (size_t i = 0; i < schema_.size(); ++i) {
        const auto& column = schema_[i];

        ofs << column.name << ":" << columnTypeToString(column.type);

        std::string attributes_str = serializeAttributes(column.attributes);
        if (!attributes_str.empty()) {
            ofs << "{" << attributes_str << "}";
        }

        if (column.type == config::ColumnType::STRING) {
            ofs << "[" << column.max_size << "]";
        }

        if (i != schema_.size() - 1) {
            ofs << ";";
        }
    }

    ofs << '\n';

    for (size_t i = 0; i < schema_.size(); ++i) {
        ofs << schema_[i].name;
        if (i != schema_.size() - 1) {
            ofs << ',';
        }
    }
    ofs << '\n';

    for (const auto& [id, row] : data_) {
        for (size_t i = 0; i < schema_.size(); ++i) {
            const auto& column_name = schema_[i].name;
            auto it = row.find(column_name);
            if (it != row.end()) {
                std::string value_str = convertColumnValueToString(it->second);
                ofs << value_str;
            } else {
                ofs << "NULL";
            }
            if (i != schema_.size() - 1) {
                ofs << ',';
            }
        }
        ofs << '\n';
    }

    return true;
}

std::string Table::convertColumnValueToString(const config::ColumnValue& value) const
{
    if (std::holds_alternative<std::monostate>(value)) {
        return "NULL";
    }
    return std::visit([](const auto& val) -> std::string {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int>) {
            return std::to_string(val);
        } else if constexpr (std::is_same_v<T, std::string>) {
            std::string escaped = val;
            size_t pos = 0;
            while ((pos = escaped.find('"', pos)) != std::string::npos) {
                escaped.insert(pos, 1, '"');
                pos += 2;
            }
            if (escaped.find(',') != std::string::npos || escaped.find('\n') != std::string::npos || escaped.find('"') != std::string::npos) {
                escaped = '"' + escaped + '"';
            }
            return escaped;
        } else if constexpr (std::is_same_v<T, bool>) {
            return val ? "true" : "false";
        } else if constexpr (std::is_same_v<T, config::BitString>) {
            std::ostringstream oss;
            oss << "0x";
            for (uint8_t byte : val) {
                oss << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
            }
            return oss.str();
        } else {
            return "";
        }
    }, value);
}

void Table::parseAttributes(const std::string& attributes_str, bool attributes[3]) 
{
    std::stringstream ss(attributes_str);
    std::string attr;
    while (std::getline(ss, attr, ',')) {
        if (attr == "unique") {
            attributes[0] = true;
        } else if (attr == "autoincrement") {
            attributes[1] = true;
        } else if (attr == "key") {
            attributes[2] = true;
        }
    }
}

void Table::rebuildIndicesAndConstraints() {
    // Очищаем существующие индексы и структуры
    indices_.clear();
    ordered_indices_.clear();
    autoincrement_counters_.clear();
    unique_null_value_.clear();

    for (const auto& column_schema : schema_) {
        const std::string& column_name = column_schema.name;

        if (column_schema.attributes[1]) {
            autoincrement_counters_[column_name] = std::set<int>();
        }

        if (column_schema.attributes[0] || column_schema.attributes[2]) { 
            unique_null_value_[column_name] = false;
        }

        if (column_schema.attributes[2]) { 
            indices_[column_name] = std::unordered_multimap<size_t, int>();
            has_index_ = true;
        }
    }

    for (const auto& [id, row] : data_) {
        for (const auto& [column_name, value] : row) {
            if (autoincrement_counters_.find(column_name) != autoincrement_counters_.end()) {
                if (std::holds_alternative<int>(value)) {
                    autoincrement_counters_[column_name].insert(std::get<int>(value));
                }
            }

            auto schema_it = std::find_if(schema_.begin(), schema_.end(),
                [&column_name](const auto& col) { return col.name == column_name; });

            if (schema_it != schema_.end()) {
                const auto& column_schema = *schema_it;
                if (column_schema.attributes[2]) { // key
                    size_t hash_value = makeHashKey(value);
                    indices_[column_name].emplace(hash_value, id);
                }
            }

            if (std::holds_alternative<std::monostate>(value)) {
                if (schema_it != schema_.end() &&
                    (schema_it->attributes[0] || schema_it->attributes[2])) {
                    unique_null_value_[column_name] = true;
                }
            }
        }
    }

    if (!data_.empty()) {
        int max_id = std::numeric_limits<int>::min();
        for (const auto& [id, _] : data_) {
            if (id > max_id) {
                max_id = id;
            }
        }
        next_id_ = max_id + 1;
    } else {
        next_id_ = 0;
    }
}

bool Table::loadFromCSV(std::istream& is)
{
    std::string line;

    if (!std::getline(is, line)) {
        return false;
    }

    if (line.rfind("#SCHEMA#", 0) == 0) {
        std::string schema_line = line.substr(8);

        schema_ = parseSchemaLine(schema_line);
        if (schema_.empty()) {
            return false;
        }
        if (!std::getline(is, line)) {
            return false;
        }
    } else {
        if (schema_.empty()) {
            return false; 
        }
    }

    std::vector<std::string> headers = parseCSVLine(line);

    if (headers.size() != schema_.size()) {
        return false;
    }
    for (size_t i = 0; i < headers.size(); ++i) {
        if (headers[i] != schema_[i].name) {
            return false;
        }
    }

    data_.clear();
    next_id_ = 0;

    std::string record;
    while (std::getline(is, line)) {
        if (!record.empty()) {
            record += "\n";
        }
        record += line;

        size_t quote_count = std::count(record.begin(), record.end(), '"');
        if (quote_count % 2 != 0) {
            continue;
        }

        std::vector<std::string> fields = parseCSVLine(record);
        if (fields.size() != headers.size()) {
            return false;
        }

        std::unordered_map<std::string, std::string> insert_values;
        for (size_t i = 0; i < headers.size(); ++i) {
            insert_values[headers[i]] = fields[i];
        }

        if (!insertRecord(insert_values)) {
            return false;
        }
        record.clear();
    }

    if (!data_.empty()) {
        auto max_id_iter = std::max_element(data_.begin(), data_.end(),
            [](const auto& lhs, const auto& rhs) {
                return lhs.first < rhs.first;
            });
        next_id_ = max_id_iter->first + 1;
    } else {
        next_id_ = 0;
    }
    rebuildIndicesAndConstraints();
    return true;
}

std::vector<config::ColumnSchema> Table::parseSchemaLine(const std::string& schema_line) {
    std::vector<config::ColumnSchema> schema;
    std::stringstream ss(schema_line);
    std::string column_info;

    while (std::getline(ss, column_info, ';')) {
        column_info.erase(0, column_info.find_first_not_of(" \t"));
        column_info.erase(column_info.find_last_not_of(" \t") + 1);

        if (column_info.empty()) {
            continue;
        }

        size_t colon_pos = column_info.find(':');
        if (colon_pos == std::string::npos) {
            std::cerr << "Ошибка: Некорректный формат столбца в схеме: " << column_info << std::endl;
            return {};
        }

        std::string column_name = column_info.substr(0, colon_pos);
        std::string type_and_rest = column_info.substr(colon_pos + 1);

        bool attributes[3] = { false, false, false }; // {unique, autoincrement, key}

        size_t attr_start = type_and_rest.find('{');
        size_t attr_end = type_and_rest.find('}');
        std::string type_str = (attr_start != std::string::npos) ? type_and_rest.substr(0, attr_start) : type_and_rest;

        if (attr_start != std::string::npos && attr_end != std::string::npos && attr_end > attr_start) {
            std::string attributes_str = type_and_rest.substr(attr_start + 1, attr_end - attr_start - 1);
            parseAttributes(attributes_str, attributes);
        }

        size_t max_size = 0;
        size_t bracket_open = type_str.find('[');
        size_t bracket_close = type_str.find(']');
        if (bracket_open != std::string::npos && bracket_close != std::string::npos && bracket_close > bracket_open) {
            std::string size_str = type_str.substr(bracket_open + 1, bracket_close - bracket_open - 1);
            max_size = static_cast<size_t>(std::stoi(size_str));
            type_str = type_str.substr(0, bracket_open);
        }

        config::ColumnType column_type;
        if (type_str == "INT") {
            column_type = config::ColumnType::INT;
        } else if (type_str == "BOOL") {
            column_type = config::ColumnType::BOOL;
        } else if (type_str == "STRING") {
            column_type = config::ColumnType::STRING;
        } else {
            std::cerr << "Ошибка: Неизвестный тип столбца '" << type_str << "'." << std::endl;
            return {};
        }

        config::ColumnSchema column_schema;
        column_schema.name = column_name;
        column_schema.type = column_type;
        column_schema.max_size = max_size;
        std::copy(std::begin(attributes), std::end(attributes), std::begin(column_schema.attributes));

        schema.push_back(column_schema);
    }

    return schema;
}

std::vector<std::string> Table::parseCSVLine(const std::string& line) const
{
    std::vector<std::string> fields;
    std::string field;
    bool inside_quotes = false;

    for (size_t i = 0; i <= line.length(); ++i) {
        char c = (i < line.length()) ? line[i] : '\0';

        if (inside_quotes) {
            if (c == '"') {
                if (i + 1 < line.length() && line[i + 1] == '"') {
                    field += '"';
                    ++i;
                } else {
                    inside_quotes = false;
                }
            } else {
                field += c;
            }
        } else {
            if (c == '"') {
                inside_quotes = true;
            } else if (c == ',' || c == '\0') {
                fields.push_back(field);
                field.clear();
            } else {
                field += c;
            }
        }
    }

    return fields;
}

std::unordered_set<int> intersect(const std::vector<std::vector<int>>& vectors) {
    if (vectors.empty()) return {};

    std::unordered_set<int> result(vectors[0].begin(), vectors[0].end());

    for (size_t i = 1; i < vectors.size(); ++i) {
        std::unordered_set<int> temp;
        for (const auto &elem: vectors[i]) {
            if (result.count(elem)) {
                temp.insert(elem);
            }
        }
        result = std::move(temp);
    }
    return result;
}

    bool isNum(const std::string& str){
        if(str[0] != '+' && str[0] != '-' && (str[0]-'0' < 0 || str[0] - '9' > 0)){
            return false;
        }
        for(int i = 1; i < str.size(); ++i){
            if(str[i]-'0' < 0 || str[i] - '9' > 0){
                return false;
            }
        }
        return true;
    }

    bool isBool(const std::string& str){
        if(str == "true" || str == "false"){
            return true;
        }
        return false;
    }

    bool isStr(const std::string& str){
        if(str.size() > 1){
            if(str.front() == '\"' && str.back() == '\"'){
                return true;
            }
        }
        return false;
    }

    bool isBitString(std::string str){
        if(str.size() > 1){
            if(str.front() == '0' && std::tolower(str[1]) == 'x'){
                if(str.size() % 2 == 1){
                    std::cout << "error of byteString";
                }
                for(int i = 2 ; i < str.size(); ++i){
                    if(!((str[i] - '0' >= 0 || str[i] - '9' <= 0) || (std::tolower(str[i]) - 'a' < 0 || std::tolower(str[i]) - 'f' > 0))){
                        return false;
                    }
                }
                return true;
            }
        }
        return false;
    }

std::unordered_set<int> Table::record_index(const std::vector<std::vector<std::string>>& PCNF, bool& is_index) {
    std::vector<std::vector<int>> ans;
    if (PCNF.empty()) {
        is_index = false;
        return intersect(ans);
    }
    for (const auto &item: PCNF) {
        config::ColumnValue col_value;
        std::string col_name;
        bool is_first = true;
        for (int i = 0; i < 2; ++i) {
            std::string str = item[i];
            if (isNum(str)) {
                col_value = atoi(str.c_str());
            } else if (isStr(str)) {
                col_value = str.substr(1, str.size() - 2);
            } else if (isBool(str)) {
                col_value = bool(str == "true");
            } else if (isBitString(str)) {
                config::BitString new_stmt(str.size() - 2);
                for (int j = 2; j < str.size(); ++j) {
                    new_stmt[j - 2] = str[j];
                }
                col_value = new_stmt;
            } else {
                if (i == 1) {
                    is_first = false;
                }
                col_name = str;
            }
        }

        std::vector<int> num;
        if (item[2] == "=") {
            if(!indices_.contains(col_name)){
                continue;
            }
            auto [start_id, end_id] = indices_[col_name].equal_range(makeHashKey(col_value));
            if (start_id == end_id){
                continue;
            }
            for (; start_id != end_id; ++start_id) {
                num.push_back(start_id->second);
            }
        } else {
            if(!ordered_indices_.contains(col_name)){
                continue;
            }
            auto lower = ordered_indices_[col_name].begin();
            auto upper = ordered_indices_[col_name].end();
            if(lower == upper){
                continue;
            }
            if ( (item[2] == "<=" && is_first) || (item[2] == ">=" && !is_first) ) {
                upper = ordered_indices_[col_name].upper_bound(col_value);
            } else if ( (item[2] == "<=" && !is_first) || (item[2] == ">=" && is_first) ) {
                lower = ordered_indices_[col_name].lower_bound(col_value);
            } else if ( (item[2] == "<" && is_first) || (item[2] == ">" && !is_first) ){
                upper = ordered_indices_[col_name].lower_bound(col_value);
            } else if ( (item[2] == "<" && !is_first) || (item[2] == ">" && is_first) ) {
                lower = ordered_indices_[col_name].upper_bound(col_value);
            }
            for (; lower != upper; ++lower) {
                num.push_back(lower->second);
            }
        }
        ans.push_back(num);
    }
    is_index = !ans.empty();

    return intersect(ans);
}

}
