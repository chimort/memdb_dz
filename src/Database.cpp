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

bool Database::loadFromFile(const std::string& filename, const std::string& table_name)
{
    auto it = tables_.find(table_name);
    if (it == tables_.end()) {
        std::cerr << "Table '" << table_name << "' not found." << std::endl;
        return false;
    }

    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        std::cerr << "Failed to open file for reading: " << filename << std::endl;
        return false;
    }

    if (!it->second->loadFromCSV(ifs)) {
        std::cerr << "Failed to load table '" << table_name << "' from CSV." << std::endl;
        return false;
    }

    return true;
}

bool Database::saveToFile(const std::string& filename, const std::string& table_name) const
{
    auto it = tables_.find(table_name);
    if (it == tables_.end()) {
        std::cerr << "Table '" << table_name << "' not found." << std::endl;
        return false;
    }

    std::ofstream ofs(filename);
    if (!ofs.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return false;
    }

    if (!it->second->saveToCSV(ofs)) {
        std::cerr << "Failed to save table '" << table_name << "' to CSV." << std::endl;
        return false;
    }

    return true;
}

std::unique_ptr<Response> Database::execute(const std::string_view &str)
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

            auto& table = table_it->second;
            bool success;

            std::vector<config::ColumnSchema> columns = table->getSchema();
            auto it = insert_values.find("1");
            if (it != insert_values.end()){
                std::vector<std::string> insert2(insert_values.size());
                for (auto& pair : insert_values){
                    if (pair.second != ""){
                        insert2[std::stoi(pair.first)] = pair.second;
                    }
                }

                
                success = table->insertRecord(insert2);
            } else {
                success = table->insertRecord(insert_values);
            }

            if (success) {
                response->setStatus(true);
                response->setMessage("Record inserted successfully");
            } else {
                response->setStatus(false);
                response->setMessage("Failed to insert record");
            }

            break;
        } case parser::CommandType::CREATE_TABLE: {
            std::shared_ptr<Table> table = std::make_shared<Table>(parser.getCreateTableParametrs());
            if (table == nullptr) {
                response->setStatus(false);
                response->setMessage("Failed create table");                
            }

            tables_[parser.getTableName()] = table;
            if (tables_.find(parser.getTableName()) == tables_.end()){
                response->setStatus(false);
                response->setMessage("Failed load table in db");  
            } else {
                response->setStatus(true);
                response->setMessage("Table created successfully");                 
            }
            
            break;
        } case parser::CommandType::CREATE_INDEX: {
            auto table_name_opt = parser.getTableName();

            const std::string table_name = table_name_opt;

            auto table_it = tables_.find(table_name);
            if (table_it == tables_.end()) {
                response->setStatus(false);
                response->setMessage("Table not found for insert");
                return response;
            }

            auto& table = table_it->second;
            bool success;

            auto queryIndexs = parser.getCreateIndexType();
            std::vector<config::ColumnSchema> columns = table->getSchema();
            std::vector<std::string> columns_name;
            bool flag = false;
            for (auto& column : columns) {
                for (auto& [name_column_query, queryType] : queryIndexs) {
                    if (column.name == name_column_query) {
                        columns_name.push_back(column.name);
                        column.ordering = queryType;
                        flag = true;
                    }
                }
            }


            if (!flag) {
                response->setStatus(false);
                response->setMessage("Compatibility colums error");
                return response;
            }

            bool createIndex_res = table->createUnorderedIndex(columns_name);
            

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
