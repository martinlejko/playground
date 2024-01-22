cmake_minimum_required(VERSION 3.26)
project(calculator)

set(CMAKE_CXX_STANDARD 20)

add_executable(calculator main.cpp
        parser.h
        parser.cpp
        calculator.h
        calculator.cpp
        treenode.h
)
