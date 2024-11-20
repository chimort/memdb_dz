//
// Created by kait on 11/20/24.
//

#ifndef MEMDB_DZ_WHEREPARSER_H
#define MEMDB_DZ_WHEREPARSER_H
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

using BitString = std::vector<uint8_t>;
using ColumnValue = std::variant<int, std::string, bool, BitString, std::monostate>;
using RowType = std::unordered_map<std::string, ColumnValue>;

class Statement {
public:
    virtual std::vector<ColumnValue> apply(std::vector<ColumnValue> in) const = 0;

    virtual ~Statement() = default;
};

class Combine : public Statement {

public:
    Combine(std::shared_ptr<Statement> l, std::shared_ptr<Statement> r): left(l), right(r) {}

    virtual std::vector<ColumnValue> apply(std::vector<ColumnValue> in) const override {
        return right->apply(std::move(left->apply(std::move(in))));
    }

private:
    std::shared_ptr<Statement> left, right;
};

class SpaceOp : public Statement {
public:
    std::vector<ColumnValue> apply(std::vector<ColumnValue> in) const override {
        return in;
    }
};

class PlusOp : public Statement {
public:
    std::vector<ColumnValue> apply(std::vector<ColumnValue> in) const override {
        ColumnValue a = in[in.size() - 1];
        ColumnValue b = in[in.size() - 2];

        if(std::holds_alternative<int>(a) && std::holds_alternative<int>(b)){
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<int>(a) + std::get<int>(b));
        }

