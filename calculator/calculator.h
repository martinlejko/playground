//
// Created by Martin Lejko on 10/12/2023.
//

#ifndef CALCULATOR_CALCULATOR_H
#define CALCULATOR_CALCULATOR_H
#include <string>
#include <unordered_map>
#include "treenode.h"
#include <stack>
#include <map>

class Calculator {
private:
    std::map<std::string, int> expressions;
    std::unique_ptr<TreeNode> buildExpressionTree(std::stack<std::string>& tokens);
    int evaluateExpressionTree(const std::unique_ptr<TreeNode>& root);

public:
    void calculate(std::string &expression, const std::string& type);
    void printResults();
};
#endif //CALCULATOR_CALCULATOR_H
