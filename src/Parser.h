#pragma once
#define _USE_MATH_DEFINES
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <cctype>
#include <math.h>
#include <numeric>
#include <stdexcept>
#include <algorithm>
namespace parser {

    inline std::unordered_map<std::string, double> constants = {
        {"pi", M_PI},
        {"e", M_E},
        {"tau", 2 * M_PI},
        {"phi", 1.61803398875},
        {"c", 299792458},
        {"g", 9.80665},
        {"h", 6.62607015e-34},
        {"k", 5}
    };
    inline const std::unordered_map<std::string, std::function<double(double)>> funcs = {
        {"sin",   std::function<double(double)>(static_cast<double(*)(double)>(std::sin))},
        {"cos",   std::function<double(double)>(static_cast<double(*)(double)>(std::cos))},
        {"tan",   std::function<double(double)>(static_cast<double(*)(double)>(std::tan))},
        {"asin",  std::function<double(double)>(static_cast<double(*)(double)>(std::asin))},
        {"acos",  std::function<double(double)>(static_cast<double(*)(double)>(std::acos))},
        {"atan",  std::function<double(double)>(static_cast<double(*)(double)>(std::atan))},
        {"sinh",  std::function<double(double)>(static_cast<double(*)(double)>(std::sinh))},
        {"cosh",  std::function<double(double)>(static_cast<double(*)(double)>(std::cosh))},
        {"tanh",  std::function<double(double)>(static_cast<double(*)(double)>(std::tanh))},
        {"log",   std::function<double(double)>(static_cast<double(*)(double)>(std::log10))},
        {"ln",    std::function<double(double)>(static_cast<double(*)(double)>(std::log))},
        {"exp",   std::function<double(double)>(static_cast<double(*)(double)>(std::exp))},
        {"abs",   std::function<double(double)>(static_cast<double(*)(double)>(std::fabs))},
        {"sqrt",  std::function<double(double)>(static_cast<double(*)(double)>(std::sqrt))},
        {"cbrt",  std::function<double(double)>(static_cast<double(*)(double)>(std::cbrt))},
        {"ceil",  std::function<double(double)>(static_cast<double(*)(double)>(std::ceil))},
        {"floor", std::function<double(double)>(static_cast<double(*)(double)>(std::floor))},
        {"round", std::function<double(double)>(static_cast<double(*)(double)>(std::round))},
        {"trunc", std::function<double(double)>(static_cast<double(*)(double)>(std::trunc))},
        {"sign",  std::function<double(double)>([](double x) {
            return x < 0 ? -1.0 : (x > 0 ? 1.0 : 0.0);
        })},
        {"min",   std::function<double(double)>([](double x) { return x; })},
        {"max",   std::function<double(double)>([](double x) { return x; })},
        {"gcd",   std::function<double(double)>([](double x) { return x; })},
        {"lcm",   std::function<double(double)>([](double x) { return x; })},
        { "deg", [](double x) { return x * M_PI / 180.0; } }

    };




    inline void addConstant(std::string name, double val) {
        constants[name] = val;
    }

    inline bool isVar(const std::string& x) {
        return x == "x";
    }

    inline bool isNumber(char x) {
        return std::isdigit(x);
    }

    inline bool isSym(char x) {
        return x == '+' || x == '-' || x == '*' || x == '/' || x == '^';
    }

    inline bool isConstant(const std::string& x) {
        return constants.find(x) != constants.end();
    }

    inline bool isFunction(const std::string& x) {
        return funcs.find(x) != funcs.end();
    }

    inline std::function<double(double)> getFunction(const std::string& x) {
        return funcs.at(x);
    }


    class Expr {
        public:
            virtual double eval(double x = 0.0) const = 0;
            virtual std::string repr() const = 0;
            virtual std::string deriv() const = 0;
            virtual std::shared_ptr<Expr> simplify() const = 0;

            virtual ~Expr() = default;
        };

