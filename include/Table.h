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

    Table(const std::vector<config::ColumnSchema>& columns)
        : schema_(columns) 
    {
        for (const auto& column : schema_) {
            indices_[column.name] = {};
        }
    }

    std::vector<std::string> findByCol(const std::string& column_name,
                            const config::ColumnValue& value) const;

    bool insertRecord(const std::unordered_map<std::string, std::string>& insert_values);
    // Метод для вставки без указания названий колонок
    bool insertRecord(const std::vector<std::string>& insert_values);

    inline const std::unordered_map<int, config::RowType>& getData() const { return data_; }

    bool saveToCSV(std::ofstream& ofs) const;
    bool loadFromCSV(std::istream& is);

private:
    std::string convertColumnValueToString(const config::ColumnValue& value) const;
    std::vector<std::string> parseCSVLine(const std::string& line) const;
    
    void indexRow(const int& id, const config::RowType& row);
    size_t makeHashKey(const config::ColumnValue& value) const;

    config::ColumnValue getDefaultValue(config::ColumnType column_type);
    bool convertValue(const std::string& value_str, const config::ColumnSchema& column_schema, config::ColumnValue& out_value);

    std::vector<config::ColumnSchema> schema_;
    std::unordered_map<int, config::RowType> data_;
    std::unordered_map<std::string,
        std::unordered_multimap<std::size_t, int>> indices_;
    int next_id_ = 0;
};
    
} // namespace memdb
