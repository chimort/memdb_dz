#include <gtest/gtest.h>
#include <string>
#include <unordered_map>
#include "QueryParser.h"
#include "Config.h" 

using namespace memdb;
using namespace parser; 

TEST(QueryParserTest, CreateTableTestSimple) {
    std::string create_query = R"(CREATE\n\nTABLE users ({key, unique}   id: int32, \n login: string[32]="Привет, БД.", password_hash: bytes[9] = 0x1341, is_admin : bool   = false))";
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
        {"id", config::ColumnType::INT, 0, {1, 0, 1}},
        {"login", config::ColumnType::STRING, 32, {0, 0, 0}, "Привет, БД."}, 
        {"password_hash", config::ColumnType::BITSTRING, 9, {0, 0, 0}, "0x1341"},
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
    
}

TEST(QueryParserTest, CreateTableTestRegisters) {
    std::string create_query = R"(CReATE TaBle family (  {UNiquE} name: string[12], \n age: bytes[50] = 0x0, {UNIQUE,\n\n    \n KEY} passport_data : bytes[100], job : string[30] = "Безработный(ая).", {unIque, auToincRemenT, KeY} family_status : string[30]))";
    QueryParser parser(create_query); 
    
    std::cout << "Testing query: " << create_query << std::endl;

    bool parse_result = parser.parse();
    
    std::cout << "Parse result: " << (parse_result ? "Success" : "Failure") << std::endl;
    std::cout << "Parsed command: " << (parse_result ? "CREATE_TABLE" : "Invalid") << std::endl;
    std::cout << "Parsed table name: " << parser.getTableName() << std::endl;

    EXPECT_TRUE(parse_result);
    EXPECT_EQ(parser.getCommandName(), CommandType::CREATE_TABLE);    
    EXPECT_EQ(parser.getTableName(), "family");
    

    std::vector<config::ColumnSchema> columns_parametrs_ = {
        {"name", config::ColumnType::STRING, 12, {1, 0, 0}},
        {"age", config::ColumnType::BITSTRING, 50, {0, 0, 0}, "0x0"}, 
        {"passport_data", config::ColumnType::BITSTRING, 100, {1, 0, 1}},
        {"job", config::ColumnType::BOOL, 30, {0, 0, 0}, "Безработный(ая)."},
        {"family_status", config::ColumnType::BOOL, 30, {1, 1, 1}}
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
    
}



TEST(QueryParserTest, CreateIndexTestSimple) {
    std::string create_query = "crEatE uNordErED  \n  INdEX ON\n\n\nusers BY id, login";
    std::cout << "Testing query: " << create_query << std::endl;
    QueryParser parser(create_query);
    bool parse_result = parser.parse();

    std::cout << "Parse result: " << (parse_result ? "Success" : "Failure") << std::endl;
    std::cout << "Parsed command: " << (parse_result ? "CREATE_INDEX" : "Invalid") << std::endl;
    std::cout << "Parsed table name: " << parser.getTableName() << std::endl;

    EXPECT_TRUE(parse_result);
    EXPECT_EQ(parser.getCommandName(), CommandType::CREATE_INDEX);    
    EXPECT_EQ(parser.getTableName(), "users");

    std::cout << std::endl;
    std::cout << "Expected values:" << std::endl;
    std::unordered_map<std::string, config::IndexType> column_index_type_ = {
        {"login", {config::IndexType::UNORDERED}},

        {"id", {config::IndexType::UNORDERED}},

    };
    
    for (const auto& [key, value] : column_index_type_){
        std::cout << "Column: " << key << " Type: ";
        switch (value) {
        case config::IndexType::ORDERED:
            std::cout << "ordered" << std::endl;
            break;
        case config::IndexType::UNORDERED:
            std::cout << "unordered" << std::endl;
            break;
        default:
            std::cout << "unknown" << std::endl;
            break;
        }
    }    

    auto reate_values = parser.getCreateIndexType();
    std::cout << std::endl;
    std::cout << "Parsed Values" << std::endl;
    for (const auto& [key, value] : reate_values){
        std::cout << "Column: " << key << " Type: ";
        switch (value) {
        case config::IndexType::ORDERED:
            std::cout << "ordered" << std::endl;
            break;
        case config::IndexType::UNORDERED:
            std::cout << "unordered" << std::endl;
            break;
        default:
            std::cout << "unknown" << std::endl;
            break;
        }
    }  
}

TEST(QueryParserTest, InsertParseTestSimple) {
    std::string insert_query = R"(insert\n\n\n\n\n(id = 1,\n\nage = 30,    \n     to = "Alice") tO users)";
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
        {"to", "Alice"},
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

TEST(QueryParserTest, InsertParseTestWithoutName) {
    std::string insert_query = R"(inSErt  \n\n\n\n\n (,\n\n   age=30  ,    \n     to="Alice") tO users)";
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
        {"to", "Alice"},
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


TEST(QueryParserTest, SelectParseTestSimple) {
    std::string create_query = R"(select id, login from users where is_admin || id < 10)";
    std::cout << create_query << std::endl;
    QueryParser parser(create_query);

    bool parse_result = parser.parse();

    std::cout << "Parse result: " << (parse_result ? "Success" : "Failure") << std::endl;
    std::cout << "Parsed command: " << (parse_result ? "SELECT" : "Invalid") << std::endl;
    std::cout << "Parsed table name: " << parser.getTableName() << std::endl;

    EXPECT_TRUE(parse_result);
    EXPECT_EQ(parser.getCommandName(), CommandType::SELECT);    
    EXPECT_EQ(parser.getTableName(), "users");

    std::cout << std::endl;
    std::cout << "Parsed values: " << std::endl;
    auto it = parser.getSelectedCol();
    for (auto& expression : it){
        std::cout << "Value: " << expression << std::endl;
    }
    std::cout << "Condition: " << parser.getCondition() << std::endl;
}

TEST(QueryParserTest, SelectParseTestConditions) {
    std::string create_query = R"(SelEct id, login fRoM users WHErE (is_admin != 'Abdylla' || |is_admin| > 10) && id = 5 && task < 20) ^^ done > 0x5321)";
    std::cout << create_query << std::endl;
    QueryParser parser(create_query);

    bool parse_result = parser.parse();

    std::cout << "Parse result: " << (parse_result ? "Success" : "Failure") << std::endl;
    std::cout << "Parsed command: " << (parse_result ? "SELECT" : "Invalid") << std::endl;
    std::cout << "Parsed table name: " << parser.getTableName() << std::endl;

    EXPECT_TRUE(parse_result);
    EXPECT_EQ(parser.getCommandName(), CommandType::SELECT);    
    EXPECT_EQ(parser.getTableName(), "users");

    std::cout << std::endl;
    std::cout << "Parsed values: " << std::endl;
    auto it = parser.getSelectedCol();
    for (auto& expression : it){
        std::cout << "Value: " << expression << std::endl;
    }
    std::cout << "Condition: " << parser.getCondition() << std::endl;
}


TEST(QueryParserTest, DeleteParseTestSimple) {
    std::string create_query = R"( DeLETe users WHErE is_admin || id < 10)";
    std::cout << create_query << std::endl;
    QueryParser parser(create_query);

    bool parse_result = parser.parse();

    std::cout << "Parse result: " << (parse_result ? "Success" : "Failure") << std::endl;
    std::cout << "Parsed command: " << (parse_result ? "DELETE" : "Invalid") << std::endl;
    std::cout << "Parsed table name: " << parser.getTableName() << std::endl;

    EXPECT_TRUE(parse_result);
    EXPECT_EQ(parser.getCommandName(), CommandType::DELETE);    
    EXPECT_EQ(parser.getTableName(), "users");

    std::cout << "Table name: " << parser.getTableName() << " Condition: " << parser.getCondition() << std::endl;
}

TEST(QueryParserTest, UpdateParseTestSimple) {
    std::string create_query = R"(UPdate users SET is_admin = true WHErE login = "vasya")";
    std::cout << create_query << std::endl;
    QueryParser parser(create_query);

    bool parse_result = parser.parse();

    std::cout << "Parse result: " << (parse_result ? "Success" : "Failure") << std::endl;
    std::cout << "Parsed command: " << (parse_result ? "UPDATE" : "Invalid") << std::endl;
    std::cout << "Parsed table name: " << parser.getTableName() << std::endl;

    EXPECT_TRUE(parse_result);
    EXPECT_EQ(parser.getCommandName(), CommandType::UPDATE);    
    EXPECT_EQ(parser.getTableName(), "users");

    std::cout << std::endl;
    std::cout << "Parsed values: " << std::endl;
    auto it = parser.getUpdateValues();
    for (auto& [key, value] : it){
        std::cout << "Key: " << key << " Value: " << value << std::endl;
    }
    std::cout << " Condition: " << parser.getCondition();
}

TEST(QueryParserTest, UpdateParseTestHardConditions) {
    std::string create_query = R"(UPdate users SET city = "Moscow", name = "Vasily" WHErE login = "vasya", id = 0x4912f4)";
    std::cout << create_query << std::endl;
    QueryParser parser(create_query);

    bool parse_result = parser.parse();

    std::cout << "Parse result: " << (parse_result ? "Success" : "Failure") << std::endl;
    std::cout << "Parsed command: " << (parse_result ? "UPDATE" : "Invalid") << std::endl;
    std::cout << "Parsed table name: " << parser.getTableName() << std::endl;

    EXPECT_TRUE(parse_result);
    EXPECT_EQ(parser.getCommandName(), CommandType::UPDATE);    
    EXPECT_EQ(parser.getTableName(), "users");

    std::cout << std::endl;
    std::cout << "Parsed values: " << std::endl;
    auto it = parser.getUpdateValues();
    for (auto& [key, value] : it){
        std::cout << "Key: " << key << " Value: " << value << std::endl;
    }
    std::cout << " Condition: " << parser.getCondition();
}

TEST(QueryParserTest, EmptyConditionTest){
    std::string str1 = "inSert (id = 1, name = 'Alice', age = 30) users"; //skiped to
    std::string str2 = "SelEct id, login users WHErE is_admin || id < 10"; //skiped from
    std::string str3 = "DeLETe users is_admin || id < 10"; //skiped where
    std::string str4 = "UPdate users is_admin = true where login = 'vasya'"; //skiped set

    std::cout << "Strings: " << std::endl;
    std::cout << str1 << std::endl << str2 << std::endl << str3 << std::endl << str4 << std::endl; 

    QueryParser parser1(str1);
    bool res_parse1 = parser1.parse();
    QueryParser parser2(str2);
    bool res_parse2 = parser2.parse();
    QueryParser parser3(str3);
    bool res_parse3 = parser3.parse();
    QueryParser parser4(str4);
    bool res_parse4 = parser4.parse();

    std::cout << "Expected false";
    EXPECT_FALSE(res_parse1);
    EXPECT_FALSE(res_parse2);
    EXPECT_FALSE(res_parse3);
    EXPECT_FALSE(res_parse4);
}