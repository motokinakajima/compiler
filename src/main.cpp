//
// Created by 中嶋幹 on 10/17/24.
//

#include "main.h"

#include <iostream>
#include "NodeParser.h"
#include "CodeGenerator.h"

int main() {
    char input[] = "a = 0;while(a < 4) a = a + 7; return a;";

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
