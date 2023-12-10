//
// Created by Martin Lejko on 10/12/2023.
//

#ifndef CALCULATOR_CALCULATOR_H
#define CALCULATOR_CALCULATOR_H
#include <string>
#include <unordered_map>
class Calculator {
public:
    std::unordered_map<std::string,int> expressions;
    void calculate(std::string expression, std::string type);

private:
    void postfix(std::string expression);
};
#endif //CALCULATOR_CALCULATOR_H