    class Lit : public Expr {
            double value;
        public:
            Lit(double val) : value(val) {}
            double eval(double x = 0.0) const override { return value; }
            std::string repr() const override { return std::to_string(value); }
            std::string deriv() const override { return "0"; }
            std::shared_ptr<Expr> simplify() const override {
                return std::make_shared<Lit>(value);
            }
        };

    class Var : public Expr {
        public:
            double eval(double x = 0.0) const override { return x; }
            std::string repr() const override { return "x"; }
            std::string deriv() const override { return "1"; }
            std::shared_ptr<Expr> simplify() const override {
                return std::make_shared<Var>();
            }

        };

    class FnExpr : public Expr {
        std::string name;
        std::function<double(double)> func;
    public:
        std::shared_ptr<Expr> argument;

        FnExpr(const std::string& fname, std::function<double(double)> f, std::shared_ptr<Expr> arg)
            : name(fname), func(f), argument(arg) {
        }

        double eval(double x = 0.0) const override {
            return func(argument->eval(x));
        }

        std::string repr() const override {
            return name + "(" + argument->repr() + ")";
        }
            std::string deriv() const override {
                std::string arg = argument->repr();
                std::string darg = argument->deriv();

                if (name == "sin")    return "cos(" + arg + ") * " + darg;
                if (name == "cos")    return "-sin(" + arg + ") * " + darg;
                if (name == "tan")    return "(1 + tan(" + arg + ")^2) * " + darg;
                if (name == "asin")   return "(1 / sqrt(1 - " + arg + "^2)) * " + darg;
                if (name == "acos")   return "(-1 / sqrt(1 - " + arg + "^2)) * " + darg;
                if (name == "atan")   return "(1 / (1 + " + arg + "^2)) * " + darg;
                if (name == "sinh")   return "cosh(" + arg + ") * " + darg;
                if (name == "cosh")   return "sinh(" + arg + ") * " + darg;
                if (name == "tanh")   return "(1 - tanh(" + arg + ")^2) * " + darg;
                if (name == "log")    return "(1 / (" + arg + " * ln(10))) * " + darg;
                if (name == "ln")     return "(1 / " + arg + ") * " + darg;
                if (name == "exp")    return "exp(" + arg + ") * " + darg;
                if (name == "abs")    return "(" + arg + " / abs(" + arg + ")) * " + darg;
                if (name == "sqrt")   return "(1 / (2 * sqrt(" + arg + "))) * " + darg;
                if (name == "cbrt")   return "(1 / (3 * cbrt(" + arg + ")^2)) * " + darg;
                if (name == "ceil")   return "0";
                if (name == "floor")  return "0";
                if (name == "round")  return "0";
                if (name == "trunc")  return "0";
                if (name == "sign")   return "0";
                if (name == "min")    return "0";
                if (name == "max")    return "0";
                if (name == "gcd")    return "0";
                if (name == "lcm")    return "0";

                return name + "'(" + arg + ") * " + darg; 
            }
            std::shared_ptr<Expr> simplify() const override {
                auto simplifiedArg = argument->simplify();
                return std::make_shared<FnExpr>(name,func, simplifiedArg);
            }

        };

    class Operation : public Expr {
        public:
            std::shared_ptr<Expr> left;
            std::shared_ptr<Expr> right;
            Operation(std::shared_ptr<Expr> l, std::shared_ptr<Expr> r) : left(l), right(r) {}
        };

    class Add : public Operation {
        public:
            Add(std::shared_ptr<Expr> l, std::shared_ptr<Expr> r) : Operation(l, r) {}
            double eval(double x = 0.0) const override { return left->eval(x) + right->eval(x); }
            std::string repr() const override { return "(" + left->repr() + " + " + right->repr() + ")"; }
            std::string deriv() const override { return "(" + left->deriv() + " + " + right->deriv() + ")";}
            std::shared_ptr<Expr> simplify() const override {
                auto l = left->simplify();
                auto r = right->simplify();

                auto litL = dynamic_cast<Lit*>(l.get());
                auto litR = dynamic_cast<Lit*>(r.get());

                if (litL && litL->eval() == 0) return r;
                if (litR && litR->eval() == 0) return l;

                if (litL && litR) return std::make_shared<Lit>(litL->eval() + litR->eval());

                return std::make_shared<Add>(l, r);
            }
        };

