#include <map>
#include <vector>
#include <cstdint>
#include <variant>
#include <stack>
#include <string>
#include <memory>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include "Config.h"

class Statement {
public:
    virtual std::vector<config::ColumnValue> apply(std::vector<config::ColumnValue> in) const = 0;

    virtual ~Statement() = default;
};

class Combine : public Statement {

public:
    Combine(std::shared_ptr<Statement> l, std::shared_ptr<Statement> r): left(l), right(r) {}

    virtual std::vector<config::ColumnValue> apply(std::vector<config::ColumnValue> in) const override {
        return right->apply(std::move(left->apply(std::move(in))));
    }

private:
    std::shared_ptr<Statement> left, right;
};

class SpaceOp : public Statement {
public:
    std::vector<config::ColumnValue> apply(std::vector<config::ColumnValue> in) const override {
        return in;
    }
};

class PlusOp : public Statement {
public:
    std::vector<config::ColumnValue> apply(std::vector<config::ColumnValue> in) const override {
        config::ColumnValue a = in[in.size() - 1];
        config::ColumnValue b = in[in.size() - 2];

        if(std::holds_alternative<int>(a) && std::holds_alternative<int>(b)){
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<int>(a) + std::get<int>(b));
        }

        else if(std::holds_alternative<std::string>(a) && std::holds_alternative<std::string>(b)){
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<std::string>(b) + std::get<std::string>(a));
        }

        else if(std::holds_alternative<int>(a) && std::holds_alternative<std::monostate>(b)){
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<int>(a));
        }

        else if(std::holds_alternative<std::monostate>(a) && std::holds_alternative<int>(b)){
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<int>(b));
        }

        else if(std::holds_alternative<std::string>(a) && std::holds_alternative<std::monostate>(b)){
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<std::string>(a));
        }

        else if(std::holds_alternative<std::monostate>(a) && std::holds_alternative<std::string>(b)){
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<std::string>(b));
        }

        else if(std::holds_alternative<std::monostate>(a) && std::holds_alternative<std::monostate>(b)){
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::monostate());
        }

        else{
            std::cout << "error of type";
        }

        return in;
    }
};

class MinusOp : public Statement {
public:
    std::vector<config::ColumnValue> apply(std::vector<config::ColumnValue> in) const override {
        config::ColumnValue a = in[in.size() - 1];
        config::ColumnValue b = in[in.size() - 2];

        if(std::holds_alternative<int>(a) && std::holds_alternative<int>(b)){
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<int>(b) - std::get<int>(a));
        }

        else if(std::holds_alternative<int>(a) && std::holds_alternative<std::monostate>(b)){
            in.pop_back();
            in.pop_back();
            in.emplace_back(0 - std::get<int>(a));
        }

        else if(std::holds_alternative<std::monostate>(a) && std::holds_alternative<int>(b)){
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<int>(b));
        }

        else if(std::holds_alternative<std::monostate>(a) && std::holds_alternative<std::monostate>(b)){
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::monostate());
        }

        else{
            std::cout << "error of type";
        }

        return in;
    }
};

class MultiplicationOp : public Statement {
public:
    std::vector<config::ColumnValue> apply(std::vector<config::ColumnValue> in) const override {
        config::ColumnValue a = in[in.size() - 1];
        config::ColumnValue b = in[in.size() - 2];

        if (std::holds_alternative<int>(a) && std::holds_alternative<int>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<int>(a) * std::get<int>(b));
        } else if (std::holds_alternative<int>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(0);
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<int>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(0);
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::monostate());
        } else {
            std::cout << "error of type";
        }

        return in;
    }
};

class DivisionOp : public Statement {
public:
    std::vector<config::ColumnValue> apply(std::vector<config::ColumnValue> in) const override {
        config::ColumnValue a = in[in.size() - 1];
        config::ColumnValue b = in[in.size() - 2];

        if (std::holds_alternative<int>(a) && std::holds_alternative<int>(b)) {
            in.pop_back();
            in.pop_back();
            if(std::get<int>(b) != 0){
                in.emplace_back(std::get<int>(b) / std::get<int>(a));
            }else{
                in.emplace_back(std::monostate());
            }
        } else if (std::holds_alternative<int>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            if(std::get<int>(a) != 0){
                in.emplace_back(0);
            }else{
                in.emplace_back(std::monostate());
            }
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<int>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::monostate());
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::monostate());
        } else {
            std::cout << "error of type";
        }

        return in;
    }
};

