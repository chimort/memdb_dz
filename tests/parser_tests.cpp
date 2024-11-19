#include <gtest/gtest.h>
#include <string>
#include <unordered_map>
#include "QueryParser.h"
#include "Config.h" 

using namespace memdb;
using namespace parser; 

TEST(QueryParserTest, InsertParseTest) {
    std::string insert_query = "insert (id = 1, name = 'Alice', age = 30) to users";
    QueryParser parser(insert_query);
    
    std::cout << "Testing query: " << insert_query << std::endl;

    bool parse_result = parser.parse();
    
    std::cout << "Parse result: " << (parse_result ? "Success" : "Failure") << std::endl;
    std::cout << "Parsed command: " << (parse_result ? "INSERT" : "Invalid") << std::endl;
    std::cout << "Parsed table name: " << parser.getTableName() << std::endl;

    EXPECT_TRUE(parse_result);
    EXPECT_EQ(parser.getCommandName(), CommandType::INSERT);    
    EXPECT_EQ(parser.getTableName(), "users");
    
    std::unordered_map<std::string, std::string> expected_values = {
        {"id", "1"},
        {"name", "'Alice'"},
        {"age", "30"}
    };
    
    auto insert_values = parser.getInsertValues();
    EXPECT_EQ(insert_values.size(), expected_values.size());

    std::cout << "Expected values: " << std::endl;
    for (const auto& [key, value] : expected_values) {
        std::cout << "  " << key << " = " << value << std::endl;
    }

    std::cout << "Parsed insert values: " << std::endl;
    for (const auto& [key, value] : insert_values) {
        std::cout << "  " << key << " = " << value << std::endl;
    }

    for (const auto& [key, value] : expected_values) {
        auto it = insert_values.find(key);
        ASSERT_TRUE(it != insert_values.end()) << "Column " << key << " not found in insert values.";
        EXPECT_EQ(it->second, value) << "Value for column " << key << " does not match.";
    }
}

TEST(QueryParserTest, CreateParseTest) {
    std::string create_query = R"(create table users ({key, autoincrement} id : int32 = 5000,\n {unique} login: string[32] = "Привет, БД.", password_hash: bytes[9], is_admin : bool = false))";
    QueryParser parser(create_query);
    
    std::cout << "Testing query: " << create_query << std::endl;

    bool parse_result = parser.parse();
    
    std::cout << "Parse result: " << (parse_result ? "Success" : "Failure") << std::endl;
    std::cout << "Parsed command: " << (parse_result ? "CREATE_TABLE" : "Invalid") << std::endl;
    std::cout << "Parsed table name: " << parser.getTableName() << std::endl;

    EXPECT_TRUE(parse_result);
    EXPECT_EQ(parser.getCommandName(), CommandType::CREATE_TABLE);    
    EXPECT_EQ(parser.getTableName(), "users");
    

    std::vector<config::ColumnSchema> columns_parametrs_ = {
        {"id", config::ColumnType::INT, 0, {0, 1, 1}, "5000"},
        {"login", config::ColumnType::STRING, 12, {1, 0, 0}, "Привет, БД."}, 
        {"password_hash", config::ColumnType::BITSTRING, 9, {0, 0, 0}, ""},
        {"is_admin", config::ColumnType::BOOL, 0, {0, 0, 0}, "false"}
    };

    std::cout << std::endl;
    std::cout << "Expected values: " << std::endl;
    for (const auto& it : columns_parametrs_) {
        std::cout << "Key: " << it.name << " Type: ";
        switch (it.type) {
            case config::ColumnType::INT:
                std::cout << "int";
                break;
            case config::ColumnType::STRING:
                std::cout << "string";
                break;
            case config::ColumnType::BOOL:
                std::cout << "bool";
                break;
            case config::ColumnType::BITSTRING:
                std::cout << "bitstring";
                break;
            default:
                std::cout << "Unknown";
        }

        std::cout << " Default: " << it.default_value << " Attributes: ";

        if (it.attributes[0] == 1){
            std::cout << "unique ";
        }
        if (it.attributes[1] == 1){
            std::cout << "autoincrement ";
        }
        if (it.attributes[2] == 1){
            std::cout << "key ";
        }

        std::cout << "Max Size:" << it.max_size;
        std::cout << std::endl;       
    }
    std::cout << std::endl;

    auto create_values = parser.getCreateTableParametrs();

    std::cout << std::endl;
    std::cout << "Parsed values: " << std::endl;
    for (const auto& it : create_values) {
        std::cout << "Key: " << it.name << " Type: ";
        switch (it.type) {
            case config::ColumnType::INT:
                std::cout << "int";
                break;
            case config::ColumnType::STRING:
                std::cout << "string";
                break;
            case config::ColumnType::BOOL:
                std::cout << "bool";
                break;
            case config::ColumnType::BITSTRING:
                std::cout << "bitstring";
                break;
            default:
                std::cout << "Unknown";
        }
        
        std::cout << " Default: " << it.default_value << " Attributes: ";

        if (it.attributes[0] == 1){
            std::cout << "unique ";
        }
        if (it.attributes[1] == 1){
            std::cout << "autoincrement ";
        }
        if (it.attributes[2] == 1){
            std::cout << "key ";
        }

        std::cout << "Max Size:" << it.max_size;
        std::cout << std::endl;
    }
    std::cout << std::endl;




    create_query = "create ordered index\n on users by login, id, admin";
    std::cout << "Testing query: " << create_query << std::endl;
    QueryParser parser2(create_query);
    bool parse2_result = parser2.parse();

    std::cout << "Parse result: " << (parse_result ? "Success" : "Failure") << std::endl;
    std::cout << "Parsed command: " << (parse_result ? "CREATE_INDEX" : "Invalid") << std::endl;
    std::cout << "Parsed table name: " << parser.getTableName() << std::endl;

    EXPECT_TRUE(parse2_result);
    EXPECT_EQ(parser2.getCommandName(), CommandType::CREATE_INDEX);    
    EXPECT_EQ(parser2.getTableName(), "users");

    std::cout << std::endl;
    std::cout << "Expected values:" << std::endl;
    std::unordered_map<std::string, IndexType> column_index_type_ = {
        {"login", {IndexType::ORDERED}},

        {"id", {IndexType::ORDERED}},

        {"admin", {IndexType::ORDERED}}
    };
    
    for (const auto& [key, value] : column_index_type_){
        std::cout << "Column: " << key << " Type: ";
        switch (value) {
        case IndexType::ORDERED:
            std::cout << "ordered" << std::endl;
            break;
        case IndexType::UNORDERED:
            std::cout << "unordered" << std::endl;
            break;
        default:
            std::cout << "unknown" << std::endl;
            break;
        }
    }    

    auto reate_values = parser2.getCreateIndexType();
    std::cout << std::endl;
    std::cout << "Parsed Values" << std::endl;
    for (const auto& [key, value] : column_index_type_){
        std::cout << "Column: " << key << " Type: ";
        switch (value) {
        case IndexType::ORDERED:
            std::cout << "ordered" << std::endl;
            break;
        case IndexType::UNORDERED:
            std::cout << "unordered" << std::endl;
            break;
        default:
            std::cout << "unknown" << std::endl;
            break;
        }
    }  
    
}