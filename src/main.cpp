//
// Created by 中嶋幹 on 10/17/24.
//

#include "main.h"

#include <iostream>
#include <fstream>
#include <iterator>
#include "NodeParser.h"
#include "CodeGenerator.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file_path>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }


    std::string content((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());

    char *input = new char[content.size() + 1];
    std::copy(content.begin(), content.end(), input);
    input[content.size()] = '\0';

    Token *token = TokenParser::tokenize(input);

    NodeParser nodeParser(*token);

    //nodeParser.codegen.MOV("x29", "xzr");
    nodeParser.main_func.PUSH("x29");
    nodeParser.main_func.MOV("x29", "sp");
    nodeParser.main_func.SUB("sp", "sp", "256");

    for(int i = 0; nodeParser.code[i]; i++) {
        nodeParser.gen(nodeParser.code[i], nodeParser.main_func);
        nodeParser.main_func.POP("x0");
    }

    std::cout<< nodeParser.get_integrated_code() <<std::endl;

    return 0;
}