class RemainOp : public Statement {
public:
    std::vector<config::ColumnValue> apply(std::vector<config::ColumnValue> in) const override {
        config::ColumnValue a = in[in.size() - 1];
        config::ColumnValue b = in[in.size() - 2];

        if (std::holds_alternative<int>(a) && std::holds_alternative<int>(b)) {
            in.pop_back();
            in.pop_back();
            if(std::get<int>(b) != 0){
                in.emplace_back(std::get<int>(b) % std::get<int>(a));
            }else{
                in.emplace_back(std::monostate());
            }
        } else if (std::holds_alternative<int>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            if(std::get<int>(a) != 0){
                in.emplace_back(0);
            }else{
                in.emplace_back(std::monostate());
            }
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<int>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::monostate());
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::monostate());
        } else {
            std::cout << "error of type";
        }

        return in;
    }
};

class LenOp : public Statement {
public:
    std::vector<config::ColumnValue> apply(std::vector<config::ColumnValue> in) const override {
        config::ColumnValue a = in[in.size() - 1];

        if(std::holds_alternative<std::string>(a)){
            in.pop_back();
            in.emplace_back(int(std::get<std::string>(a).size()));
        }

        else if(std::holds_alternative<config::BitString>(a)){
            in.pop_back();
            in.emplace_back(int(std::get<config::BitString>(a).size()/2));
        }

        else if(std::holds_alternative<std::monostate>(a)){
            in.pop_back();
            in.emplace_back(0);
        }
        else{
            std::cout << "error of type";
        }

        return in;
    }
};

class BOp : public Statement {
public:
    std::vector<config::ColumnValue> apply(std::vector<config::ColumnValue> in) const override {
        config::ColumnValue a = in[in.size() - 1];
        config::ColumnValue b = in[in.size() - 2];

        if (std::holds_alternative<int>(a) && std::holds_alternative<int>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<int>(b) > std::get<int>(a));
        } else if (std::holds_alternative<int>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(0 > std::get<int>(a));
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<int>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<int>(b) > 0);
        }

        else if (std::holds_alternative<std::string>(a) && std::holds_alternative<std::string>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<std::string>(b) > std::get<std::string>(a));
        } else if (std::holds_alternative<std::string>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back("" > std::get<std::string>(a));
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<std::string>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<std::string>(b) > "");
        }

        else if (std::holds_alternative<config::BitString>(a) && std::holds_alternative<config::BitString>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<config::BitString>(b) > std::get<config::BitString>(a));
        } else if (std::holds_alternative<config::BitString>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            std::vector<uint8_t> temp;
            in.emplace_back(temp > std::get<config::BitString>(a));
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<config::BitString>(b)) {
            in.pop_back();
            in.pop_back();
            std::vector<uint8_t> temp;
            in.emplace_back(std::get<config::BitString>(b) > temp);
        }

        else if (std::holds_alternative<bool>(a) && std::holds_alternative<bool>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<bool>(b) > std::get<bool>(a));
        } else if (std::holds_alternative<bool>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(false > std::get<bool>(a));
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<bool>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<bool>(b) > false);
        }

        else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::monostate());
        } else {
            std::cout << "error of type";
        }
        return in;
    }
};

