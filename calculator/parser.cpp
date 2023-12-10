//
// Created by Martin Lejko on 10/12/2023.
//
#include "parser.h"
#include <iostream>
#include <fstream>
#include <sstream>

bool Parser::isFileValid(std::string fileName) {
    std::ifstream file(fileName);

    if (!file.is_open()) {
        std::cout << "Error: File could not be opened." << std::endl;
        return false;
    }

    if (file.peek() == std::ifstream::traits_type::eof()) {
        std::cout << "Error: File is empty." << std::endl;
        return false;
    }
    return true;
}

std::vector<std::string> Parser::parse(std::string fileName) {
    std::vector<std::string> lines;
    std::ifstream file(fileName);
    std::string line;
    while (std::getline(file, line)) {

        if (line.find("=") == std::string::npos) {
            std::cout << "Error: No equals sign." << std::endl;
            exit(0);
        }

        std::istringstream iss(line);
        std::string variable;
        iss >> variable;
        //Also substr can be used like this: variable.substr(0, variable.find("="))
        if (!isCorrectVariable(variable)) {
            std::cout << "Error: Invalid variable name." << std::endl;
            exit(0);
        }
        lines.push_back(line);
    }
    return lines;
}

bool Parser::isCorrectVariable(std::string &variable) {
    if (!isalpha(variable[0])){
        return false;
    }
    for (int i = 1; i < variable.length(); i++) {
        if (!isalnum(variable[i])) {
            return false;
        }
    }
    return true;
}