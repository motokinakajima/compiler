//
// Created by 中嶋幹 on 10/17/24.
//

#include "main.h"

#include <iostream>
#include "NodeParser.h"
#include "CodeGenerator.h"

int main() {
    char input[] = "a = 1 + 2;";

    Token *token = TokenParser::tokenize(input);

    NodeParser nodeParser(*token);

    nodeParser.codegen.MOV("x29", "xzr");
    nodeParser.codegen.SUB("x30", "x30", "208");

    for(int i = 0; nodeParser.code[i]; i++) {
        nodeParser.gen(nodeParser.code[i]);
        nodeParser.codegen.MOV("x0", "x0");
    }
    nodeParser.codegen.MOV("x30", "x29");
    nodeParser.codegen.RET();

    std::cout<<nodeParser.codegen.get_code() <<std::endl;

    return 0;
}
