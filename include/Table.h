#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <tuple>
#include <variant>

namespace memdb
{
class TableBase {
public:
    virtual ~TableBase() = default;
    virtual void display() const = 0;
};


class Table : public TableBase {
public:
    using ColumnValue = std::variant<int, std::string, bool, std::vector<uint8_t>>; 
    using RowType = std::unordered_map<std::string, ColumnValue>;

    Table(const std::vector<std::string>& column_names) : schema_(column_names) {
        for (const auto& columnNames : schema_) {
            indices_[columnNames] = {};
        }
    }

    void insert(const std::string& id, const RowType& row_data);
    std::vector<std::string> findByCol(const std::string& column_name,
                            const ColumnValue& value) const;
    void display() const override;

private:
    void indexRow(const std::string& id, const RowType& row);
    std::size_t getKey(const ColumnValue& value) const;
    void printValue(const ColumnValue& value) const;

    std::vector<std::string> schema_;
    std::unordered_map<std::string, RowType> data_;
    std::unordered_map<std::string,
        std::unordered_multimap<std::size_t, std::string>> indices_;
};
    
} // namespace memdb
