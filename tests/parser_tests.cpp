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
    std::string create_query = "create table users ({key, autoincrement} id : int32 = 5000,\n {unique} login: string[32] = 'Привет БД', password_hash: bytes[8], is_admin : bool = false)";
    QueryParser parser(create_query);
    
    std::cout << "Testing query: " << create_query << std::endl;

    bool parse_result = parser.parse();
    
    std::cout << "Parse result: " << (parse_result ? "Success" : "Failure") << std::endl;
    std::cout << "Parsed command: " << (parse_result ? "CREATE_TABLE" : "Invalid") << std::endl;
    std::cout << "Parsed table name: " << parser.getTableName() << std::endl;

    EXPECT_TRUE(parse_result);
    EXPECT_EQ(parser.getCommandName(), CommandType::CREATE_TABLE);    
    EXPECT_EQ(parser.getTableName(), "users");
    
    struct Columns {
        std::string type;
        std::string default_value;
        std::vector<std::string> attributes;
    };

    std::unordered_map<std::string, Columns> columns_parametrs_ = {

        {"id", {"int", {"5000"}, {"key", "autoincrement"}}},

        {"login", {"string[32]", {"Привет БД"}, {"unique"}}},

        {"password_hash", {"bytes[8]", {}, {}}},

        {"is_admin", {"bool", "false", {}}}

    };
    
    auto create_values = parser.getCreateTableParametrs();

    std::cout << std::endl;
    std::cout << "Expected values: " << std::endl;
    for (const auto& [key, value] : columns_parametrs_) {
        std::cout << "Key: " << key << " Type: " << value.type << " Default: " << value.default_value << " Attributes: ";
        for (const auto& it : value.attributes){
            std::cout << " " << it << " ";
        }
        std::cout << std::endl;
    }

    std::cout << std::endl;
    std::cout << "Parsed values: " << std::endl;
    for (const auto& [key, value] : create_values) {
        std::cout << "Key: " << key << " Type: ";
        switch (value.type) {
            case ColumnType::INT:
                std::cout << "int";
                break;
            case ColumnType::STRING:
                std::cout << "string";
                break;
            case ColumnType::BOOL:
                std::cout << "bool";
                break;
            case ColumnType::BITSTRING:
                std::cout << "bitstring";
                break;
            default:
                std::cout << "Unknown";
        }
        
        std::cout << " Default: " << value.default_value << " Attributes: ";

        if (value.attributes[0] == 1){
            std::cout << "unique ";
        }
        if (value.attributes[1] == 1){
            std::cout << "autoincrement ";
        }
        if (value.attributes[2] == 1){
            std::cout << "key ";
        }
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

    std::cout << std::endl;
    std::cout << "Parsed values:" << std::endl;
    auto create2_values = parser2.getCreateIndexType();
    for (const auto& [key, value] : create2_values){
        std::cout << "Column: " << key << "Type: ";
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