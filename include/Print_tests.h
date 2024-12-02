#pragma once
#include <iostream>
#include "Config.h"

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
        std::cout << ", ";
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