    class Sub : public Operation {
        public:
            Sub(std::shared_ptr<Expr> l, std::shared_ptr<Expr> r) : Operation(l, r) {}
            double eval(double x = 0.0) const override { return left->eval(x) - right->eval(x); }
            std::string repr() const override { return "(" + left->repr() + " - " + right->repr() + ")"; }
            std::string deriv() const override { return "(" + left->deriv() + " - " + right->deriv() + ")"; }
            std::shared_ptr<Expr> simplify() const override {
                auto l = left->simplify();
                auto r = right->simplify();
                auto litL = dynamic_cast<Lit*>(l.get());
                auto litR = dynamic_cast<Lit*>(r.get());

                if (auto litR = dynamic_cast<Lit*>(r.get()); litR && litR->eval() == 0) return l;

                if (auto litL = dynamic_cast<Lit*>(l.get()); litL && litR)
                    return std::make_shared<Lit>(litL->eval() - litR->eval());

                return std::make_shared<Sub>(l, r);
            }
        };

    class Mul : public Operation {
        public:
            Mul(std::shared_ptr<Expr> l, std::shared_ptr<Expr> r) : Operation(l, r) {}
            double eval(double x = 0.0) const override { return left->eval(x) * right->eval(x); }
            std::string repr() const override { return "(" + left->repr() + " * " + right->repr() + ")"; }
            std::string deriv() const override {
                return "(" + left->deriv() + " * " + right->repr() + " + " +
                    left->repr() + " * " + right->deriv() + ")";
            }
            std::shared_ptr<Expr> simplify() const override {
                auto l = left->simplify();
                auto r = right->simplify();

                if (auto litL = dynamic_cast<Lit*>(l.get()); litL) {
                    if (litL->eval() == 0) return std::make_shared<Lit>(0);
                    if (litL->eval() == 1) return r;
                }

                if (auto litR = dynamic_cast<Lit*>(r.get()); litR) {
                    if (litR->eval() == 0) return std::make_shared<Lit>(0);
                    if (litR->eval() == 1) return l;
                }

                if (auto litL = dynamic_cast<Lit*>(l.get()), litR = dynamic_cast<Lit*>(r.get()); litL && litR)
                    return std::make_shared<Lit>(litL->eval() * litR->eval());

                return std::make_shared<Mul>(l, r);
            }

        };

    class Div : public Operation {
        public:
            Div(std::shared_ptr<Expr> l, std::shared_ptr<Expr> r) : Operation(l, r) {}
            double eval(double x = 0.0) const override { return left->eval(x) / right->eval(x); }
            std::string repr() const override { return "(" + left->repr() + " / " + right->repr() + ")"; }
            std::string deriv() const override {
                return "((" + left->deriv() + " * " + right->repr() + " - " +
                    left->repr() + " * " + right->deriv() + ") / (" +
                    right->repr() + " ^ 2))";
            }
            std::shared_ptr<Expr> simplify() const override {
                auto l = left->simplify();
                auto r = right->simplify();

                if (auto litL = dynamic_cast<Lit*>(l.get()); litL && litL->eval() == 0)
                    return std::make_shared<Lit>(0);

                if (auto litR = dynamic_cast<Lit*>(r.get()); litR && litR->eval() == 1)
                    return l;

                if (auto litL = dynamic_cast<Lit*>(l.get()), litR = dynamic_cast<Lit*>(r.get()); litL && litR)
                    return std::make_shared<Lit>(litL->eval() / litR->eval());

                return std::make_shared<Div>(l, r);
            }

        };

