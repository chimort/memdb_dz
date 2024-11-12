#pragma once

#include "Config.h"

#include <unordered_map>
#include <string>
#include <vector>
#include <tuple>
#include <variant>

namespace memdb
{

class Table{
public:

    Table(const std::vector<std::string>& column_names) : schema_(column_names) {
        for (const auto& columnNames : schema_) {
            indices_[columnNames] = {};
        }
    }

    void insert(const std::string& id, const config::RowType& row_data);
    std::vector<std::string> findByCol(const std::string& column_name,
                            const config::ColumnValue& value) const;

    bool insertRecord(const std::unordered_map<std::string, std::string>& insert_values);
    
    void printAllRecords() const;

private:
    void indexRow(const std::string& id, const config::RowType& row);
    std::size_t getKey(const config::ColumnValue& value) const;
    void printValue(const config::ColumnValue& value) const;

    config::ColumnValue getDefaultValue();
    bool convertValue(const std::string& value_str, config::ColumnValue& out_value);

    std::vector<std::string> schema_;
    std::unordered_map<std::string, config::RowType> data_;
    std::unordered_map<std::string,
        std::unordered_multimap<std::size_t, const config::RowType*>> indices_;

    size_t next_id_ = 1;
};
    
} // namespace memdb