class LOp : public Statement {
public:
    std::vector<config::ColumnValue> apply(std::vector<config::ColumnValue> in) const override {
        config::ColumnValue a = in[in.size() - 1];
        config::ColumnValue b = in[in.size() - 2];

        if (std::holds_alternative<int>(a) && std::holds_alternative<int>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<int>(b) < std::get<int>(a));
        } else if (std::holds_alternative<int>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(0 < std::get<int>(a));
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<int>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<int>(b) < 0);
        }

        else if (std::holds_alternative<std::string>(a) && std::holds_alternative<std::string>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<std::string>(b) < std::get<std::string>(a));
        } else if (std::holds_alternative<std::string>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back("" < std::get<std::string>(a));
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<std::string>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<std::string>(b) < "");
        }

        else if (std::holds_alternative<config::BitString>(a) && std::holds_alternative<config::BitString>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<config::BitString>(b) < std::get<config::BitString>(a));
        } else if (std::holds_alternative<config::BitString>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            std::vector<uint8_t> temp;
            in.emplace_back(temp < std::get<config::BitString>(a));
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<config::BitString>(b)) {
            in.pop_back();
            in.pop_back();
            std::vector<uint8_t> temp;
            in.emplace_back(std::get<config::BitString>(b) < temp);
        }

        else if (std::holds_alternative<bool>(a) && std::holds_alternative<bool>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<bool>(b) < std::get<bool>(a));
        } else if (std::holds_alternative<bool>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(false < std::get<bool>(a));
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<bool>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<bool>(b) < false);
        }

        else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::monostate());
        } else {
            std::cout << "error of type";
        }
        return in;
    }
};

class EOp : public Statement {
public:
    std::vector<config::ColumnValue> apply(std::vector<config::ColumnValue> in) const override {
        config::ColumnValue a = in[in.size() - 1];
        config::ColumnValue b = in[in.size() - 2];

        if (std::holds_alternative<int>(a) && std::holds_alternative<int>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<int>(b) == std::get<int>(a));
        } else if (std::holds_alternative<int>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(0 == std::get<int>(a));
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<int>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<int>(b) == 0);
        }

        else if (std::holds_alternative<std::string>(a) && std::holds_alternative<std::string>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<std::string>(b) == std::get<std::string>(a));
        } else if (std::holds_alternative<std::string>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back("" == std::get<std::string>(a));
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<std::string>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<std::string>(b) == "");
        }

        else if (std::holds_alternative<config::BitString>(a) && std::holds_alternative<config::BitString>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<config::BitString>(b) == std::get<config::BitString>(a));
        } else if (std::holds_alternative<config::BitString>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            std::vector<uint8_t> temp;
            in.emplace_back(temp == std::get<config::BitString>(a));
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<config::BitString>(b)) {
            in.pop_back();
            in.pop_back();
            std::vector<uint8_t> temp;
            in.emplace_back(std::get<config::BitString>(b) == temp);
        }

        else if (std::holds_alternative<bool>(a) && std::holds_alternative<bool>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<bool>(b) == std::get<bool>(a));
        } else if (std::holds_alternative<bool>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(false == std::get<bool>(a));
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<bool>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<bool>(b) == false);
        }

        else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::monostate());
        } else {
            std::cout << "error of type";
        }
        return in;
    }
};

class BEOp : public Statement {
public:
    std::vector<config::ColumnValue> apply(std::vector<config::ColumnValue> in) const override {
        config::ColumnValue a = in[in.size() - 1];
        config::ColumnValue b = in[in.size() - 2];

        if (std::holds_alternative<int>(a) && std::holds_alternative<int>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<int>(b) >= std::get<int>(a));
        } else if (std::holds_alternative<int>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(0 >= std::get<int>(a));
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<int>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<int>(b) >= 0);
        }

        else if (std::holds_alternative<std::string>(a) && std::holds_alternative<std::string>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<std::string>(b) >= std::get<std::string>(a));
        } else if (std::holds_alternative<std::string>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back("" >= std::get<std::string>(a));
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<std::string>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<std::string>(b) >= "");
        }

        else if (std::holds_alternative<config::BitString>(a) && std::holds_alternative<config::BitString>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<config::BitString>(b) >= std::get<config::BitString>(a));
        } else if (std::holds_alternative<config::BitString>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            std::vector<uint8_t> temp;
            in.emplace_back(temp >= std::get<config::BitString>(a));
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<config::BitString>(b)) {
            in.pop_back();
            in.pop_back();
            std::vector<uint8_t> temp;
            in.emplace_back(std::get<config::BitString>(b) >= temp);
        }

        else if (std::holds_alternative<bool>(a) && std::holds_alternative<bool>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<bool>(b) >= std::get<bool>(a));
        } else if (std::holds_alternative<bool>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(false >= std::get<bool>(a));
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<bool>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<bool>(b) >= false);
        }

        else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::monostate());
        } else {
            std::cout << "error of type";
        }
        return in;
    }
};

