#pragma once

#include <string_view>

namespace memdb
{

class Database;

class Response
{
public:
    Response() = default;
    ~Response() = default;

    inline bool getStatus() const { return ok_; }
    inline std::string_view getMessage() const { return exc_; }

private:
    friend class Database;

    inline void setMessage(const std::string_view& msg) { exc_ = msg; }
    inline void setStatus(const bool& status) { ok_ = status; }

    void checkParser();

    bool ok_;
    std::string_view exc_;
};  
} // namespace memdb
