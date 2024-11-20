#pragma once
#include <variant>
#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include <cstdint>

namespace config {

enum class ColumnType {
    INT,
    STRING,
    BOOL,
    BITSTRING
};

struct ColumnSchema {
    std::string name;
    ColumnType type;
    size_t max_size;
    bool attributes[3] = {0}; // {unique, autoincrement, key}
    std::string default_value;
};


using BitString = std::vector<uint8_t>;
using ColumnValue = std::variant<std::monostate, int, std::string, bool, BitString>; 
using RowType = std::unordered_map<std::string, ColumnValue>;

} // namespace config
