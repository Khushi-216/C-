#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <cctype>
#include <map>
#include <iomanip>
#include <sstream>

// Context
class Context {
public:
    std::map<std::string, double> variables;
};

// Abstract Expression Interface
class Expression {
public:
    virtual double interpret(Context& context) = 0;
    virtual ~Expression() {} 
};

// Terminal Expression (NumberExpression)
class NumberExpression : public Expression {
private:
    double number;

public:
    NumberExpression(double number) : number(number) {}
    double interpret(Context& context) override {
        return number;
    }
};

// Terminal Expression (VariableExpression)
class VariableExpression : public Expression {
private:
    std::string name;

public:
    VariableExpression(const std::string& name) : name(name) {}
    double interpret(Context& context) override {
        if (context.variables.find(name) != context.variables.end()) {
            return context.variables[name];
        }
        throw std::runtime_error("Undefined variable: " + name);
    }
};

// Non-Terminal Expressions
class BinaryExpression : public Expression {
protected:
    Expression* left;
    Expression* right;

public:
    BinaryExpression(Expression* left, Expression* right) : left(left), right(right) {}
    virtual ~BinaryExpression() {
        delete left;
        delete right;
    }
};

class AdditionExpression : public BinaryExpression {
public:
    AdditionExpression(Expression* left, Expression* right) : BinaryExpression(left, right) {}
    double interpret(Context& context) override {
        return left->interpret(context) + right->interpret(context);
    }
};

class SubtractionExpression : public BinaryExpression {
public:
    SubtractionExpression(Expression* left, Expression* right) : BinaryExpression(left, right) {}
    double interpret(Context& context) override {
        return left->interpret(context) - right->interpret(context);
    }
};

class MultiplicationExpression : public BinaryExpression {
public:
    MultiplicationExpression(Expression* left, Expression* right) : BinaryExpression(left, right) {}
    double interpret(Context& context) override {
        return left->interpret(context) * right->interpret(context);
    }
};

class DivisionExpression : public BinaryExpression {
public:
    DivisionExpression(Expression* left, Expression* right) : BinaryExpression(left, right) {}
    double interpret(Context& context) override {
        double denominator = right->interpret(context);
        if (denominator == 0) throw std::runtime_error("Division by zero error");
        return left->interpret(context) / denominator;
    }
};

// Interpreter
class Interpreter {
private:
    Context* context;
    std::map<char, int> precedence = {{'+', 1}, {'-', 1}, {'*', 2}, {'/', 2}};

public:
    Interpreter(Context* context) : context(context) {}
    
    double interpret(std::string input) {
        std::istringstream iss(input);
        std::string token;
        while (std::getline(iss, token, ',')) {
            size_t eq_pos = token.find('=');
            if (eq_pos != std::string::npos) {
                std::string var = token.substr(0, eq_pos);
                std::string expr = token.substr(eq_pos + 1);
                trim(var);
                trim(expr);
                Expression* expressionTree = buildExpressionTree(tokenize(expr));
                double value = expressionTree->interpret(*context);
                context->variables[var] = value;
                delete expressionTree;
            } else {
                Expression* expressionTree = buildExpressionTree(tokenize(token));
                double result = expressionTree->interpret(*context);
                delete expressionTree;
                return result;
            }
        }
        return 0;
    }

private:
    std::vector<std::string> tokenize(const std::string& input) {
        std::vector<std::string> tokens;
        std::string number;
        
        for (size_t i = 0; i < input.size(); i++) {
            char c = input[i];
            if (std::isalnum(c) || c == '.') {
                number += c;
            } else {
                if (!number.empty()) {
                    tokens.push_back(number);
                    number.clear();
                }
                if (c == '+' || c == '-' || c == '*' || c == '/' || c == '(' || c == ')') {
                    tokens.push_back(std::string(1, c));
                }
            }
        }
        if (!number.empty()) {
            tokens.push_back(number);
        }
        return tokens;
    }

    Expression* buildExpressionTree(const std::vector<std::string>& tokens) {
        std::stack<Expression*> values;
        std::stack<char> operators;
        
        for (const std::string& token : tokens) {
            if (std::isdigit(token[0]) || token.find('.') != std::string::npos) {
                values.push(new NumberExpression(std::stod(token)));
            } else if (std::isalpha(token[0])) {
                values.push(new VariableExpression(token));
            } else if (token == "(") {
                operators.push('(');
            } else if (token == ")") {
                while (!operators.empty() && operators.top() != '(') {
                    applyOperator(values, operators);
                }
                operators.pop();
            } else if (token == "+" || token == "-" || token == "*" || token == "/") {
                while (!operators.empty() && operators.top() != '(' && precedence[operators.top()] >= precedence[token[0]]) {
                    applyOperator(values, operators);
                }
                operators.push(token[0]);
            }
        }
        
        while (!operators.empty()) {
            applyOperator(values, operators);
        }
        return values.top();
    }

    void applyOperator(std::stack<Expression*>& values, std::stack<char>& operators) {
        char op = operators.top(); operators.pop();
        Expression* right = values.top(); values.pop();
        Expression* left = values.top(); values.pop();
        
        switch (op) {
            case '+': values.push(new AdditionExpression(left, right)); break;
            case '-': values.push(new SubtractionExpression(left, right)); break;
            case '*': values.push(new MultiplicationExpression(left, right)); break;
            case '/': values.push(new DivisionExpression(left, right)); break;
        }
    }
    
    void trim(std::string& str) {
        str.erase(0, str.find_first_not_of(" "));
        str.erase(str.find_last_not_of(" ") + 1);
    }
};

int main() {
    std::string input;
    Context context;
    Interpreter interpreter(&context);
    
    while (true) {
        std::cout << "Enter expression (or type 'end' to exit): ";
        std::getline(std::cin, input);
        if (input == "end") break;
        try {
            double result = interpreter.interpret(input);
            std::cout << "Result: " << result << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
    }
    return 0;
}