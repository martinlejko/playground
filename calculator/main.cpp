#include "parser.h"
#include "calculator.h"
#include <iostream>

int main(int argc, char** argv) {
    std::vector<std::string> args(argv + 1, argv + argc);

    if (args.size() != 1) {
        std::cout << "parse error" << std::endl;
        exit(0);
    }

    std::string fileName = args[0];
    Parser parser;
    Calculator calculator;
    if (parser.isFileValid(fileName)){
        std::vector<std::string> lines = parser.parse(fileName);
        for (auto line : lines) {
            calculator.calculate(line, "prefix");
        }
        calculator.printResults();
    }

    return 0;
}
