#include "QueryParser.h"

#include <algorithm>
#include <cctype>

namespace memdb
{
namespace parser
{

bool QueryParser::createParse()
{
    return false;
}

bool QueryParser::insertParse()
{
    // скажем, что мы уже находимся на открывающей скобки, далее нужно в тупую пройтись по всем элементам
    // после закрывающей скобки будет to и назавине таблицы, нужно их просто распарсить и вернуть true/false
    return false;
}

} // namespace parser
} // namespace memdb


