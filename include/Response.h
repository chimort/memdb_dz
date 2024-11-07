#pragma once
#include <exception>

namespace memdb
{
class Response
{
public:
    Response();
    ~Response();

    

private:
    bool ok_;
    std::exception exc_;
};  
} // namespace memdb
