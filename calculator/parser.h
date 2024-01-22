//
// Created by Martin Lejko on 10/12/2023.
//

#ifndef CALCULATOR_PARSER_H
#define CALCULATOR_PARSER_H
#include <vector>
#include <string>

class Parser {
public:
    static bool isFileValid(std::string &fileName);
    static std::vector<std::string> parse(std::string &fileName);
private:
    static bool isCorrectVariable(std::string &variable);
};
#endif //CALCULATOR_PARSER_H
