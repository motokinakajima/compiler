//
// Created by 中嶋幹 on 10/17/24.
//

#include "main.h"

#include <iostream>
#include "NodeParser.h"
#include "CodeGenerator.h"

int main() {
    char input[] = "abs = 5 - 2; b = 8;abs == b;b = abs; return b; a = 1;";

    Token *token = TokenParser::tokenize(input);

    NodeParser nodeParser(*token);

    //nodeParser.codegen.MOV("x29", "xzr");
    nodeParser.codegen.PUSH("x29");
    nodeParser.codegen.MOV("x29", "sp");
    nodeParser.codegen.SUB("sp", "sp", "208");

    for(int i = 0; nodeParser.code[i]; i++) {
        nodeParser.gen(nodeParser.code[i]);
        nodeParser.codegen.POP("x0");
    }

    std::cout<<nodeParser.codegen.get_code() <<std::endl;

    return 0;
}
