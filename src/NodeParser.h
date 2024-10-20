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
    TK_RESERVED,
    TK_IDENT,
    TK_NUM,
    TK_EOF,
};

enum NodeKind {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_ASSIGN,
    ND_LVAR,
    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE,
    ND_NUM,
};

class Token {
public:
    TokenKind kind;
    Token *next;
    long int val;
    char *str;
    long int len;

    Token() = default;

    static Token *new_token(const TokenKind kind, Token *cur, char *str, int len) {
        auto *tok = static_cast<Token *>(calloc(1, sizeof(Token)));
        tok->kind = kind;
        tok->str = str;
        tok->len = len;
        cur->next = tok;
        return tok;
    }
};

class TokenParser {
public:
    explicit TokenParser() = default;

    static Token *tokenize(char *p) {
        Token head{};
        head.next = nullptr;
        Token *cur = &head;

        while (*p != '\0') {
            if (isspace(*p)) {
                p++;
                continue;
            }

            if(starts_with(p, "==") || starts_with(p, "!=") || starts_with(p, "<=") || starts_with(p, ">=")) {
                cur = Token::new_token(TK_RESERVED, cur, p, 2);
                p += 2;
                continue;
            }

            if (strchr("+-*/()<>=;", *p)) {
                cur = Token::new_token(TK_RESERVED, cur, p++, 1);
                continue;
            }

            if (isdigit(*p)) {
                cur = Token::new_token(TK_NUM, cur, p, 0);
                const char *q = p;
                cur->val = strtol(p, &p, 10);
                cur->len = p - q;
                continue;
            }

            if ('a' <= *p && *p <= 'z') {
                cur = Token::new_token(TK_IDENT, cur, p++, 1);
                continue;
            }

            throw std::runtime_error("cannot tokenize");
        }

        Token::new_token(TK_EOF, cur, p, 0);
        return head.next;
    }

    static bool consume(Token **current, const char *op) {
        if((*current)->kind != TK_RESERVED) {
            return false;
        }
        if(strlen(op) != (*current)->len) {
            return false;
        }
        const int is_equal = memcmp((*current)->str, op, (*current)->len);
        if(is_equal != 0) {
            return false;
        }

        *current = (*current)->next;
        return true;
    }

    static Token *consume_ident(Token **current) {
        if((*current)->kind != TK_IDENT) {
            return nullptr;
        }
        Token *prev = *current;
        *current = (*current)->next;
        return prev;
    }

    static void expect(Token **current, const char *op) {
        if ((*current)->kind != TK_RESERVED || strlen(op) != (*current)->len || memcmp((*current)->str, &op, (*current)->len) == 0) {
            throw std::runtime_error("unexpected token");
        }
        *current = (*current)->next;
    }

    static long int expect_number(Token **current) {
        if ((*current)->kind != TK_NUM) {
            throw std::runtime_error("expected number");
        }
        const long int val = (*current)->val;
        *current = (*current)->next;
        return val;
    }

    static bool at_eof(const Token *current) {
        return current->kind == TK_EOF;
    }

    static bool starts_with(const char *p, const char *q) {
        return memcmp(p, q, strlen(q)) == 0;
    }
};

class Node {
public:
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    long int val;
    long int offset;

    Node() = default;

    static Node *new_node(const NodeKind kind) {
        auto *node = static_cast<Node *>(calloc(1, sizeof(Node)));
        node->kind = kind;
        return node;
    }

    static Node *new_binary(const NodeKind kind, Node *lhs, Node *rhs) {
        Node *node = new_node(kind);
        node->lhs = lhs;
        node->rhs = rhs;
        return node;
    }

    static Node *new_num(const long int val) {
        auto *node = new_node(ND_NUM);
        node->val = val;
        return node;
    }

    static Node *new_ident(const char *str) {
        auto *node = new_node(ND_LVAR);
        node->offset = (str[0] - 'a' + 1) * 8;
        return node;
    }
};

class NodeParser {
public:
    CodeGenerator codegen;
    Node *code[100];

    explicit NodeParser(Token &token) : token(&token) {
        program();
    }

    void program() {
        int i = 0;
        while(!TokenParser::at_eof(token)) {
            code[i++] = stmt();
        }
        code[i] = nullptr;
    }

    Node *stmt() {
        Node *node = expr();
        TokenParser::expect(&token, ";");
        return node;
    }

    Node *expr() {
        return assign();
    }

    Node *assign() {
        Node *node = equality();
        if(TokenParser::consume(&token, "=")) {
            node = Node::new_binary(ND_ASSIGN, node, assign());
        }
        return node;
    }

