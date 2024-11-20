#pragma once

#include "Config.h"

#include <string>
#include <string_view>
#include <optional>
#include <vector>
#include <unordered_map>
#include <optional>

namespace memdb {
namespace parser {

enum class CommandType { 
    CREATE_TABLE,
    SELECT, 
    INSERT, 
    UPDATE, 
    DELETE, 
    JOIN, 
    CREATE_INDEX, 
    UNKNOWN 
};
enum class IndexType { ORDERED, UNORDERED };

class QueryParser {
public:
    QueryParser(const std::string& str) : str_(str) {}
    bool parse();

    inline std::string getTableName() const { return table_name_; }
    inline CommandType getCommandName() const { return command_type_; }
    inline std::string getCondition() const { return condition_; }
    inline std::vector<std::string> getSelectedCol() const { return selected_columns_; }
    inline std::unordered_map<std::string, std::optional<config::ColumnValue>> getValues() const { return values_; }
    inline std::unordered_map<std::string, std::string> getInsertValues() const { return insert_values_; }
    std::unordered_map<std::string, std::string> getUpdateValues() const { return update_values_; }
    inline std::vector<config::ColumnSchema> getCreateTableParametrs() const { return columns_parametrs_; };
    inline std::unordered_map<std::string, IndexType> getCreateIndexType() const { return column_index_type_; };

private:
    std::string str_;
    std::string table_name_;
    std::string condition_; // where condition
    CommandType command_type_;
    std::vector<std::string> selected_columns_;
    std::unordered_map<std::string, std::optional<config::ColumnValue>> values_;
    std::unordered_map<std::string, std::string> insert_values_;
    std::unordered_map<std::string, std::string> update_values_;
    std::vector<config::ColumnSchema> columns_parametrs_;
    std::unordered_map<std::string, IndexType> column_index_type_;
    
    bool createTableParse();
    bool createIndexParse();
    bool insertParse();
    bool selectParse();
    bool deleteParse();
    bool updateParse();

    std::vector<std::string> splitByComma(const std::string &str);

};

}
}
