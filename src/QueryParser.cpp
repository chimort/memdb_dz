#include "QueryParser.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>

#include <iostream>

namespace memdb
{
namespace parser
{

bool QueryParser::parse()
{   

    size_t start_pos = str_.find_first_not_of(' ');
    if (start_pos == std::string::npos) {
        return false;
    }
    str_ = str_.substr(start_pos);


    if (str_.compare(0, 6, "insert") == 0) {
        command_type_ = CommandType::INSERT;
        str_ = str_.substr(6); 
        insertParse();
    } else if (str_.compare(0, 6, "select") == 0) {
        command_type_ = CommandType::SELECT;
        str_ = str_.substr(7);
        selectParse();
    } else if (str_.compare(0, 6, "delete") == 0) {
        command_type_ = CommandType::DELETE;
        str_ = str_.substr(7);
        deleteParse();
    } else if (str_.compare(0, 6, "update") == 0) {
        command_type_ = CommandType::UPDATE;
        str_ = str_.substr(7); 
        updateParse();
    }

    return true;
}

bool QueryParser::createParse()
{
    return false;
}


std::vector<std::string> QueryParser::splitByComma(const std::string& str) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, ',')) {
        token.erase(token.begin(), std::find_if(token.begin(), token.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
        token.erase(std::find_if(token.rbegin(), token.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), token.end());
        if (token.size() >= 1) {
            result.push_back(token);
        }
    }
    
    return result;
}

bool QueryParser::insertParse() {
    size_t open_paren_pos = str_.find('(');
    size_t close_paren_pos = str_.find(')');

    if (open_paren_pos == std::string::npos || close_paren_pos == std::string::npos) {
        return false;
    }

    std::string value_str = str_.substr(open_paren_pos + 1, close_paren_pos - open_paren_pos - 1);

    std::vector<std::string> tokens = splitByComma(value_str);
    int i = 0;
    for (const auto& token : tokens) {
        size_t eq_pos = token.find('=');
        if (eq_pos == std::string::npos) {
            insert_values_[std::to_string(i++)] = token;
        } else {
            insert_values_[token.substr(0, eq_pos - 1)] = token.substr(eq_pos + 2); 
        }
    }

    size_t to_pos = str_.find("to");
    if (to_pos == std::string::npos) {
        return false;
    }
    table_name_ = str_.substr(to_pos + 3);
    return true;
}

bool QueryParser::selectParse()
{   
    size_t from_pos = str_.find("from");
    if (from_pos == std::string::npos) {
        return false;
    }
    std::string values_str = str_.substr(0, from_pos);

    std::vector<std::string> tokens = splitByComma(values_str);
    for (const auto& token : tokens) {
        selected_columns_.push_back(token);
    }

    for (auto i : selected_columns_) {
        std::cout << i << std::endl;
    }

    size_t where_pos = str_.find("where");
    if (where_pos == std::string::npos) {
        return false;
    }

    table_name_ = str_.substr(from_pos + 5, where_pos - from_pos - 6);
    condition_ = str_.substr(where_pos + 6);

    return true;
}

bool QueryParser::deleteParse()
{   
    size_t where_pos = str_.find("where");
    if (where_pos == std::string::npos) {
        return false;
    }
    table_name_ = str_.substr(0, where_pos - 1);
    std::cout << table_name_ << std::endl;
    condition_ = str_.substr(where_pos + 6);
    std::cout << condition_ << std::endl;
    return true;
}

bool QueryParser::updateParse()
{
    size_t set_pos = str_.find("set");
    if (set_pos == std::string::npos) {
        return false;
    }
    table_name_ = str_.substr(0, set_pos - 1);
    std::cout << table_name_ << std::endl;
    size_t where_pos = str_.find("where");
    std::string conditions_str = str_.substr(set_pos + 4, where_pos - set_pos - 5);
    std::cout << conditions_str << std::endl;

    std::vector<std::string> tokens = splitByComma(conditions_str);
    for (const auto& token : tokens) {
        size_t eq_pos = token.find('=');
        update_values_[token.substr(0, eq_pos - 1)] = token.substr(eq_pos + 2); 
    }

    condition_ = str_.substr(where_pos + 6);
    std::cout << condition_ << std::endl;
    return true;
}

} // namespace parser
} // namespace memdb


