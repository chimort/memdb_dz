#include "QueryParser.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>
#include <regex>

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

    if (str_.length() < 6) {
        return false;
    }
    std::transform(str_.begin(), str_.begin() + 6, str_.begin(), [](unsigned char c) { return std::tolower(c); });

    if (str_.compare(0, 6, "insert") == 0) {
        command_type_ = CommandType::INSERT;
        str_ = str_.substr(6); 
        return insertParse();
    } else if (str_.compare(0, 6, "select") == 0) {
        command_type_ = CommandType::SELECT;
        str_ = str_.substr(7);
        return selectParse();
    } else if (str_.compare(0, 6, "delete") == 0) {
        command_type_ = CommandType::DELETE;
        str_ = str_.substr(7);
        return deleteParse();
    } else if (str_.compare(0, 6, "update") == 0) {
        command_type_ = CommandType::UPDATE;
        str_ = str_.substr(7); 
        return updateParse();
    } else if (str_.compare(0, 6, "create") == 0) {
        if (str_.length() < 12) {
            return false;
        }
        std::transform(str_.begin() + 6, str_.begin() + 12, str_.begin() + 6, [](unsigned char c) { return std::tolower(c); });
        if (str_.compare(0, 12, "create table") == 0){
            command_type_ = CommandType::CREATE_TABLE;
            return createTableParse();
        } else {
            command_type_ = CommandType::CREATE_INDEX;
            return createIndexParse();
        }
    } else {
        command_type_ = CommandType::UNKNOWN;
        return false;
    }

    return true;
}


ssize_t regularSearchWhere(const std::string& str){
    static const std::regex pattern(R"(\b[wW][hH][eE][rR][eE]\b)");
    std::smatch match;
    if (!std::regex_search(str, match, pattern)) {
        return -1;
    }

    return match.position(0);
}

ssize_t regularSearchOn(const std::string& str){
    static const std::regex pattern(R"(\b[oO][nN]\b)");
    std::smatch match;
    if (!std::regex_search(str, match, pattern)) {
        return -1;
    }

    return match.position(0);
}

