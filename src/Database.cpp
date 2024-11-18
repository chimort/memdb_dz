#include "Database.h"
#include "QueryParser.h"

#include <iostream>

namespace memdb
{

Database& Database::getInstance()
{
    static Database instance;
    return instance;
}

bool Database::loadFromFile(std::ifstream)
{
    return false;
}

bool Database::saveToFile(std::ofstream)
{
    return false;
}

std::unique_ptr<IResponse> Database::execute(const std::string_view &str)
{
    std::string query = {str.begin(), str.end()};
    parser::QueryParser parser(query);
    auto response = std::make_unique<Response>();

    if (!parser.parse()) {
        response->setStatus(false);
        response->setMessage("Failed to parse query");
        return response;
    }

    parser::CommandType command_type = parser.getCommandName();

    switch (command_type) {
        case parser::CommandType::INSERT: {
            auto table_name_opt = parser.getTableName();
            const auto& insert_values = parser.getInsertValues();

            const std::string table_name = table_name_opt;

            auto table_it = tables_.find(table_name);
            if (table_it == tables_.end()) {
                response->setStatus(false);
                response->setMessage("Table not found for insert");
                return response;
            }
            // Вот тут надо будет подшаманить, потому что у нас два метода
            // Потому что у нас 2 метода ставки, с жестким указанием (age = 234) и просто значения
            // В один из них передаем вектор строк, а в другой словарик
            auto& table = table_it->second;
            bool success = table->insertRecord(insert_values);
            if (success) {
                response->setStatus(true);
                response->setMessage("Record inserted successfully");
            } else {
                response->setStatus(false);
                response->setMessage("Failed to insert record");
            }

            break;
        }
        // Handle other command types if needed
        default: {
            response->setStatus(false);
            response->setMessage("Unknown or unhandled command");
            break;
        }
    }

    return response;
}


std::shared_ptr<Table> Database::getTable(const std::string& table_name) {
    auto it = tables_.find(table_name);
    if (it != tables_.end()) {
        return it->second;
    }
    return nullptr;
}

} //namespace memdb
