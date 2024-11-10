#include "QueryParser.h"

#include <algorithm>
#include <cctype>
#include <sstream>

#include <iostream>

namespace memdb
{
namespace parser
{

bool QueryParser::parse()
{   
    size_t insert_pos = str_.find("insert");
    if (insert_pos == std::string::npos) {
        return false; // тут убрать потом надо это
    } else {
        std::cout << "Insert was recognized" << std::endl;
        command_type_ = CommandType::INSERT;
        str_ = str_.substr(insert_pos + 6);
        insertParse();
    }

    return true;
}

bool QueryParser::createParse()
{
    return false;
}

std::string QueryParser::trim(const std::string& str)
{
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_first_not_of(' ');
    return str.substr(first, last - first + 1);
}

bool QueryParser::insertParse()
{
    // скажем, что мы уже находимся на открывающей скобки, далее нужно в тупую пройтись по всем элементам
    // после закрывающей скобки будет to и назавине таблицы, нужно их просто распарсить и вернуть true/false

    size_t open_paren_pos = str_.find('(');
    size_t close_paren_pos = str_.find(')');

    if (open_paren_pos == std::string_view::npos || close_paren_pos == std::string_view::npos) {
        return false;
    }

    std::string value_str = str_.substr(open_paren_pos + 1, close_paren_pos - open_paren_pos - 1);
    value_str = trim(value_str);

    std::istringstream value_stream(value_str);
    std::string pair;

    while (std::getline(value_stream, pair, ',')) {  // ВОТ ТУТ КАКОЕ-ТО УСЛОВИЕ ЕБАНОЕ
        std::cout << "here" << std::endl; 
        pair = trim(pair);
        size_t equal_pos = pair.find('=');
        if (equal_pos != std::string::npos) {
            std::string key = trim(pair.substr(0, equal_pos));
            std::string value = trim(pair.substr(equal_pos + 1));
            insert_values_[key] = value;
            std::cout << key << " = " << value << std::endl;
        } else {
            insert_values_["value"] = pair;
        }
    }

    size_t to_pos = str_.find("to");
    if (to_pos == std::string::npos) {
        return false;
    }

    std::string table_name_ = trim(str_.substr(to_pos + 2));
    if (table_name_.empty()) {
        return false;
    }

    return true;
}

} // namespace parser
} // namespace memdb


