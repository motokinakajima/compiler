//
// Created by 中嶋幹 on 10/17/24.
//

#include "main.h"

#include <iostream>
#include "NodeParser.h"
#include "CodeGenerator.h"

int main(int argc, char *argv[]) {
    char input[] = "-5 * (3 + 2) + 40";

    Token *token = TokenParser::tokenize(input);

    NodeParser nodeParser(*token);

    nodeParser.gen(nodeParser.node);

    nodeParser.codegen.MOV("x0", "x1");
    nodeParser.codegen.RET();

    std::cout<<nodeParser.codegen.get_code() <<std::endl;

    return 0;
}
