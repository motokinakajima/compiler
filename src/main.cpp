//
// Created by 中嶋幹 on 10/17/24.
//

#include "main.h"

#include <iostream>
#include "NodeParser.h"
#include "CodeGenerator.h"

int main() {
    char input[] = "2 + 1 + 1";

    CodeGenerator codeGenerator;

    TokenParser parser;
    Token *token = TokenParser::tokenize(input);

    NodeParser nodeParser(*token);

    codeGenerator.MOV("x0", std::to_string(TokenParser::expect_number(&token)).c_str());

    while (!TokenParser::at_eof(token)) {
        if (TokenParser::consume(&token, '+')) {
            const long int current_number = TokenParser::expect_number(&token);
            codeGenerator.ADD("x0", "x0", std::to_string(current_number).c_str());
            continue;
        }

        if (TokenParser::consume(&token, '-')) {
            const long int current_number = TokenParser::expect_number(&token);
            codeGenerator.SUB("x0", "x0", std::to_string(current_number).c_str());
            continue;
        }

        throw std::runtime_error("unexpected token");
    }

    codeGenerator.RET();

    std::cout<<codeGenerator.get_code();

    return 0;
}