    class Pow : public Operation {
        public:
            Pow(std::shared_ptr<Expr> l, std::shared_ptr<Expr> r) : Operation(l, r) {}
            double eval(double x = 0.0) const override { return std::pow(left->eval(x), right->eval(x)); }
            std::string repr() const override { return "(" + left->repr() + " ^ " + right->repr() + ")"; }
            std::string deriv() const override {
                auto leftLit = dynamic_cast<Lit*>(left.get());
                auto rightLit = dynamic_cast<Lit*>(right.get());
                auto leftVar = dynamic_cast<Var*>(left.get());

                if (leftVar && rightLit) {
                    double n = rightLit->eval();
                    return std::to_string(n) + " * x ^ " + std::to_string(n - 1);
                }

                if (leftLit && !rightLit) {
                    std::string base = left->repr();
                    std::string expo = right->repr();
                    std::string dexpo = right->deriv();
                    return "ln(" + base + ") * " + base + "^(" + expo + ") * " + dexpo;
                }

                if (rightLit && !leftLit) {
                    double c = rightLit->eval();
                    std::string fx = left->repr();
                    std::string dfx = left->deriv();
                    return std::to_string(c) + " * " + fx + " ^ " + std::to_string(c - 1) + " * " + dfx;
                }

                std::string fx = left->repr();
                std::string gx = right->repr();
                std::string dfx = left->deriv();
                std::string dgx = right->deriv();

                return "(" + repr() + ") * (" + dgx + " * ln(" + fx + ") + " + gx + " * " + dfx + " / " + fx + ")";
            }
            std::shared_ptr<Expr> simplify() const override {
                auto l = left->simplify();
                auto r = right->simplify();

                if (auto litR = dynamic_cast<Lit*>(r.get()); litR) {
                    double exp = litR->eval();
                    if (exp == 0) return std::make_shared<Lit>(1);
                    if (exp == 1) return l;
                }

                if (auto litL = dynamic_cast<Lit*>(l.get()), litR = dynamic_cast<Lit*>(r.get()); litL && litR)
                    return std::make_shared<Lit>(std::pow(litL->eval(), litR->eval()));

                return std::make_shared<Pow>(l, r);
            }

        };

    class Mod : public Expr {
        public:
            std::shared_ptr<Expr> expr;
            Mod(std::shared_ptr<Expr> e) : expr(e) {}
            double eval(double x = 0.0) const override { return std::fabs(expr->eval(x)); }
            std::string repr() const override { return "|" + expr->repr() + "|"; }
            std::string deriv() const override {
                std::string fx = expr->repr();
                std::string dfx = expr->deriv();
                return "((" + fx + " / |" + fx + "|) * " + dfx + ")";
            }
            std::shared_ptr<Expr> simplify() const override {
                auto simplifiedExpr = expr->simplify();

                if (auto lit = dynamic_cast<Lit*>(simplifiedExpr.get()))
                    return std::make_shared<Lit>(std::fabs(lit->eval()));

                return std::make_shared<Mod>(simplifiedExpr);
            }

        };

    class Parser {
        std::string str;
        int pos;

        int getPrecedence(char op) {
            switch (op) {
            case '+': case '-': return 1;
            case '*': case '/': return 2;
            case '^': return 3;
            default: return -1;
            }
        }
        void skipWhitespace() {
            while (pos < str.size() && std::isspace(str[pos])) pos++;
        }
        std::shared_ptr<Expr> parseExpression(int i) {
            auto left = parsePrimary();
            skipWhitespace();

            while (pos < str.size()) {
                char op = str[pos];
                int prec = getPrecedence(op);
                if (prec < i) break;

                pos++; 
                auto right = parseExpression(prec + (op == '^' ? 0 : 1)); 

                if (op == '+') left = std::make_shared<Add>(left, right);
                else if (op == '-') left = std::make_shared<Sub>(left, right);
                else if (op == '*') left = std::make_shared<Mul>(left, right);
                else if (op == '/') left = std::make_shared<Div>(left, right);
                else if (op == '^') left = std::make_shared<Pow>(left, right);
            }

            return left;
        }

