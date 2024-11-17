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
