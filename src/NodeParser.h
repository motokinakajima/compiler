//
// Created by 中嶋幹 on 10/17/24.
//

#ifndef NODEPARSER_H
#define NODEPARSER_H

#include <iostream>
#include <cstdlib>
#include <cctype>
#include <string>

enum TokenKind {
    TK_RESERVED,  // Operators like + or -
    TK_NUM,       // Numeric tokens
    TK_EOF,       // End of file
};

class Token {
public:
    TokenKind kind;
    Token *next;
    long int val;  // If kind is TK_NUM, store the number
    char *str;     // Token string (operator or number)

    Token() = default;

    // Helper to create a new token
    static Token *new_token(const TokenKind kind, Token *cur, char *str) {
        auto *tok = static_cast<Token *>(calloc(1, sizeof(Token)));
        tok->kind = kind;
        tok->str = str;
        cur->next = tok;
        return tok;
    }
};

class NodeParser {
public:
    explicit NodeParser(std::string input_string) {
        this->input = std::move(input_string);
    }

    // Tokenize the input string and return the head of the token list
    static Token *tokenize(char *p) {
        Token head{};
        head.next = nullptr;
        Token *cur = &head;

        while (*p != '\0') {
            if (isspace(*p)) {
                p++;
                continue;
            }

            if (*p == '+' || *p == '-') {
                cur = Token::new_token(TK_RESERVED, cur, p++);
                continue;
            }

            if (isdigit(*p)) {
                cur = Token::new_token(TK_NUM, cur, p);
                cur->val = strtol(p, &p, 10);
                continue;
            }

            throw std::runtime_error("cannot tokenize");
        }

        Token::new_token(TK_EOF, cur, p);
        return head.next;  // Return the head of the token list
    }

    // Consume a token of the expected kind
    static bool consume(Token **current, const char op) {
        if ((*current)->kind != TK_RESERVED || (*current)->str[0] != op) {
            return false;
        }
        *current = (*current)->next;
        return true;
    }

    // Expect and consume a specific token, throwing an error if it's not found
    static void expect(Token **current, const char op) {
        std::cout << "expect: " << op << " actual: " << (*current)->str << "\n";
        if ((*current)->kind != TK_RESERVED || (*current)->str[0] != op) {
            throw std::runtime_error("unexpected token");
        }
        *current = (*current)->next;
    }

    // Expect and return a number token, or throw an error if the token is not a number
    static long int expect_number(Token **current) {
        if ((*current)->kind != TK_NUM) {
            throw std::runtime_error("expected number");
        }
        const long int val = (*current)->val;
        *current = (*current)->next;
        return val;
    }

    // Check if the token is EOF
    static bool at_eof(const Token *current) {
        return current->kind == TK_EOF;
    }

private:
    std::string input;
};

#endif //NODEPARSER_H
