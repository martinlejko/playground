#include "parser.h"
#include <iostream>

int main(int argc, char** argv) {
    std::vector<std::string> args(argv + 1, argv + argc);
    std::string fileName = args[0];

    std::cout << fileName << std::endl;
    Parser parser;
    if (parser.isFileValid(fileName)){
        std::vector<std::string> lines = parser.parse(fileName);
        for (auto line : lines) {
            std::cout << line << std::endl;
        }
    }

    return 0;
}
