//
// Created by 中嶋幹 on 10/17/24.
//

#ifndef NODEPARSER_H
#define NODEPARSER_H

#include <iostream>
#include <cstdlib>
#include <cctype>
#include "CodeGenerator.h"

enum TokenKind {
    TK_RESERVED,  // Operators like + or -
    TK_NUM,       // Numeric tokens
    TK_EOF,       // End of file
};

enum NodeKind {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM,
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

class TokenParser {
public:
    explicit TokenParser() = default;

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

            if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')') {
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
};

class Node {
public:
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    long int val;

    Node() = default;

    static Node *new_node(const NodeKind kind, Node *lhs, Node *rhs) {
        auto *node = static_cast<Node *>(calloc(1, sizeof(Node)));
        node->kind = kind;
        node->lhs = lhs;
        node->rhs = rhs;
        return node;
    }

    static Node *new_node_num(const long int val) {
        auto *node = static_cast<Node *>(calloc(1, sizeof(Node)));
        node->kind = ND_NUM;
        node->val = val;
        return node;
    }
};

class NodeParser {
public:
    Node *node;
    CodeGenerator codegen;

    explicit NodeParser(Token &token) : token(&token) {
        node = expr();
    }

    Node *expr() {
        Node *node = mul();

        for(;;) {
            if(TokenParser::consume(&token, '+')) {
                node = Node::new_node(ND_ADD, node, mul());
            }else if(TokenParser::consume(&token, '-')) {
                node = Node::new_node(ND_SUB, node, mul());
            }else {
                return node;
            }
        }
    }

    Node *mul() {
        Node *node = unary();

        for(;;) {
            if(TokenParser::consume(&token, '*')) {
                node = Node::new_node(ND_MUL, node, mul());
            }else if(TokenParser::consume(&token, '/')) {
                node = Node::new_node(ND_DIV, node, mul());
            }else {
                return node;
            }
        }
    }

    Node *unary() {
        if(TokenParser::consume(&token, '+')) {
            return primary();
        }
        if(TokenParser::consume(&token, '-')) {
            return Node::new_node(ND_SUB, Node::new_node_num(0), primary());
        }
        return primary();
    }

    Node *primary() {
        if(TokenParser::consume(&token, '(')) {
            Node *node = expr();
            TokenParser::expect(&token, ')');
            return node;
        }

        return Node::new_node_num(TokenParser::expect_number(&token));
    }

    void gen(const Node *node) {
        if(node->kind == ND_NUM) {
            this->codegen.SUB("sp", "sp", "16");
            this->codegen.MOV("x3", std::to_string(node->val).c_str());
            this->codegen.STR("x3", "sp", "0");
            return;
        }

        gen(node->lhs);
        gen(node->rhs);

        this->codegen.LDR("x1", "sp", "0");
        this->codegen.ADD("sp", "sp", "16");
        this->codegen.LDR("x2", "sp", "0");
        this->codegen.ADD("sp", "sp", "16");

        switch(node->kind) {
            case ND_ADD:
                this->codegen.ADD("x1", "x2", "x1");
                break;
            case ND_SUB:
                this->codegen.SUB("x1", "x2", "x1");
                break;
            case ND_MUL:
                this->codegen.MUL("x1", "x2", "x1");
                break;
            case ND_DIV:
                this->codegen.SDIV("x1", "x2", "x1");
                break;
            default:
                break;
        }

        this->codegen.SUB("sp", "sp", "16");
        this->codegen.STR("x1", "sp", "0");
    }

private:
    Token *token = {};
};

#endif //NODEPARSER_H
