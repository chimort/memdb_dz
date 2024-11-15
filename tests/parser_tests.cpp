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
    
    bool parse_result = parser.parse();
    
    
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
    
    for (const auto& [key, value] : expected_values) {
        auto it = insert_values.find(key);
        ASSERT_TRUE(it != insert_values.end()) << "Column " << key << " not found in insert values.";
        EXPECT_EQ(it->second, value) << "Value for column " << key << " does not match.";
    }
}

