#pragma once
//#include "Response.h"
#include "Table.h"

#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace memdb {

class IResponse {
public:
    virtual ~IResponse() = default;
    virtual bool getStatus() const = 0;
    virtual const std::string& getMessage() const = 0;
};

class Database
{
public:
    static Database& getInstance();

    Database(const Database&) = delete;
    Database operator=(const Database&) = delete;

    bool loadFromFile(std::ifstream& ifs) const;
    bool saveToFile(const std::string& filename, const std::string& table_name) const;

    std::unique_ptr<IResponse> execute(const std::string_view& str);

    std::shared_ptr<Table> getTable(const std::string& table_name);

private:
    Database() = default;
    ~Database() = default;

    struct Response : public IResponse {
        bool status_;
        std::string msg_;

        inline void setStatus(const bool& status) { status_ = status; }
        inline void setMessage(const std::string& msg) { msg_ = msg; }
        inline bool getStatus() const override { return status_; }
        inline const std::string& getMessage() const override { return msg_; }
    };
    
    std::unordered_map<std::string, std::shared_ptr<Table>> tables_;
};
}