class LEOp : public Statement {
public:
    std::vector<config::ColumnValue> apply(std::vector<config::ColumnValue> in) const override {
        config::ColumnValue a = in[in.size() - 1];
        config::ColumnValue b = in[in.size() - 2];

        if (std::holds_alternative<int>(a) && std::holds_alternative<int>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<int>(b) <= std::get<int>(a));
        } else if (std::holds_alternative<int>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(0 <= std::get<int>(a));
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<int>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<int>(b) <= 0);
        }

        else if (std::holds_alternative<std::string>(a) && std::holds_alternative<std::string>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<std::string>(b) <= std::get<std::string>(a));
        } else if (std::holds_alternative<std::string>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back("" <= std::get<std::string>(a));
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<std::string>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<std::string>(b) <= "");
        }

        else if (std::holds_alternative<config::BitString>(a) && std::holds_alternative<config::BitString>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<config::BitString>(b) <= std::get<config::BitString>(a));
        } else if (std::holds_alternative<config::BitString>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            std::vector<uint8_t> temp;
            in.emplace_back(temp <= std::get<config::BitString>(a));
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<config::BitString>(b)) {
            in.pop_back();
            in.pop_back();
            std::vector<uint8_t> temp;
            in.emplace_back(std::get<config::BitString>(b) <= temp);
        }

        else if (std::holds_alternative<bool>(a) && std::holds_alternative<bool>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<bool>(b) <= std::get<bool>(a));
        } else if (std::holds_alternative<bool>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(false <= std::get<bool>(a));
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<bool>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<bool>(b) <= false);
        }

        else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::monostate());
        } else {
            std::cout << "error of type";
        }
        return in;
    }
};

class NEOp : public Statement {
public:
    std::vector<config::ColumnValue> apply(std::vector<config::ColumnValue> in) const override {
        config::ColumnValue a = in[in.size() - 1];
        config::ColumnValue b = in[in.size() - 2];

        if (std::holds_alternative<int>(a) && std::holds_alternative<int>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<int>(b) != std::get<int>(a));
        } else if (std::holds_alternative<int>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(0 != std::get<int>(a));
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<int>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<int>(b) != 0);
        }

        else if (std::holds_alternative<std::string>(a) && std::holds_alternative<std::string>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<std::string>(b) != std::get<std::string>(a));
        } else if (std::holds_alternative<std::string>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back("" != std::get<std::string>(a));
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<std::string>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<std::string>(b) != "");
        }

        else if (std::holds_alternative<config::BitString>(a) && std::holds_alternative<config::BitString>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<config::BitString>(b) != std::get<config::BitString>(a));
        } else if (std::holds_alternative<config::BitString>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            std::vector<uint8_t> temp;
            in.emplace_back(temp != std::get<config::BitString>(a));
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<config::BitString>(b)) {
            in.pop_back();
            in.pop_back();
            std::vector<uint8_t> temp;
            in.emplace_back(std::get<config::BitString>(b) != temp);
        }

        else if (std::holds_alternative<bool>(a) && std::holds_alternative<bool>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<bool>(b) != std::get<bool>(a));
        } else if (std::holds_alternative<bool>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(false !=  std::get<bool>(a));
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<bool>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<bool>(b) != false);
        }

        else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::monostate());
        } else {
            std::cout << "error of type";
        }
        return in;
    }
};

class NotOp : public Statement {
public:
    std::vector<config::ColumnValue> apply(std::vector<config::ColumnValue> in) const override {
        config::ColumnValue a = in[in.size() - 1];

        if (std::holds_alternative<bool>(a)) {
            in.pop_back();
            in.emplace_back(!std::get<bool>(a));
        } else if (std::holds_alternative<std::monostate>(a)) {
            in.pop_back();
            in.emplace_back(std::monostate());
        } else {
            std::cout << "error of type";
        }
        return in;
    }
};

