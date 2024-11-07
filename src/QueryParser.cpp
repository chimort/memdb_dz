#include "QueryParser.h"

#include <algorithm>
#include <cctype>

namespace memdb
{
namespace parser
{

std::string toLowerCase(const std::string_view& str)
{
    std::string lower_str(str.begin(), str.end());
    std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return lower_str;
}

QueryParser::QueryParser(const std::string_view& str) 
    : str_(str), command_type_(CommandType::UNKNOWN) {}

bool QueryParser::parse() 
{
    if (!parseCommand()) {
        return false;
    }

    if (!parseTableName()) {
        return false;
    }

    if (!parseParameters()) {
        return false;
    }

    return true;
}

bool QueryParser::parseCommand()
{
    str_.remove_prefix(std::min(str_.find_first_not_of(" \t\n"), str_.size()));

    size_t pos = str_.find_first_of(" \t\n;");
    if (pos == std::string_view::npos) {
        pos = str_.size();
    }

    std::string_view command_word = str_.substr(0, pos);
    std::string command_lower = toLowerCase(command_word);

    static const std::unordered_map<std::string, CommandType> commandMap = {
        {"select", CommandType::SELECT},
        {"insert", CommandType::INSERT},
        {"update", CommandType::UPDATE},
        {"delete", CommandType::DELETE}
    };

    auto it = commandMap.find(command_lower);
    if (it != commandMap.end()) {
        command_type_ = it->second;

        str_.remove_prefix(pos);
        str_.remove_prefix(std::min(str_.find_first_not_of(" \t\n"), str_.size()));

        return true;
    } else {
        command_type_ = CommandType::UNKNOWN;
        return false;
    }
}

bool QueryParser::parseTableName()
{
    // тут вроде просто 
    return false;
}

bool QueryParser::parseParameters()
{
    // тут нужно с видом команд побадаться
    return false;
}

CommandType QueryParser::getCommandType() const 
{
    return command_type_;
}

std::optional<std::string_view> QueryParser::getTableName() const 
{
    return table_name_;
}

const std::unordered_map<std::string_view, 
    std::string_view>& QueryParser::getParameters() const 
{
    return parameters_;
}

}
}

