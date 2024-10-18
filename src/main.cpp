//
// Created by 中嶋幹 on 10/17/24.
//

#include "main.h"

#include <iostream>
#include "NodeParser.h"

int main() {
    char input[] = "1 + 1 - 10 - 20 - 30 - 40 - 50 - 60 - 70 - 80 - 90";

    NodeParser parser(input);
    Token *token = NodeParser::tokenize(input);

    long int result = NodeParser::expect_number(&token);

    while (!NodeParser::at_eof(token)) {
        if (NodeParser::consume(&token, '+')) {
            result += NodeParser::expect_number(&token);
            continue;
        }

        if (NodeParser::consume(&token, '-')) {
            result -= NodeParser::expect_number(&token);
            continue;
        }

        throw std::runtime_error("unexpected token");
    }

    std::cout << "Result: " << result << std::endl;

    return 0;
}
