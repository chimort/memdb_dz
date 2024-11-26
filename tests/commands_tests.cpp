#include <gtest/gtest.h>
#include "Database.h"

class DatabaseTest : public ::testing::Test {
protected:
    memdb::Database& db = memdb::Database::getInstance();
};

void PrintVariant(const std::string& col_name, const config::ColumnValue& col_value){ 
    if(std::holds_alternative<int>(col_value)){ 
        std::cout << col_name << ": " << std::get<int>(col_value) << ", "; 
    } 
    else if(std::holds_alternative<bool>(col_value)){ 
        if(std::get<bool>(col_value)){ 
            std::cout << col_name << ": true, "; 
        }else{ 
            std::cout << col_name << ": false, "; 
        } 
    } 
    else if(std::holds_alternative<std::string>(col_value)){ 
        std::cout << col_name << ": " << std::get<std::string>(col_value) << ", "; 
    } 
    else if(std::holds_alternative<config::BitString>(col_value)){ 
        auto temp = std::get<config::BitString>(col_value); 
        std::cout << col_name << ": "; 
        for(const auto& te: temp){ 
            std::cout << te; 
        } 
        std::cout << col_name << ", "; 
    } 
    else{ 
        std::cout << col_name << ": NULL, "; 
    } 
} 
 
void PrintData(const std::unordered_map<int, config::RowType> &data, const std::string& query){ 
    std::cout << " << " << query << " >>\n"; 
    for (const auto& [key, row] : data) { 
        for(const auto& [col_name, col_value] : row){ 
            PrintVariant(col_name, col_value); 
        } 
        std::cout << "\n"; 
    } 
    std::cout << "\n"; 
} 

