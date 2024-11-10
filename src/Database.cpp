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

Response Database::execute(const std::string_view &str)
{
    std::string query = {str.begin(), str.end()};
    parser::QueryParser parser(query);
    Response response;

    if (!parser.parse()) {
        response.setStatus(false);
        response.setMessage("Failed to parse query");
        return response;
    }

    parser::CommandType command_type = parser.getCommandName();

    switch (command_type) {
        case parser::CommandType::INSERT: {
            // Extract table name and insert values from the parser
            auto table_name_opt = parser.getTableName();
            const auto& insert_values = parser.getInsertValues();


            const std::string table_name = table_name_opt;

            // Output the table name and insert values
            std::cout << "Table Name: " << table_name << std::endl;
            std::cout << "Insert Values:" << std::endl;
            for (const auto& [key, value] : insert_values) {
                std::cout << key << " = " << value << std::endl;
            }

            // Implement the logic to insert the values into the table
            auto table_it = tables_.find(table_name);
            if (table_it == tables_.end()) {
                response.setStatus(false);
                response.setMessage("Table not found for insert");
                return response;
            }

            // auto& table = table_it->second;
            // bool success = table->insertRecord(insert_values);
            // if (success) {
            //     response.setStatus(true);
            //     response.setMessage("Record inserted successfully");
            // } else {
            //     response.setStatus(false);
            //     response.setMessage("Failed to insert record");
            // }

            break;
        }
        // Handle other command types if needed
        default: {
            response.setStatus(false);
            response.setMessage("Unknown or unhandled command");
            break;
        }
    }

    return response;

    // parser::CommandType command_type = parser.getCommandType();
    // auto table_name_opt = parser.getTableName();
    // const auto& parameters = parser.getParameters();

    // if (!table_name_opt.has_value()) {
    //     response.setStatus(false);
    //     response.setMessage("Table name is missing");
    //     return response;
    // }

    // const std::string& table_name = std::string(table_name_opt.value());
    // auto table_it = tables_.find(table_name);

    // switch (command_type) {
    //     case parser::CommandType::SELECT: {
    //         if (table_it != tables_.end()) {
    //             auto &table = table_it->second;
    //             //дальше логика для select
    //             response.setStatus(true);
    //             response.serData(selectedRecords); // что-то такое можно
    //         } else {
    //             response.serStatus(false);
    //             response.setMessage("Table not found");
    //         }
    //         break;
    //     }
    //     case parser::CommandType::INSERT: {
    //         if (table_it != tables_.end()) {
    //             auto& table = table_it->second;
    //             // Implement INSERT logic
    //             // Insert the provided parameters as a new record
    //             bool success = table->insertRecord(parameters);
    //             if (success) {
    //                 response.setStatus(true);
    //                 response.setMessage("Record inserted successfully");
    //             } else {
    //                 response.setStatus(false);
    //                 response.setMessage("Failed to insert record");
    //             }
    //         } else {
    //             response.setStatus(false);
    //             response.setMessage("Table not found for INSERT");
    //         }
    //         break;
    //     }
    //     case parser::CommandType::UPDATE: {
    //         if (table_it != tables_.end()) {
    //             auto& table = table_it->second;
    //             // Implement UPDATE logic
    //             // Update records matching certain criteria
    //             bool success = table->updateRecords(parameters);
    //             if (success) {
    //                 response.setStatus(true);
    //                 response.setMessage("Records updated successfully");
    //             } else {
    //                 response.setStatus(false);
    //                 response.setMessage("Failed to update records");
    //             }
    //         } else {
    //             response.setStatus(false);
    //             response.setMessage("Table not found for UPDATE");
    //         }
    //         break;
    //     }
    //     case parser::CommandType::DELETE: {
    //         if (table_it != tables_.end()) {
    //             auto& table = table_it->second;
    //             // Implement DELETE logic
    //             // Delete records matching certain criteria
    //             bool success = table->deleteRecords(parameters);
    //             if (success) {
    //                 response.setStatus(true);
    //                 response.setMessage("Records deleted successfully");
    //             } else {
    //                 response.setStatus(false);
    //                 response.setMessage("Failed to delete records");
    //             }
    //         } else {
    //             response.setStatus(false);
    //             response.setMessage("Table not found for DELETE");
    //         }
    //         break;
    //     }
    //     default: {
    //         response.setStatus(false);
    //         response.setMessage("Unknown command");
    //         break;
    //     }
    // }

    // return response;
}

} //namespace memdb
