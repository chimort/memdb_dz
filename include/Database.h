#pragma once
#include "Response.h"
#include "Table.h"

#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace memdb {
class Database
{
public:
    static Database& getInstance();

    Database(const Database&) = delete;
    Database operator=(const Database&) = delete;

    bool loadFromFile(std::ifstream);
    bool saveToFile(std::ofstream);

    memdb::Response execute(const std::string_view& str);

private:
    Database() = default;
    ~Database() = default;

    std::unordered_map<std::string, std::shared_ptr<TableBase>> tables_;
};
}