#include "Table.h"
#include "Config.h"

#include <fstream>
#include <string>

namespace memdb 
{

bool Table::insertRecord(const std::unordered_map<std::string, std::string>& insert_values) {
    return true;
}

bool Table::convertValue(const std::string& value_str, config::ColumnValue& out_value) {
    return true;
}

config::ColumnValue getDefaultValue() {
    return ""; // Заглушка, тут нужно будет какую-то проверку по типам сделать
}

void Table::indexRow(const int& id, const config::RowType& row) {
    for (const auto& column: schema_) {
        auto value = row.at(column);
        size_t hash_value = makeHashKey(value);
        indices_[column].emplace(hash_value, id);
    }
}

size_t Table::makeHashKey(const config::ColumnValue &value) const
{
    return std::visit([](const auto& val) -> size_t {
        if constexpr (std::is_same_v<std::decay_t<decltype(val)>, config::BitString>) {
            size_t hash_value = 0;
            for (uint8_t byte : val) {
                hash_value = hash_value * 31 + std::hash<uint8_t>{}(byte);
            }
            return hash_value;
        } else {
            return std::hash<std::decay_t<decltype(val)>>{}(val);
        }
    }, value);
}


void Table::printAllRecords() const {
}

bool Table::saveToCSV(std::ofstream& ofs) const
{
    for (size_t i = 0; i < schema_.size(); ++i) {
        ofs << schema_[i];
        if (i != schema_.size() - 1)
            ofs << ',';
    }
    ofs << '\n';

    for (const auto& [id, row] : data_) {
        for (size_t i = 0; i < schema_.size(); ++i) {
            const auto& column_name = schema_[i];
            auto it = row.find(column_name);
            if (it != row.end()) {
                const std::string value_str = convertColumnValueToString(it->second);
                ofs << value_str;
            } else {
                ofs << "";
            }

            if (i != schema_.size() - 1)
                ofs << ',';
        }
        ofs << '\n';
    }

    return true;
}

std::string Table::convertColumnValueToString(const config::ColumnValue& value) const
{
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
            return '"' + escaped + '"';
        } else if constexpr (std::is_same_v<T, bool>) {
            return val ? "true" : "false";
        } else {
            return "";
        }
    }, value);
}


}