        else if(std::holds_alternative<std::string>(a) && std::holds_alternative<std::string>(b)){
            in.pop_back();
            in.pop_back();
            in.emplace_back(std::get<std::string>(a) + std::get<std::string>(b));
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
    std::vector<ColumnValue> apply(std::vector<ColumnValue> in) const override {
        ColumnValue a = in[in.size() - 1];
        ColumnValue b = in[in.size() - 2];

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
    std::vector<ColumnValue> apply(std::vector<ColumnValue> in) const override {
        ColumnValue a = in[in.size() - 1];
        ColumnValue b = in[in.size() - 2];

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
    std::vector<ColumnValue> apply(std::vector<ColumnValue> in) const override {
        ColumnValue a = in[in.size() - 1];
        ColumnValue b = in[in.size() - 2];

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
    std::vector<ColumnValue> apply(std::vector<ColumnValue> in) const override {
        ColumnValue a = in[in.size() - 1];
        ColumnValue b = in[in.size() - 2];

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
    std::vector<ColumnValue> apply(std::vector<ColumnValue> in) const override {
        ColumnValue a = in[in.size() - 1];

        if(std::holds_alternative<std::string>(a)){
            in.pop_back();
            in.emplace_back(int(std::get<std::string>(a).size()));
        }

        else if(std::holds_alternative<BitString>(a)){
            in.pop_back();
            in.emplace_back(int(std::get<BitString>(a).size()));
        }

        else if(std::holds_alternative<std::monostate>(a)){
            in.pop_back();
            in.emplace_back(0);
        }
        else{
            //сообщает об ошибке
        }

        return in;
    }
};
/*
class SpaceOp : public Statement {
public:
    std::vector<ColumnValue> apply(std::vector<ColumnValue> in) const override {
        return in;
    }
};

class SpaceOp : public Statement {
public:
    std::vector<ColumnValue> apply(std::vector<ColumnValue> in) const override {
        return in;
    }
};

class SpaceOp : public Statement {
public:
    std::vector<ColumnValue> apply(std::vector<ColumnValue> in) const override {
        return in;
    }
};

class SpaceOp : public Statement {
public:
    std::vector<ColumnValue> apply(std::vector<ColumnValue> in) const override {
        return in;
    }
};

class SpaceOp : public Statement {
public:
    std::vector<ColumnValue> apply(std::vector<ColumnValue> in) const override {
        return in;
    }
};

class SpaceOp : public Statement {
public:
    std::vector<ColumnValue> apply(std::vector<ColumnValue> in) const override {
        return in;
    }
};

class SpaceOp : public Statement {
public:
    std::vector<ColumnValue> apply(std::vector<ColumnValue> in) const override {
        return in;
    }
};

class SpaceOp : public Statement {
public:
    std::vector<ColumnValue> apply(std::vector<ColumnValue> in) const override {
        return in;
    }
};

class SpaceOp : public Statement {
public:
    std::vector<ColumnValue> apply(std::vector<ColumnValue> in) const override {
        return in;
    }
};

class SpaceOp : public Statement {
public:
    std::vector<ColumnValue> apply(std::vector<ColumnValue> in) const override {
        return in;
    }
};

class SpaceOp : public Statement {
public:
    std::vector<ColumnValue> apply(std::vector<ColumnValue> in) const override {
        return in;
    }
};

class SpaceOp : public Statement {
public:
    std::vector<ColumnValue> apply(std::vector<ColumnValue> in) const override {
        return in;
    }
};

class SpaceOp : public Statement {
public:
    std::vector<ColumnValue> apply(std::vector<ColumnValue> in) const override {
        return in;
    }
};

class SpaceOp : public Statement {
public:
    std::vector<ColumnValue> apply(std::vector<ColumnValue> in) const override {
        return in;
    }
};

class SpaceOp : public Statement {
public:
    std::vector<ColumnValue> apply(std::vector<ColumnValue> in) const override {
        return in;
    }
};

class SpaceOp : public Statement {
public:
    std::vector<ColumnValue> apply(std::vector<ColumnValue> in) const override {
        return in;
    }
};

class SpaceOp : public Statement {
public:
    std::vector<ColumnValue> apply(std::vector<ColumnValue> in) const override {
        return in;
    }
};
*/
class ConstOp : public Statement {
public:
    ConstOp(ColumnValue val, int preced): val(val), preced(preced) {};
    std::vector<ColumnValue> apply(std::vector<ColumnValue> in) const override {
        if(preced != -1){
            ColumnValue a = in[preced];
            in.push_back(a);
        }else{
            in.push_back(val);
        };

        return in;
    }
private:
    ColumnValue val;
    int preced;
};

std::shared_ptr<Statement> operator|(std::shared_ptr<Statement> lhs, std::shared_ptr<Statement> rhs) {
    return std::make_shared<Combine>(lhs, rhs);
}

bool isNum(const std::string& str){
    for(int i = 0; i < str.size(); ++i){
        if(str[i]-'0' < 0 || str[i] - '0' > 9){
            return false;
        }
    }
    return true;
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
        else if(stmt[0] == '+' || stmt[0] == '-') {
            ptr = std::make_shared<ConstOp>(atoi(stmt.c_str()), -1);
        }
        else if(isNum(stmt)){
            ptr = std::make_shared<ConstOp>(atoi(stmt.c_str()), -1);
        }else{
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
    std::string s;
    std::stringstream ss(str);
    std::vector<std::string> v;

    while (getline(ss, s, ' ')) {
        v.push_back(s);
    }

    auto temp = infixToPostfix(v);
    return compile(temp, column_name);
}
/*
int main(){
    std::string s("( ( x + 7 ) * 4 - ( 19 - 2 * y ) ) * 10 - ( a + 100 )");
    RowType ip;
    ip["x"] = 5;
    ip["a"] = std::monostate();
    ip["y"] = 2;
    std::vector<std::string> column_name;
    auto temp = parse_where(s, column_name);
    std::vector<ColumnValue> statement (column_name.size());
    for(int i = 0; i < column_name.size(); ++i){
        statement[i] = ip[column_name[i]];
    }
    auto ans = temp ->apply(statement);
    for(int i = 0 ; i < ans.size(); ++i){
        if(std::holds_alternative<int>(ans[i])){
            std::cout << std::get<int>(ans[i]) << " ";
        }else if(std::holds_alternative<std::string>(ans[i])){
            std::cout << std::get<std::string>(ans[i]) << " ";
        }else if(std::holds_alternative<bool>(ans[i])){
            std::cout << std::get<bool>(ans[i]) << " ";
        }else if(std::holds_alternative<BitString >(ans[i])){
            auto t = std::get<BitString>(ans[i]);
            for(int j = 0; j < t.size(); ++j){
                std::cout << t[j];
            }
            std::cout << " ";
        }else{
            std::cout << "NON ";
        }
    }
}
 */
#endif //MEMDB_DZ_WHEREPARSER_H
