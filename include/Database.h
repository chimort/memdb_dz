#pragma once
#include "Responce.h"
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
    Database(/* args */);
    ~Database();

    bool loadFromFile(std::ifstream);
    bool saveToFile(std::ofstream);

    memdb::Responce execute(const std::string_view& str);

private:
    std::unordered_map<std::string, std::shared_ptr<TableBase>> tables_;
};
}