    Node *equality() {
        Node *node = relational();

        for(;;) {
            if(TokenParser::consume(&token, "==")) {
                node = Node::new_binary(ND_EQ, node, relational());
            }else if(TokenParser::consume(&token, "!=")) {
                node = Node::new_binary(ND_NE, node, relational());
            }else {
                return node;
            }
        }
    }

    Node *relational() {
        Node *node = add();

        for(;;) {
            if(TokenParser::consume(&token, "<")) {
                node = Node::new_binary(NodeKind::ND_LT, node, add());
            }else if(TokenParser::consume(&token, "<=")) {
                node = Node::new_binary(NodeKind::ND_LE, node, add());
            }else if(TokenParser::consume(&token, ">")) {
                node = Node::new_binary(NodeKind::ND_LT, add(), node);
            }else if(TokenParser::consume(&token, ">=")) {
                node = Node::new_binary(NodeKind::ND_LE, add(), node);
            }else {
                return node;
            }
        }
    }

    Node *add() {
        Node *node = mul();

        for(;;) {
            if(TokenParser::consume(&token, "+")) {
                node = Node::new_binary(ND_ADD, node, mul());
            }else if(TokenParser::consume(&token, "-")) {
                node = Node::new_binary(ND_SUB, node, mul());
            }else {
                return node;
            }
        }
    }

    Node *mul() {
        Node *node = unary();

        for(;;) {
            if(TokenParser::consume(&token, "*")) {
                node = Node::new_binary(ND_MUL, node, unary());
            }else if(TokenParser::consume(&token, "/")) {
                node = Node::new_binary(ND_DIV, node, unary());
            }else {
                return node;
            }
        }
    }

    Node *unary() {
        if(TokenParser::consume(&token, "+")) {
            return unary();
        }
        if(TokenParser::consume(&token, "-")) {
            return Node::new_binary(ND_SUB, Node::new_num(0), unary());
        }
        return primary();
    }

    Node *primary() {
        if(TokenParser::consume(&token, "(")) {
            Node *node = expr();
            TokenParser::expect(&token, ")");
            return node;
        }

        const Token *tok = TokenParser::consume_ident(&token);
        if(tok) {
            Node *node = Node::new_ident(tok->str);
            return node;
        }

        return Node::new_num(TokenParser::expect_number(&token));
    }

    void gen_lval(const Node *node) {
        if (node->kind != ND_LVAR) {
            throw std::runtime_error("lval is not a variable");
        }

        this->codegen.MOV("x0", "x29");
        this->codegen.SUB("x0", "x0", std::to_string(node->offset).c_str());
        this->codegen.STR("x0", "sp", "-16");
    }

    void gen(const Node *node) {
        switch(node->kind) {
            case ND_NUM:
                this->codegen.MOV("x0", std::to_string(node->val).c_str());
                this->codegen.STR("x0", "sp", "-16");
                return;
            case ND_LVAR:
                gen_lval(node);
                this->codegen.LDR("x0", "sp", "16");
                this->codegen.LDR("x0", "x0");
                this->codegen.STR("x0", "sp", "-16!");
                return;
            case ND_ASSIGN:
                gen_lval(node->lhs);
                gen(node->lhs);
                this->codegen.LDR("x1", "sp", "16");
                this->codegen.LDR("x0", "sp", "16");
                this->codegen.STR("x1", "x0");
                this->codegen.STR("x1", "sp", "-16!");
                return;
        }

        gen(node->lhs);
        gen(node->rhs);

        this->codegen.LDR("x1", "sp", "16");
        this->codegen.LDR("x0", "sp", "16");

        switch(node->kind) {
            case ND_ADD:
                this->codegen.ADD("x0", "x0", "x1");
                break;
            case ND_SUB:
                this->codegen.SUB("x0", "x0", "x1");
                break;
            case ND_MUL:
                this->codegen.MUL("x0", "x0", "x1");
                break;
            case ND_DIV:
                this->codegen.SDIV("x0", "x0", "x1");
                break;
            case ND_EQ:
                this->codegen.CMP("x2", "x1");
                this->codegen.CSET("x1", "eq");
                break;
            case ND_NE:
                this->codegen.CMP("x2", "x1");
                this->codegen.CSET("x1", "ne");
                break;
            case ND_LT:
                this->codegen.CMP("x2", "x1");
                this->codegen.CSET("x1", "lt");
                break;
            case ND_LE:
                this->codegen.CMP("x2", "x1");
                this->codegen.CSET("x1", "le");
                break;
            default:
                break;
        }

        this->codegen.STR("x0", "sp", "-16!");
    }

private:
    Token *token = {};
};

#endif //NODEPARSER_H
