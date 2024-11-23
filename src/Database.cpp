#include "Database.h"
#include "QueryParser.h"
#include "WhereParser.h"

#include <iostream>
#include <variant>

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
        } case parser::CommandType::DELETE : {
            auto table_name_opt = parser.getTableName();
            
            const std::string table_name = table_name_opt;
            auto table_it = tables_.find(table_name);
            if (table_it == tables_.end()) {
                response->setStatus(false);
                response->setMessage("Table not found for delete");
                return response;
            } 

            auto condition = parser.getCondition();
            std::vector<std::string> column_name;
            auto expression = parse_where(condition, column_name);

            std::vector<config::ColumnValue> statement(column_name.size());
            for (auto row : table_it->second->getData()) {
                for (int i = 0; i < column_name.size(); ++i) {
                    statement[i] = row.second[column_name[i]];
                }
                auto ans = expression->apply(statement);
                if (std::holds_alternative<std::monostate>(ans[column_name.size()])) {
                    const int row_id = row.first;
                    if (!table_it->second->deleteRow(row_id)) {
                        response->setStatus(false);
                        response->setMessage("Error in delete query");
                    }
                } else if (std::get<bool>(ans[column_name.size()])) {
                    const int row_id = row.first;
                    if (table_it->second->deleteRow(row_id)) {
                        response->setStatus(false);
                        response->setMessage("Error in delete query");
                    }
                }
            }
            response->setStatus(true);
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
        case parser::CommandType::SELECT: {
            auto table_name_opt = parser.getTableName();
            const auto& selected_columns = parser.getSelectedCol();

            const std::string table_name = table_name_opt;

            auto table_it = tables_.find(table_name);
            if (table_it == tables_.end()) {
                response->setStatus(false);
                response->setMessage("Table not found for select");
                return response;
            }
            std::vector<config::ColumnSchema> new_schema;
            for( auto old_s : table_it->second->getSchema()){
                for ( auto new_s : selected_columns){
                    if(old_s.name == new_s){
                        new_schema.push_back(old_s);
                    }
                }
            }
            if(new_schema.size() != selected_columns.size()){
                response->setStatus(false);
                response->setMessage("selected_columns not found for select");
                break;
            }
            Table new_Table(new_schema);

            auto condition = parser.getCondition();

            std::vector<std::string> column_name;
            auto Expression = parse_where(condition, column_name);

            std::vector<config::ColumnValue> statement(column_name.size());
            for( auto row : table_it->second->getData()){
                for(int i = 0; i < column_name.size(); ++i) {
                    statement[i] = row.second[column_name[i]];
                }
                auto ans = Expression ->apply(statement);
                if(std::holds_alternative<std::monostate>(ans[column_name.size()])){
                    config::RowType new_row;
                    for(auto name: new_schema){
                        new_row[name.name] = row.second[name.name];
                    }
                    new_Table.insertRowType(new_row);
                }else if(std::get<bool>(ans[column_name.size()])){
                    config::RowType new_row;
                    for(auto name: new_schema){
                        new_row[name.name] = row.second[name.name];
                    }
                    new_Table.insertRowType(new_row);
                }
            }
            response->setStatus(true);
            response->setData(new_Table.getData());
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
