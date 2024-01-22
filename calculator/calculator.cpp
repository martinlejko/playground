//
// Created by Martin Lejko on 10/12/2023.
//
#include "calculator.h"
#include <iostream>
#include <sstream>
#include <stack>
#include <map>


void Calculator::calculate(std::string &expression, const std::string& type) {
    if (type == "prefix") {
        prefix(expression);
    } else if (type == "postfix") {
        std::cout << "parse error" << std::endl;
        exit(0);
    } else {
        std::cout << "parse error" << std::endl;
        exit(0);
    }
}

void Calculator::prefix(std::string &expression) {
    std::stack<int> operands;
    std::string variable;
    std::string equals;

    std::istringstream iss(expression);
    std::string token;
    std::stack<std::string> tokens;
    iss >> variable;
    iss >> equals;
    while (iss >> token) {
        tokens.push(token);
    }

    while (!tokens.empty()) {
        token = tokens.top();
        tokens.pop();

        if (isdigit(token[0])) {
            for (int i = 1; i < token.length(); i++) {
                if (!isdigit(token[i])) {
                    std::cout << "parse error" << std::endl;
                    exit(0);
                }
            }
            operands.push(std::stoi(token));
        } else if (isalpha(token[0])) {
            if (expressions.find(token) != expressions.end()) {
                operands.push(expressions[token]);
            } else {
                std::cout << "parse error" << std::endl;
                exit(0);
            }
        } else if (token == "+" || token == "-" || token == "*" || token == "/") {
            int operand1 = operands.top();
            operands.pop();
            int operand2 = operands.top();
            operands.pop();
            int result;
            switch (token[0]) {
                case '+':
                    result = operand1 + operand2;
                    break;
                case '-':
                    result = operand1 - operand2;
                    break;
                case '*':
                    result = operand1 * operand2;
                    break;
                case '/':
                    result = operand1 / operand2;
                    break;
            }
            operands.push(result);
        }
    }
    if (operands.size() != 1) {
        std::cout << "parse error" << std::endl;
        exit(0);
    }

    expressions[variable] = operands.top();
}

void Calculator::printResults() {
    std::map<std::string, int> orderedMap(expressions.begin(), expressions.end());
    for (auto expression : orderedMap) {
        std::cout << expression.first << " = " << expression.second << std::endl;
    }
}