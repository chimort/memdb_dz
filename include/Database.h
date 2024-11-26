#pragma once

#include "Table.h"
#include "utils.h"

#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace memdb {

struct Response {
    bool status_;
    std::string msg_;
    std::unordered_map<int, config::RowType> data_;

    void setStatus(const bool& status) { status_ = status; }
    void setMessage(const std::string& msg) { msg_ = msg; }
    bool getStatus() const { return status_; }
    const std::string& getMessage() const { return msg_; }

    void setData(const std::unordered_map<int, config::RowType>& data) { data_ = data; }
    std::unordered_map<int, config::RowType>& getData() { return data_; }

    template<typename KeyType>
    auto get(const KeyType& key) {
        return utils::get(data_, key);
    }
};

class Database
{
public:
    static Database& getInstance();

    Database(const Database&) = delete;
    Database operator=(const Database&) = delete;

    bool loadFromFile(const std::string& filename, const std::string& table_name);
    bool saveToFile(const std::string& filename, const std::string& table_name) const;

    std::unique_ptr<memdb::Response> execute(const std::string_view& str);

    std::shared_ptr<Table> getTable(const std::string& table_name);

private:
    Database() = default;
    ~Database() = default;
    
    std::unordered_map<std::string, std::shared_ptr<Table>> tables_;
};
}