class AndOp : public Statement {
public:
    std::vector<config::ColumnValue> apply(std::vector<config::ColumnValue> in) const override {
        config::ColumnValue a = in[in.size() - 1];
        config::ColumnValue b = in[in.size() - 2];

        if (std::holds_alternative<bool>(a) && std::holds_alternative<bool>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<bool>(a) && std::get<bool>(b));
        } else if (std::holds_alternative<bool>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(false);
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<int>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(false);
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::monostate());
        } else {
            std::cout << "error of type";
        }

        return in;
    }
};

class OrOp : public Statement {
public:
    std::vector<config::ColumnValue> apply(std::vector<config::ColumnValue> in) const override {
        config::ColumnValue a = in[in.size() - 1];
        config::ColumnValue b = in[in.size() - 2];

        if (std::holds_alternative<bool>(a) && std::holds_alternative<bool>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<bool>(a) || std::get<bool>(b));
        } else if (std::holds_alternative<std::monostate>(a) || std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(true);
        } else {
            std::cout << "error of type";
        }

        return in;
    }
};

class XorOp : public Statement {
public:
    std::vector<config::ColumnValue> apply(std::vector<config::ColumnValue> in) const override {
        config::ColumnValue a = in[in.size() - 1];
        config::ColumnValue b = in[in.size() - 2];

        if (std::holds_alternative<bool>(a) && std::holds_alternative<bool>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(bool(std::get<bool>(a) ^ std::get<bool>(b)));
        } else if (std::holds_alternative<bool>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(!std::get<bool>(a));
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<int>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(!std::get<bool>(b));
        } else if (std::holds_alternative<std::monostate>(a) && std::holds_alternative<std::monostate>(b)) {
            in.pop_back();
            in.pop_back();
            in.emplace_back(false);
        } else {
            std::cout << "error of type";
        }

        return in;
    }
};

class ConstOp : public Statement {
public:
    ConstOp(config::ColumnValue val, int preced): val(val), preced(preced) {};
    std::vector<config::ColumnValue> apply(std::vector<config::ColumnValue> in) const override {
        if(preced != -1){
            config::ColumnValue a = in[preced];
            in.push_back(a);
        }else{
            in.push_back(val);
        };

        return in;
    }
private:
    config::ColumnValue val;
    int preced;
};

std::shared_ptr<Statement> operator|(std::shared_ptr<Statement> lhs, std::shared_ptr<Statement> rhs) {
    return std::make_shared<Combine>(lhs, rhs);
}

bool isNum(const std::string& str){
    if(str[0] != '+' && str[0] != '-' && (str[0]-'0' < 0 || str[0] - '9' > 0)){
        return false;
    }
    for(int i = 1; i < str.size(); ++i){
        if(str[i]-'0' < 0 || str[i] - '9' > 0){
            return false;
        }
    }
    return true;
}

bool isBool(const std::string& str){
    if(str == "true" || str == "false"){
        return true;
    }
    return false;
}

bool isStr(const std::string& str){
    if(str.size() > 1){
        if(str.front() == '\"' && str.back() == '\"'){
            return true;
        }
    }
    return false;
}

bool isBitString(std::string str){
    if(str.size() > 1){
        if(str.front() == '0' && std::tolower(str[1]) == 'x'){
            if(str.size() % 2 == 1){
                std::cout << "error of byteString";
            }
            for(int i = 2 ; i < str.size(); ++i){
                if(!((str[i] - '0' >= 0 || str[i] - '9' <= 0) || (std::tolower(str[i]) - 'a' < 0 || std::tolower(str[i]) - 'f' > 0))){
                    return false;
                }
            }
            return true;
        }
    }
    return false;
}