ssize_t regularSearchBy(const std::string& str){
    static const std::regex pattern(R"(\b[bB][yY]\b)");
    std::smatch match;
    if (!std::regex_search(str, match, pattern)) {
        return -1;
    }

    return match.position(0);
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

bool QueryParser::createIndexParse() {
    std::smatch match;

    std::regex index_regex(R"(^\s*create\s+(ordered|unordered)\s+index\s+on\s+(\w+)\s+by\s+([\w\s,]+)\s*$)", std::regex_constants::icase);

    if (!std::regex_match(str_, match, index_regex)) {
        throw std::invalid_argument("Invalid create index syntax");
    }

    // Извлекаем данные из строки
    std::string index_type_str = match[1];  // Тип индекса
    table_name_ = match[2];                 // Имя таблицы

    // Определяем тип индекса
    config::IndexType index_type;
    if (index_type_str == "ordered") {
        index_type = config::IndexType::ORDERED;
    } else if (index_type_str == "unordered") {
        index_type = config::IndexType::UNORDERED;
    } else {
        throw std::invalid_argument("Invalid index type");
    }

    // Разбор колонок
    std::string columns_str = match[3];
    std::regex column_split_regex(R"(\s*,\s*)");
    std::sregex_token_iterator it(columns_str.begin(), columns_str.end(), column_split_regex, -1);
    std::sregex_token_iterator end;

    while (it != end) {
        std::string column = *it++;
        if (!column.empty()) {
            column_index_type_[column] = index_type;
        }
    }

    return true;
}


bool QueryParser::createTableParse() {
    std::smatch match;

    // Регулярное выражение для заголовка таблицы
    std::regex table_regex(R"(^\s*create\s+table\s+(\w+)\s*\(([\s\S]*)\)\s*$)", std::regex_constants::icase);
    if (!std::regex_match(str_, match, table_regex)) {
        throw std::invalid_argument("Invalid create table syntax");
    }

    table_name_ = match[1]; // Имя таблицы
    std::string columns_str = match[2]; // Строка с описанием колонок

    // Регулярное выражение для разбора колонок
    std::regex column_regex(R"((?:\{\s*([^}]*)\s*\}\s*)?(\w+)\s*:\s*(\w+)(\[\d+\])?\s*(?:=\s*(".*?"|[^,]*))?)");

    std::sregex_iterator it(columns_str.begin(), columns_str.end(), column_regex);
    std::sregex_iterator end;

    while (it != end) {
        config::ColumnSchema params;
        params.max_size = 0; // Явно обнуляем значение max_size для текущей колонки

        // Атрибуты (если есть)
        if ((*it)[1].matched) {
            std::string attributes = (*it)[1];
            std::regex attr_split_regex(R"(\s*,\s*)");
            std::sregex_token_iterator attr_it(attributes.begin(), attributes.end(), attr_split_regex, -1);
            std::sregex_token_iterator attr_end;
            while (attr_it != attr_end) {
                std::string attribute = *attr_it++;
                if (!attribute.empty()) {
                    if (attribute == "key") {
                        params.attributes[2] = 1;
                    } else if (attribute == "autoincrement") {
                        params.attributes[1] = 1;
                    } else if (attribute == "unique") {
                        params.attributes[0] = 1;
                    }
                }
            }
        }

        // Имя, тип, размер массива и значение по умолчанию
        params.name = (*it)[2];
        std::string type = (*it)[3];

        // Определение типа
        if (type == "bool") {
            params.type = config::ColumnType::BOOL;
        } else if (type == "int32") {
            params.type = config::ColumnType::INT;
        } else if (type == "string") {
            params.type = config::ColumnType::STRING;
        } else if (type == "bytes") {
            params.type = config::ColumnType::BITSTRING;
        } else {
            throw std::invalid_argument("Unknown column type: " + type);
        }

        // Проверка на наличие размера массива
        if ((*it)[4].matched) {
            std::string size_str = (*it)[4].str(); // Извлекаем строку вида "[32]"
            size_str = size_str.substr(1, size_str.size() - 2); // Убираем скобки
            params.max_size = std::stoi(size_str); // Конвертируем в число
        }

        // Значение по умолчанию (если есть)
        if ((*it)[5].matched) {
            if (params.attributes[0] == 1){
                return false;
            }

            params.default_value = (*it)[5].str();
            // Удаляем кавычки, если значение обрамлено ими
            if (!params.default_value.empty() && params.default_value.front() == '"' && params.default_value.back() == '"') {
                params.default_value = params.default_value.substr(1, params.default_value.size() - 2);
            }
        }


        columns_parametrs_.push_back(params);
        ++it;
    }

    return true;
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

    std::string take_name = str_.substr(close_paren_pos);
    static const std::regex pattern(R"(\b[tT][oO]\b)"); //Ищет вариации to (\b значит целым словом)
    std::smatch match;
    if (!std::regex_search(take_name, match, pattern)) {
        return false;
    }

    table_name_ = take_name.substr(match.position(0) + 3);
    return true;
}

bool QueryParser::selectParse()
{
    static const std::regex pattern(R"(\b[fF][rR][oO][mM]\b)");
    std::smatch match;
    if (!std::regex_search(str_, match, pattern)) {
        return false;
    }
    size_t from_pos = match.position(0);

    std::string values_str = str_.substr(0, from_pos);

    std::vector<std::string> tokens = splitByComma(values_str);
    for (const auto& token : tokens) {
        selected_columns_.push_back(token);
    }


    ssize_t where_pos = regularSearchWhere(str_);
    if (where_pos == -1){
        return false;
    }

    table_name_ = str_.substr(from_pos + 5, where_pos - from_pos - 6);
    condition_ = str_.substr(where_pos + 6);

    return true;
}

bool QueryParser::deleteParse()
{   
    ssize_t where_pos = regularSearchWhere(str_);
    if (where_pos == -1){
        return false;
    }

    table_name_ = str_.substr(0, where_pos - 1);
    condition_ = str_.substr(where_pos + 6);
    return true;
}

bool QueryParser::updateParse()
{
    static const std::regex pattern(R"(\b[sS][eE][tT]\b)");
    std::smatch match;
    if (!std::regex_search(str_, match, pattern)) {
        return false;
    }
    size_t set_pos = match.position(0);

    table_name_ = str_.substr(0, set_pos - 1);
    ssize_t where_pos = regularSearchWhere(str_);
    if (where_pos == -1){
        return false;
    }
    std::string conditions_str = str_.substr(set_pos + 4, where_pos - set_pos - 5);

    std::vector<std::string> tokens = splitByComma(conditions_str);
    for (const auto& token : tokens) {
        size_t eq_pos = token.find('=');
        update_values_[token.substr(0, eq_pos - 1)] = token.substr(eq_pos + 2); 
    }

    condition_ = str_.substr(where_pos + 6);
    return true;
}

} // namespace parser
} // namespace memdb


