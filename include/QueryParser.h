#pragma once

#include <string>
#include <string_view>
#include <optional>
#include <unordered_map>

namespace memdb {
namespace parser {

enum class CommandType { SELECT, INSERT, UPDATE, DELETE, UNKNOWN };

class QueryParser {
public:
    QueryParser(const std::string_view& str);
    bool parse();

    CommandType getCommandType() const;
    std::optional<std::string_view> getTableName() const;
    const std::unordered_map<std::string_view, std::string_view>& getParameters() const;

private:
    bool parseCommand();
    bool parseTableName();
    bool parseParameters();

    std::string_view str_;
    CommandType command_type_;
    std::optional<std::string_view> table_name_;
    std::unordered_map<std::string_view, std::string_view> parameters_;
};

}
}
