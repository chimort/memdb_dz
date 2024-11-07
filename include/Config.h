#include <variant>
#include <string>
#include <vector>
#include <unordered_map>

namespace config {

using ColumnValue = std::variant<int, std::string, bool, std::vector<uint8_t>>; 
using RowType = std::unordered_map<std::string, ColumnValue>;

} // namespace config