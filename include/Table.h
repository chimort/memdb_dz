#pragma once

#include "Config.h"

#include <unordered_map>
#include <unordered_set>
#include <map>
#include <string>
#include <vector>
#include <tuple>
#include <variant>
#include <set>

namespace memdb
{

class Table{
public:

        Table(const std::vector<config::ColumnSchema>& columns)
        : schema_(columns)
    {
        for (auto& column : schema_) {
            if (column.attributes[1]) { // autoincrement
                autoincrement_counters_[column.name] = {};
            }
            if (column.attributes[0] || column.attributes[2]) {
                unique_null_value_[column.name] = false;
            }
            if (column.attributes[2]) {
                indices_[column.name] = {};
                column.ordering.unordered = true;
                has_index_ = true;
            }
        }
    }

    bool insertRecord(const std::unordered_map<std::string, std::string>& insert_values);
    // Метод для вставки без указания названий колонок
    bool insertRecord(const std::vector<std::string>& insert_values);
    bool deleteRow(const int& row_id);
    void updateIndices(const int& row_id, const config::RowType& new_row);

    bool updateRowType(int record_id, const config::RowType& new_row);
    void removeFromIndices(const int& row_id);

    bool insertRowType(const config::RowType& insert_values);
    bool createIndex(const std::vector<std::string>& columns_name, config::IndexType index_type);
    void insertIndices(const int& id, const config::RowType& row);

    inline const std::unordered_map<int, config::RowType>& getData() const { return data_; }
    inline const std::vector<config::ColumnSchema>& getSchema() const { return schema_; }

    bool saveToCSV(std::ofstream& ofs) const;
    bool loadFromCSV(std::istream& is);

    std::unordered_set<int> record_index(const std::vector<std::vector<std::string>>& PCNF, bool&);
private:


    std::string convertColumnValueToString(const config::ColumnValue& value) const;
    std::vector<std::string> parseCSVLine(const std::string& line) const;

    size_t makeHashKey(const config::ColumnValue& value) const;
    void indexRow(const int& id, const config::RowType& row);

    bool convertValue(const std::string& value_str, const config::ColumnSchema& column_schema, config::ColumnValue& out_value);

    int findMinUnusedId(const std::string& column_name);

    std::vector<config::ColumnSchema> schema_;
    std::unordered_map<int, config::RowType> data_;

    std::unordered_map<std::string,
        std::unordered_multimap<std::size_t, int>> indices_;
    std::unordered_map<std::string, std::multimap<config::ColumnValue, int>> ordered_indices_;

    std::unordered_map<std::string, std::set<int>> autoincrement_counters_;
    std::unordered_map<std::string, bool> unique_null_value_;
    
    
    int next_id_ = 0;
    bool has_index_ = false;
};
    
} // namespace memdb
