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
    inline std::string getSetCondition() const { return set_condition_; }
    inline std::vector<std::string> getSelectedCol() const { return selected_columns_; }
    inline std::unordered_map<std::string, std::optional<config::ColumnValue>> getValues() const { return values_; }
    inline std::unordered_map<std::string, std::string> getInsertValues() const { return insert_values_; }

private:
    std::string str_;
    std::string table_name_;
    std::string condition_; // where condition
    std::string set_condition_;
    CommandType command_type_;
    std::vector<std::string> selected_columns_;
    std::unordered_map<std::string, std::optional<config::ColumnValue>> values_;
    std::unordered_map<std::string, std::string> insert_values_;
    std::unordered_map<std::string, std::string> update_values_;
    
    bool createTableParse();
    bool insertParse();
    bool selectParse();
    bool deleteParse();
    bool updateParse();

    std::vector<std::string> splitByComma(const std::string &str);

};

}
}
