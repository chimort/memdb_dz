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
    inline std::unordered_map<std::string, std::optional<config::ColumnValue>> getValues() { return values_; }
    inline std::unordered_map<std::string, std::string> getInsertValues() { return insert_values_; }

private:
    std::string str_;
    std::string table_name_;
    CommandType command_type_;
    std::unordered_map<std::string, std::optional<config::ColumnValue>> values_;
    std::unordered_map<std::string, std::string> insert_values_;

    bool commandParser();
    bool createParse();
    bool insertParse();

    std::string trim(const std::string& str);
};

}
}
