#include "Table.h"

#include <functional>

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


}
