#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <cctype>
#include <map>
#include <iomanip>
// Context
class Context {
    // Any global information needed for interpretation can be added here
};

// Abstract Expression Interface
class Expression {
public:
    virtual double interpret(Context& context) = 0;
    virtual ~Expression() {} // Virtual destructor for correct polymorphic behavior
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
        std::vector<std::string> tokens = tokenize(input);
        Expression* expressionTree = buildExpressionTree(tokens);
        double result = expressionTree->interpret(*context);
        delete expressionTree;
        return result;
    }

private:
    std::vector<std::string> tokenize(const std::string& input) {
        std::vector<std::string> tokens;
        std::string number;
        
        for (size_t i = 0; i < input.size(); i++) {
            char c = input[i];
            if (std::isdigit(c) || c == '.') {
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
        std::stack<int> parens;
        
        for (size_t i = 0; i < tokens.size(); i++) {
            const std::string& token = tokens[i];
            if (std::isdigit(token[0]) || token.find('.') != std::string::npos) {
                values.push(new NumberExpression(std::stod(token)));
            } else if (token == "(" ) {
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
};

// Client (Main function)
int main() {
    int i=0;
    std::string input;
    std::cout<< std::setw(15) <<std::setfill('*') << ""<<std::endl;
    std::cout << "Hello!! \nWelcome!"<<std::endl;
    do{
        std::cout << "Enter any expression ( e.g., '2+3.2*4' or '(12+1+1)*2'  or '10 2 3 * +' ): "<<std::endl;
        std::getline(std::cin, input);
        if (input!="0" &&input!="end"&&input!="End"&&input!="END"){
            Context* context = new Context();
            Interpreter interpreter(context);
            
            try {
                double result = interpreter.interpret(input);
                std::cout << "The Result is: " << result << std::endl;
                std::cout<< std::setw(15) <<std::setfill('*') << ""<<std::endl;
            } catch (const std::exception& e) {
                std::cout << "Error: "<< e.what() << std::endl;
            }
            delete context;
        }     
        else{
            i=1;
            std::cout << "Thank You!!"<< std::endl;
            std::cout<< std::setw(15) <<std::setfill('*') << ""<<std::endl;
        }
        
    }while(i==0);
    
    return 0;
}

