//
// Created by Martin Lejko on 10/01/2024.
//

#ifndef CALCULATOR_TREENODE_H
#define CALCULATOR_TREENODE_H
#include <string>
#include <memory>

struct TreeNode {
    std::string data;
    std::unique_ptr<TreeNode> left;
    std::unique_ptr<TreeNode> right;

    TreeNode(std::string data) : data(data), left(nullptr), right(nullptr) {}
};

#endif //CALCULATOR_TREENODE_H