std::shared_ptr<Statement> compile(const std::vector<std::string>& query, std::vector<std::string>& column_name)
{
    int preced = 0;
    std::string stmt;
    std::shared_ptr<Statement> result = std::make_shared<SpaceOp>();
    std::shared_ptr<Statement> ptr;
    for(int i = 0; i < query.size(); ++i) {
        stmt = query[i];
        if(stmt == "+") {
            ptr = std::make_shared<PlusOp>();
        }
        else if(stmt == "-"){
            ptr = std::make_shared<MinusOp>();
        }
        else if(stmt == "*") {
            ptr = std::make_shared<MultiplicationOp>();
        }
        else if(stmt == "/") {
            ptr = std::make_shared<DivisionOp>();
        }
        else if(stmt == "%") {
            ptr = std::make_shared<RemainOp>();
        }
        else if(stmt == "|"){
            ptr = std::make_shared<LenOp>();
        }
        else if(stmt == ">"){
            ptr = std::make_shared<BOp>();
        }
        else if(stmt == "<"){
            ptr = std::make_shared<LOp>();
        }
        else if(stmt == "="){
            ptr = std::make_shared<EOp>();
        }
        else if(stmt == ">="){
            ptr = std::make_shared<BEOp>();
        }
        else if(stmt == "<="){
            ptr = std::make_shared<LEOp>();
        }
        else if(stmt == "!="){
            ptr = std::make_shared<NEOp>();
        }
        else if(stmt == "!"){
            ptr = std::make_shared<NotOp>();
        }
        else if(stmt == "^^"){
            ptr = std::make_shared<XorOp>();
        }
        else if(stmt == "&&"){
            ptr = std::make_shared<AndOp>();
        }
        else if(stmt == "||"){
            ptr = std::make_shared<OrOp>();
        }
        else if(isNum(stmt)){
            ptr = std::make_shared<ConstOp>(atoi(stmt.c_str()), -1);
        }
        else if(isStr(stmt)){
            ptr = std::make_shared<ConstOp>(stmt.substr(1, stmt.size() - 2), -1);
        }
        else if(isBool(stmt)){
            ptr = std::make_shared<ConstOp>(stmt == "true", -1);
        }
        else if(isBitString(stmt)){
            config::BitString new_stmt(stmt.size()-2);
            for(int j = 2 ; j < stmt.size(); ++j){
                new_stmt[j-2] = stmt[j];
            }
            ptr = std::make_shared<ConstOp>(new_stmt, -1);
        }
        else{
            ptr = std::make_shared<ConstOp>(std::monostate(), preced);
            column_name.push_back(stmt);
            ++preced;
        }
        result = std::make_shared<Combine>(result, ptr);
    }
    return result;
}

int precedence(std::string op)
{
    if (op == "!"){
        return 11;
    }
    if (op == "*" || op == "/" || op == "%")
        return 10;
    if (op == "+" || op == "-") {
        return 9;
    }
    if (op == "<" || op == ">" ||
        op == "<=" || op == ">=")
        return 8;
    if (op == "=" || op == "!=") {
        return 7;
    }
    if (op == "^^"){
        return 6;
    }
    if (op == "&&"){
        return 5;
    }
    if (op == "||"){
        return 4;
    }
    return 0;
}

std::vector<std::string> infixToPostfix(std::vector<std::string> infix)
{
    std::stack<std::string> st;
    std::vector<std::string> postfix;
    bool is_Abs = false;
    for (int i = 0; i < infix.size(); i++) {
        std::string c = infix[i];

        if (!precedence(c) && c != "(" && c != ")" && c != "|"){
            postfix.push_back(c);
        }

        else if(c == "|"){
            if(!is_Abs){
                st.emplace("|");
                is_Abs = true;
            }
            else{
                while (st.top() != "|") {
                    postfix.emplace_back(st.top());
                    st.pop();
                }
                postfix.emplace_back("|");
                st.pop();
                is_Abs = false;
            }
        }

        else if (c == "(")
            st.emplace("(");

        else if (c == ")") {
            while (st.top() != "(") {
                postfix.emplace_back(st.top());
                st.pop();
            }
            st.pop();
        }

        else {
            while (!st.empty()
                   && precedence(c)
                      <= precedence(st.top())) {
                postfix.emplace_back(st.top());
                st.pop();
            }
            st.push(c);
        }
    }

    while (!st.empty()) {
        postfix.emplace_back(st.top());
        st.pop();
    }

    return postfix;
}

std::shared_ptr<Statement> parse_where(const std::string& str, std::vector<std::string>& column_name){
    std::string piece_str;
    std::stringstream all_str(str);
    std::vector<std::string> v;

    while (getline(all_str, piece_str, ' ')) {
        v.push_back(piece_str);
    }

    auto temp = infixToPostfix(v);
    return compile(temp, column_name);
}