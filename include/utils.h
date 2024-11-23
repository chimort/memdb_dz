#pragma once
#include "Config.h"

#include<optional>
#include<string>

namespace utils {

template<typename T>
std::optional<T> get(const config::RowType& row, const std::string& column_name) {
    auto it = row.find(column_name);
    if (it == row.end()) {
        return std::nullopt;
    }

    const auto& value_variant = it->second;
    if (std::holds_alternative<std::monostate>(value_variant)) {
        return std::nullopt; // Значение равно NULL
    }

    if (std::holds_alternative<T>(value_variant)) {
        return std::get<T>(value_variant);
    }

    return std::nullopt;
}

} // namespace utils