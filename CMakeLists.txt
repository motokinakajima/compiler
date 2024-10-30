cmake_minimum_required(VERSION 3.29)
project(compiler)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(compiler
    src/main.cpp
    src/main.h
        src/NodeParser.h
        src/CodeGenerator.cpp
        src/CodeGenerator.h
        src/foo.cpp
)
