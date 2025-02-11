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
    std::ofstream logFile;

public:
    Interpreter(Context* context) : context(context), logFile("calculations.log", std::ios::app) {}

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
            std::string result = stream.str();
            logFile << input << " = " << result << std::endl;
            return result;
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
        } else if (input.find("sin(") == 0) {
            return evaluateTrigFunction(input, "sin");
        } else if (input.find("cos(") == 0) {
            return evaluateTrigFunction(input, "cos");
        } else if (input.find("tan(") == 0) {
            return evaluateTrigFunction(input, "tan");
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

    double evaluateTrigFunction(const std::string& input, const std::string& func) {
        size_t start = input.find('(');
        size_t end = input.find(')');
        if (start == std::string::npos || end == std::string::npos || start >= end) {
            throw std::runtime_error("Invalid function syntax");
        }
        std::string arg = input.substr(start + 1, end - start - 1);
        double value = evaluateExpression(arg);
        double radians = value * M_PI / 180.0; 
        if (func == "sin") return std::sin(radians);
        if (func == "cos") return std::cos(radians);
        if (func == "tan") return std::tan(radians);
        throw std::runtime_error("Unknown function: " + func);
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
    while (true) {
        std::cout << "Enter expression: ";
        std::getline(std::cin, input);
        if (input == "exit") break;
        std::string result = interpreter.interpret(input);
        if (!result.empty()) {
            std::cout << "Result: " << result << std::endl;
        }
    }
    return 0;
}