void PrintData(const std::unordered_map<int, config::RowType> &data, const std::string& query){
    std::cout << " << " << query << " >>\n";
    for (const auto& [key, row] : data) {
        for(const auto& [col_name, col_value] : row){
            PrintVariant(col_name, col_value);
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

TEST_F(DatabaseTest, CREATEINDEXS_SELECT) {
    std::string create_table_query = "create table medicine ({key, autoincrement} id : int32, doctors : string[32], equipment : bool, age : int32)";
    auto res = db.execute(create_table_query);
    EXPECT_TRUE(res->getStatus());

    std::string create_index_query = "create unordered index on medicine by age, doctors";
    auto res1 = db.execute(create_index_query);

    for (int i = 0; i <= 1; ++i) {
        std::string insert_query = R"(insert (doctors = "Strange)" + std::to_string(i) + R"(", age =)" + std::to_string(i) + R"() to medicine)";
        auto res2 = db.execute(insert_query);
    }
  
TEST_F(DatabaseTest, INSERT_implicit) {
    std::string create_table_query = "create table technique ({unique} cars : string[32], {unique, autoincrement} cars_id : int32, {unique} phone_id : int32, {unique} phone : string[32])";
    auto res = db.execute(create_table_query);
    EXPECT_TRUE(res->getStatus());

    std::string insert_query = R"(insert (cars = "Lada", phone = "vivo") to technique)";
    auto res1 = db.execute(insert_query);
    EXPECT_TRUE(res1->getStatus());

    std::string insert_query1 = R"(insert (, 5, "IPhone") to technique)";
    auto res2 = db.execute(insert_query1);
    EXPECT_TRUE(res2->getStatus());

    auto table = db.getTable("technique");
    auto data_after = table->getData();
    for (const auto& [key, row] : data_after) {
        auto id_opt = utils::get<std::string>(row, "cars"); 
        auto id = id_opt.has_value() ? id_opt.value() : "N/A"; 
 
        auto login_opt = utils::get<int>(row, "cars_id");
        int login = login_opt.has_value() ? login_opt.value() : -1; 
 
        auto is_parent_opt = utils::get<int>(row, "phone_id"); 
        int is_parent = is_parent_opt.has_value() ? is_parent_opt.value() : -1;

        auto is_code = utils::get<std::string>(row, "phone");
        auto code = is_code.has_value() ? is_code.value() : "N/A";
 
        std::cout << "cars: " << id 
                  << ", id_cars: " << login 
                  << ", phone_id: " << is_parent
                  << ", phone " << code
                  << std::endl;
    }

    std::cout << std::endl;
}

TEST_F(DatabaseTest, SELECT) {
    std::string create_table_query = "create tABle table_name ( {  } col1 : int32, col2 : bytes[2])";
    auto res = db.execute(create_table_query);

    EXPECT_TRUE(res  -> getStatus());

    auto res1 = db.execute("iNsErt (col1= 123, col2 =0x0456) tO table_name");
    auto res1_1 = db.execute("iNsErt (col1= 331, col2 =0x5311) tO table_name");

    EXPECT_TRUE(res1_1->getStatus());
    EXPECT_TRUE(res1->getStatus());

    std::string select_query = "Select col1, col2 from table_name where col1 = 123";
    auto res2 = db.execute(select_query);

    EXPECT_TRUE(res2 -> getStatus());

    auto new_table = res2->getData();
    for (auto& [key, rows] : new_table) {
        for (auto& [column_name, row] : rows) {
            if (std::get_if<int>(&row)) {
                std::cout << column_name << " " << std::get<int>(row) << std::endl;
            } else {
                auto vec = std::get<std::vector<uint8_t>>(row);

                std::cout << column_name << " ";
                for (auto& it : vec) {
                    std::cout << it;
                }
            }
            std::cout << " ";
        }
    }

    std::cout << std::endl;
}

TEST_F(DatabaseTest, UPDATE) {
    std::string create_table_query = "create table rooms ({key, autoincrement} id : int32, occupants : string[5], {unique} beds : int32 = 3, beds_code : bytes[2] = 0x1241)";
    auto res = db.execute(create_table_query);
    EXPECT_TRUE(res -> getStatus());

    auto res1 = db.execute(R"(insert (occupants = "Anna", beds_code = 0x1351) to rooms)");
    EXPECT_TRUE(res1 -> getStatus());

    auto res2 = db.execute(R"(insert (occupants = "Bob", beds_code = 0x2351) to rooms)");
    EXPECT_TRUE(res2 -> getStatus());

    std::string update_table_query = "update rooms set beds = 5 where beds_code < 0x3451 && beds_code > 0x1241";
    auto res3 = db.execute(update_table_query);
    EXPECT_TRUE(res3 -> getStatus());

    auto table = db.getTable("rooms");
    auto data_after = table->getData();

    PrintData(data_after, create_table_query);
}

TEST_F(DatabaseTest, CREATEINDEXS) {
    std::string create_table_query = "create table medicine ({key, autoincrement} id : int32, doctors : string[32], equipment : bool, age : int32)";
    auto res = db.execute(create_table_query);
    EXPECT_TRUE(res->getStatus());

    std::string create_index_query = "create unordered index on medicine by id, doctors";
    auto res1 = db.execute(create_index_query);
    EXPECT_TRUE(res1 -> getStatus());

    std::string insert_query = R"(insert (id = 5, doctors = "Harry") to medicine)";
    std::string create_index_query = "create ordered index on medicine by id, doctors";
    auto res1 = db.execute(create_index_query);

    EXPECT_TRUE(res1 -> getStatus());

}

TEST_F(DatabaseTest, DELETE) {
    std::string create_table_query = "create tABle family ({unique} id : int32, name : string[32], isParent : bool, code : bytes[3])";

    auto res = db.execute(create_table_query);
    EXPECT_TRUE(res->getStatus());

    auto res1 = db.execute(R"(insert (id = 1, name = "Anna", isParent = false, code = 0x0ffee3) to family)");
    auto res2 = db.execute(R"(insert (id = 2, name = "Ivan", isParent = true, code = 0x0e572f) to family)");

    EXPECT_TRUE(res1->getStatus());
    EXPECT_TRUE(res2->getStatus());

    std::string delete_query = R"(DELETE family where code = 0x0ffee3)";
    auto res3 = db.execute(delete_query);
    EXPECT_TRUE(res3->getStatus());

    auto table = db.getTable("family");
    auto data_after = table->getData();

    for (const auto& [key, row] : data_after) { 
        auto id_opt = utils::get<int>(row, "id"); 
        int id = id_opt.has_value() ? id_opt.value() : -1; 
 
        auto login_opt = utils::get<std::string>(row, "name"); 
        std::string login = login_opt.has_value() ? login_opt.value() : "N/A"; 
 
        auto is_parent_opt = utils::get<bool>(row, "is_parent"); 
        bool is_parent = is_parent_opt.has_value() ? is_parent_opt.value() : false;
        auto is_code = utils::get<std::vector<uint8_t>>(row, "code");
        std::vector<uint8_t> code = is_code.has_value() ? is_code.value() : std::vector<uint8_t>{0};
 
        std::cout << "id: " << id 
                  << ", name: " << login 
                  << ", is_parent: " << (is_parent ? "true" : "false") 
                  << " code: ";
        for (auto& element_vector : code) {
            std::cout << element_vector << " ";
        }
        std::cout << std::endl;
    }
}