        std::shared_ptr<Expr> parsePrimary() {
            skipWhitespace();
            if (pos >= str.size()) {
                throw std::runtime_error("Unexpected end : '" + str + "'");
            }
            if (str[pos] == '-') {
                pos++;
                return std::make_shared<Mul>(std::make_shared<Lit>(-1), parsePrimary());
            }

            if (str[pos] == '|') {
                int start = ++pos;
                int depth = 1;
                while (pos < str.size()) {
                    if (str[pos] == '|') {
                        depth--;
                        if (depth == 0) break;
                    }
                    else if (str[pos] == '|') {
                        depth++;
                    }
                    pos++;
                }

                if (pos >= str.size() || str[pos] != '|')
                    throw std::runtime_error("Expected closing '|' for absolute value");

                std::string innerStr = str.substr(start, pos - start);
                parser::Parser inner(innerStr);
                inner.Parse();

                pos++; 
                return std::make_shared<Mod>(inner.ast.back());
            }

            if (str[pos] == '(') {
                pos++;
                auto expr = parseExpression(0);
                if (pos >= str.size() || str[pos] != ')') throw std::runtime_error("Expected ')'");
                pos++;
                return expr;
            }

            if (std::isdigit(str[pos]) || str[pos] == '.') {
                int start = pos;
                while (pos < str.size() && (std::isdigit(str[pos]) || str[pos] == '.')) pos++;
                return std::make_shared<Lit>(std::stod(str.substr(start, pos - start)));
            }
            if (str.compare(pos, 5, "d/dx(") == 0){
                pos += 5;
                int start = pos;
                int depth = 1;
                while (pos < str.size() && depth > 0) {
                    if (str[pos] == '(') depth++;
                    else if (str[pos] == ')') depth--;
                    pos++;
                }
                if (depth != 0) throw std::runtime_error("Unmatched parentheses in d/dx");

                std::string inner = str.substr(start, pos - start - 1); 

                parser::Parser innerParser(inner);
                innerParser.Parse();

                auto derived = innerParser.ast.back()->deriv(); 

                parser::Parser dParser(derived);
                dParser.Parse();

                return dParser.ast.back();
            }
            if (std::isalpha(str[pos])) {
                std::string id;
                while (pos < str.size() && (std::isalnum(str[pos]) || str[pos] == '_'))
                    id += str[pos++];
                if (id == "x") return std::make_shared<Var>();
                if (isConstant(id)) return std::make_shared<Lit>(constants.at(id));

                if (isFunction(id)) {
                    if (pos >= str.size() || str[pos] != '(')
                        throw std::runtime_error("Expected '(' after function name: " + id);

                    pos++;
                    auto arg = parseExpression(0);

                    if (pos >= str.size() || str[pos] != ')')
                        throw std::runtime_error("Expected ')' after function argument: " + id);

                    pos++;

                    return std::make_shared<FnExpr>(id, getFunction(id), arg);
                }

                throw std::runtime_error("Unknown identifier: " + id);
            }

            throw std::runtime_error("Unexpected character at parsePrimary: '" + std::string(1, str[pos]) + "'");
        }


    public:
        Parser(std::string input) : str(std::move(input)) {
            str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
        }
        std::vector<std::shared_ptr<Expr>> ast;

        bool isFunctionExpr() const {
            if (ast.empty()) return false;
            std::function<bool(std::shared_ptr<Expr>)> check;
            check = [&](std::shared_ptr<Expr> node) -> bool {
                if (!node) return false;
                if (dynamic_cast<Var*>(node.get())) return true;

                if (auto fn = dynamic_cast<FnExpr*>(node.get()))
                    return check(fn->argument);

                if (auto op = dynamic_cast<Operation*>(node.get()))
                    return check(op->left) || check(op->right);

                if (auto mod = dynamic_cast<Mod*>(node.get()))
                    return check(mod->expr);

                return false;
                };
            return check(ast.back());
        }

        void Parse() {
            ast.clear(); 
            pos = 0; 
            auto expr = parseExpression(0);
            if (!expr) throw std::runtime_error("Unexpected end");
            ast.push_back(expr);
        }


        double eval(double x = 0.0) const {
            if (ast.empty()) throw std::runtime_error("Empty AST");
            return ast.back()->eval(x);
        }


        std::function<double(double)> toFunction() const {
            if (ast.empty()) throw std::runtime_error("Empty AST");
            auto expr = ast.back();
            return [expr](double x) { return expr->eval(x); };
        }
    };
}