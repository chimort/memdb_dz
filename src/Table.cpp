#include "Table.h"

#include <iostream>

namespace memdb 
{

bool Table::insertRecord(const std::unordered_map<std::string, std::string>& insert_values) {
    return true;
}

bool Table::convertValue(const std::string& value_str, config::ColumnValue& out_value) {
    return true;
}

config::ColumnValue getDeafaultValue() {
    return ""; // Заглушка, тут нужно будет какую-то проверку по типам сделать
}

void Table::indexRow(const std::string& id, const config::RowType& row) {
}

void Table::printAllRecords() const {
}


}
