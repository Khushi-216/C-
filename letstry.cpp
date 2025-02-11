#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <cctype>
#include <map>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <stdexcept>

// Context
class Context {
public:
    std::map<std::string, double> variables;
};

// Interpreter
class Interpreter {
private:
    Context* context;
    std::map<char, int> precedence = {{'+', 1}, {'-', 1}, {'*', 2}, {'/', 2}, {'%', 2}};

public:
    Interpreter(Context* context) : context(context) {}

    std::string interpret(std::string input) {
        try {
            trim(input);
            size_t eq_pos = input.find('=');
            if (eq_pos != std::string::npos) {
                std::string var = input.substr(0, eq_pos);
                std::string expr = input.substr(eq_pos + 1);
                trim(var);
                trim(expr);
                context->variables[var] = evaluateExpression(expr);
                return "";
            }
            std::ostringstream stream;
            stream << std::fixed << std::setprecision(2) << evaluateExpression(input);
            return stream.str();
        } catch (const std::exception& e) {
            return std::string("Error: ") + e.what();
        }
    }

private:
    double evaluateExpression(const std::string& input) {
        if (context->variables.find(input) != context->variables.end()) {
            return context->variables[input];
        }
        if (input.find("add(") == 0) {
            return evaluateFunction(input, "+");
        } else if (input.find("sub(") == 0) {
            return evaluateFunction(input, "-");
        } else if (input.find("mul(") == 0) {
            return evaluateFunction(input, "*");
        } else if (input.find("div(") == 0) {
            return evaluateFunction(input, "/");
        } else if (input.find("mod(") == 0) {
            return evaluateFunction(input, "%");
        }
        return evaluateMathExpression(tokenize(input));
    }

    double evaluateFunction(const std::string& input, const std::string& op) {
        size_t start = input.find('(');
        size_t end = input.find(')');
        if (start == std::string::npos || end == std::string::npos || start >= end) {
            throw std::runtime_error("Invalid function syntax");
        }
        std::string args = input.substr(start + 1, end - start - 1);
        std::vector<std::string> tokens = split(args, ',');

        double result = evaluateExpression(tokens[0]);
        for (size_t i = 1; i < tokens.size(); ++i) {
            double value = evaluateExpression(tokens[i]);
            if (op == "+") result += value;
            else if (op == "-") result -= value;
            else if (op == "*") result *= value;
            else if (op == "/") {
                if (value == 0) throw std::runtime_error("Division by zero");
                result /= value;
            } else if (op == "%") {
                if (value == 0) throw std::runtime_error("Modulo by zero");
                result = std::fmod(result, value);
            }
        }
        return result;
    }

    std::vector<std::string> tokenize(const std::string& input) {
        std::vector<std::string> tokens;
        std::string token;
        bool lastWasOperator = true;
        for (size_t i = 0; i < input.size(); i++) {
            char c = input[i];
            if (std::isdigit(c) || c == '.' || (c == '-' && lastWasOperator)) {
                token += c;
                lastWasOperator = false;
            } else {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
                if (c != ' ') {
                    tokens.push_back(std::string(1, c));
                    lastWasOperator = true;
                }
            }
        }
        if (!token.empty()) {
            tokens.push_back(token);
        }
        return tokens;
    }

    double evaluateMathExpression(const std::vector<std::string>& tokens) {
        std::stack<double> values;
        std::stack<char> operators;
        for (const std::string& token : tokens) {
            if (std::isdigit(token[0]) || token.find('.') != std::string::npos || (token[0] == '-' && token.size() > 1)) {
                values.push(std::stod(token));
            } else if (context->variables.find(token) != context->variables.end()) {
                values.push(context->variables[token]);
            } else if (token == "(") {
                operators.push('(');
            } else if (token == ")") {
                while (!operators.empty() && operators.top() != '(') {
                    applyOperator(values, operators);
                }
                operators.pop();
            } else if (precedence.find(token[0]) != precedence.end()) {
                while (!operators.empty() && operators.top() != '(' && precedence[operators.top()] >= precedence[token[0]]) {
                    applyOperator(values, operators);
                }
                operators.push(token[0]);
            } else {
                throw std::runtime_error("Undefined variable or invalid input: " + token);
            }
        }
        while (!operators.empty()) {
            applyOperator(values, operators);
        }
        return values.top();
    }

    void applyOperator(std::stack<double>& values, std::stack<char>& operators) {
        char op = operators.top(); operators.pop();
        double right = values.top(); values.pop();
        double left = values.top(); values.pop();
        switch (op) {
            case '+': values.push(left + right); break;
            case '-': values.push(left - right); break;
            case '*': values.push(left * right); break;
            case '/': if (right == 0) throw std::runtime_error("Division by zero"); values.push(left / right); break;
            case '%': if (right == 0) throw std::runtime_error("Modulo by zero"); values.push(std::fmod(left, right)); break;
        }
    }
    
    void trim(std::string& str) {
        str.erase(0, str.find_first_not_of(" "));
        str.erase(str.find_last_not_of(" ") + 1);
    }

    std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, delimiter)) {
            trim(token);
            tokens.push_back(token);
        }
        return tokens;
    }
};

int main() {
    Context context;
    Interpreter interpreter(&context);
    std::string input;
    std::ofstream history("history.txt", std::ios::app);
    while (true) {
        std::cout << "Enter expression: ";
        std::getline(std::cin, input);
        if (input == "exit") break;
        std::string result = interpreter.interpret(input);
        history << "Input: " << input << "\nResult: " << result << "\n";
        if (!result.empty()) std::cout << "Result: " << result << std::endl;
    }
    history.close();
    return 0;
}
