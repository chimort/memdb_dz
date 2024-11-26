#include "Database.h"
#include "QueryParser.h"
#include "WhereParser.h"

#include <iostream>
#include <variant>
#include <set>
#include <algorithm>

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

bool isValidBracket(const std::string& text){
    if(std::count(text.begin(), text.end(), '|') % 2 == 1){
        return false;
    };
    int stack = 0;
    for(const auto& character: text){
        if(character == '('){
            ++stack;
        }else if(character == ')'){
            if(stack == 0){
                return false;
            }
            --stack;
        }
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
            std::vector<std::vector<std::string>> PCNF;
            auto expression = parse_where(condition, column_name, PCNF);

            std::set<int> ids; // id для удаления

            std::vector<config::ColumnValue> statement(column_name.size());
            for (auto row : table_it->second->getData()) {
                for (int i = 0; i < column_name.size(); ++i) {
                    statement[i] = row.second[column_name[i]];
                }
                auto ans = expression->apply(statement);
                if (std::holds_alternative<std::monostate>(ans[column_name.size()])) {
                    ids.insert(row.first);
                } else if (std::get<bool>(ans[column_name.size()])) {
                    ids.insert(row.first);
                }
            }
            for (const int id : ids) {
                if (!table_it->second->deleteRow(id)) {
                    response->setStatus(false);
                    response->setMessage("Error on delete method");
                    return response;
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
                response->setMessage("Table '" + table_name + "' not found");
                return response;
            }

            auto& table = table_it->second;
            auto column_index_type = parser.getCreateIndexType(); // std::unordered_map<std::string, config::IndexType>
            auto table_columns = table->getSchema();

            bool columns_exist = true;
            bool index_created = false;

            for (const auto& [column_name, index_type] : column_index_type) {
                bool found = false;
                for (const auto& col_schema : table_columns) {
                    if (col_schema.name == column_name) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    columns_exist = false;
                    response->setStatus(false);
                    response->setMessage("Column '" + column_name + "' does not exist in table '" + table_name + "'");
                    return response;
                }

                bool createIndex_res = table->createIndex({column_name}, index_type);
                if (!createIndex_res) {
                    response->setStatus(false);
                    response->setMessage("Failed to create index on column '" + column_name + "'");
                    return response;
                }
                index_created = true;
            }

            if (!index_created) {
                response->setStatus(false);
                response->setMessage("No indices were created");
                return response;
            }

            response->setStatus(true);
            response->setMessage("Index(es) created successfully");
            break;

        } case parser::CommandType::SELECT: {
            auto table_name_opt = parser.getTableName();
            const auto& selected_columns = parser.getSelectedCol();
            auto condition = parser.getCondition();

            //correct (,),|
            if(!isValidBracket(condition)) {
                response->setStatus(false);
                response->setMessage("incorrect (,),| sequence");
                break;
            }

            //correct table_name
            const std::string table_name = table_name_opt;
            auto table_it = tables_.find(table_name);
            if (table_it == tables_.end()) {
                response->setStatus(false);
                response->setMessage("Table not found for select");
                break;
            }

            //correct name of columns for select
            std::vector<config::ColumnSchema> new_schema;
            const auto& schema = table_it->second->getSchema();
            for( auto old_s : schema){
                for ( auto new_s : selected_columns){
                    if(old_s.name == new_s){
                        new_schema.push_back(old_s);
                    }
                }
            }
            if(new_schema.size() != selected_columns.size()){
                response->setStatus(false);
                response->setMessage("incorrect column names select");
                break;
            }

            std::vector<std::string> column_name;
            std::vector<std::vector<std::string>> PCNF;
            auto Expression = parse_where(condition, column_name, PCNF);

            //correct result of condition
            if(std::dynamic_pointer_cast<SpaceOp>(Expression)){
                response -> setStatus(false);
                response -> setMessage("wrong condition");
                break;
            }

            std::vector<std::string> condition_name;
            for( auto new_s : column_name){
                for ( auto old_s : schema){
                    if(old_s.name == new_s){
                        condition_name.push_back(new_s);
                    }
                }
            }

            //correct name of columns condition
            if(condition_name.size() != column_name.size()){
                response ->setStatus(false);
                response ->setMessage("incorrect column names select in condition");
                break;
            }

            //correct of type in condition;
            bool is_index = true;
            auto recordIndex = getTable(table_name)->Table::record_index(PCNF, is_index);
            std::vector<config::ColumnValue> statement(1, true);

            for( auto new_s : column_name){
                for ( auto old_s : schema){
                    if(old_s.name == new_s){
                        if(old_s.type == config::ColumnType::INT){
                            statement.push_back(0);
                        }else if(old_s.type == config::ColumnType::STRING){
                            statement.push_back("");
                        }else if(old_s.type == config::ColumnType::BOOL){
                            statement.push_back(false);
                        }else{
                            statement.push_back({});
                        }
                    }
                }
            }
            auto check_ans = Expression->apply(statement);
            if(!std::get<bool>(check_ans[0])){
                response->setStatus(false);
                response->setMessage("error of type");
                break;
            }

            Table new_Table(new_schema);
            statement.assign(column_name.size()+1, 0);
            for( auto row : table_it->second->getData()){
                if(is_index && recordIndex.find(row.first) == recordIndex.end()){
                    continue;
                }
                for(int i = 1; i < column_name.size()+1; ++i) {
                    statement[i] = row.second[column_name[i-1]];
                }
                auto ans = Expression ->apply(statement);
                if(std::holds_alternative<std::monostate>(ans[column_name.size()+1])){
                    config::RowType new_row;
                    for(auto name: new_schema){
                        new_row[name.name] = row.second[name.name];
                    }
                    new_Table.insertRowType(new_row);
                }else if(std::get<bool>(ans[column_name.size()+1])){
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

        case parser::CommandType::UPDATE: {
            auto table_name_opt = parser.getTableName();
            const auto& assignment = parser.getUpdateValues();

            const std::string table_name = table_name_opt;

            auto table_it = tables_.find(table_name);
            if (table_it == tables_.end()) {
                response->setStatus(false);
                response->setMessage("Table not found for select");
                return response;
            }

            auto condition = parser.getCondition();

            std::vector<std::string> column_name;
            std::vector<std::vector<std::string>> PCNF;
            auto Expression = parse_where(condition, column_name, PCNF);

            //надо будет написать проверку на выражение

            std::vector<config::ColumnValue> statement(column_name.size());
            //надо будет проверить налиие колонки;
            for( auto row : table_it->second->getData()){

                for(int i = 0; i < column_name.size(); ++i) {
                    statement[i] = row.second[column_name[i]];
                }
                auto ans = Expression ->apply(statement);
                bool isWhere = false ;
                if(std::holds_alternative<std::monostate>(ans[column_name.size()])){
                    isWhere = true;
                }else if(std::get<bool>(ans[column_name.size()])){
                    isWhere = true;
                }
                if(isWhere){
                    config::RowType new_row;
                    for( auto val_col : assignment){
                        std::vector<std::string> assignment_column_name;
                        std::vector<std::vector<std::string>> PCNF;
                        auto assignment_Expression = parse_where(val_col.second, assignment_column_name, PCNF);
                        std::vector<config::ColumnValue> assignment_statement(assignment_column_name.size());
                        for(int i = 0; i < assignment_column_name.size(); ++i) {
                            assignment_statement[i] = row.second[assignment_column_name[i]];
                        }
                        auto assignment_ans = assignment_Expression ->apply(assignment_statement);
                        new_row[val_col.first] = assignment_ans[assignment_column_name.size()];
                    }
                    tables_[table_name]->updateIndices(row.first, new_row);
                    tables_[table_name]->updateRowType(row.first, new_row);
                }
            }
            response->setStatus(true);
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
