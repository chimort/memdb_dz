#pragma once
#include <exception>

namespace memdb
{
class Responce
{
public:
    Responce(/* args */);
    ~Responce();

private:
    bool ok_;
    std::exception exc_;
};  
} // namespace memdb
