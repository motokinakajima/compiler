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
    TK_RETURN,
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
    ND_RETURN,
    ND_IF,
    ND_ELSE,
    ND_WHILE,
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
    unsigned long int len;

    Token() = default;

    static Token *new_token(const TokenKind kind, Token *cur, char *str, const unsigned long int len) {
        auto *tok = static_cast<Token *>(calloc(1, sizeof(Token)));
        tok->kind = kind;
        tok->str = str;
        tok->len = len;
        cur->next = tok;
        return tok;
    }
};

std::string reserved[] = {"==", "!=", "<=", ">=", "if", "else", "while", "+", "-", "*", "/", "(", ")", "<", ">", "=", ";"};

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

            bool matched = false;
            for(const auto & i : reserved) {
                if(starts_with(p, i.c_str())) {
                    cur = Token::new_token(TK_RESERVED, cur, p, strlen(i.c_str()));
                    p += strlen(i.c_str());
                    matched = true;
                    break;
                }
            }
            if(matched) { continue; }

            if (isdigit(*p)) {
                cur = Token::new_token(TK_NUM, cur, p, 0);
                const char *q = p;
                cur->val = strtol(p, &p, 10);
                cur->len = p - q;
                continue;
            }

            if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
                cur = Token::new_token(TK_RETURN, cur, p, 6);
                p += 6;
                continue;
            }

            if (isalpha(*p)) {
                char *start = p;  // Start of the identifier
                while (isalpha(*p)) {   // Loop until you hit a non-alphabet character
                    p++;
                }
                cur = Token::new_token(TK_IDENT, cur, start, p - start);  // Create a new token with the full identifier
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

    static bool consume(Token **current, const TokenKind kind) {
        if((*current)->kind != kind) {
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

    static bool is_alnum(const char c) {
        return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') || (c == '_');
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

class LVar {
public:
    mutable const LVar *next;
    mutable char *name;
    mutable unsigned long int len;
    mutable long int offset;

    LVar() = default;

    static LVar *new_var() {
        auto *lvar = static_cast<LVar *>(calloc(1, sizeof(LVar)));
        lvar->offset = 0;
        return lvar;
    }

    static const LVar *find_lvar(const Token *tok, const LVar *head) {
        for(const LVar *var = head; var->next && var->name && var->len != 0 && var->offset != 0; var = var->next) {
            if(var->len == tok->len && !memcmp(tok->str, var->name, var->len)) {
                return var;
            }
        }
        return nullptr;
    }
};

class NodeParser {
public:
    CodeGenerator main_func = CodeGenerator("main", true);
    CodeGenerator labels[100];
    std::string label_names[100];
    Node *code[100]{};

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
        Node *node;
        if(TokenParser::consume(&token, "if")) {
            TokenParser::expect(&token, "(");
            node = Node::new_node(ND_IF);
            node->lhs = expr();
            TokenParser::expect(&token, ")");
            Node *if_stmt = stmt();
            if(TokenParser::consume(&token, "else")) {
                node->rhs = Node::new_node(ND_ELSE);
                node->rhs->lhs = if_stmt;
                node->rhs->rhs = stmt();
            }else {
                node->rhs = if_stmt;
            }
            return node;
        }
        if(TokenParser::consume(&token, "while")) {
            TokenParser::expect(&token, "(");
            node = Node::new_node(ND_WHILE);
            node->lhs = expr();
            TokenParser::expect(&token, ")");
            node->rhs = stmt();
            return node;
        }
        if(TokenParser::consume(&token, TokenKind::TK_RETURN)) {
            node = Node::new_node(ND_RETURN);
            node->lhs = expr();
        }else {
            node = expr();
        }
        if(!TokenParser::consume(&token, ";")) {
            throw std::runtime_error("unexpected token");
        }
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
                node = Node::new_binary(ND_LT, node, add());
            }else if(TokenParser::consume(&token, "<=")) {
                node = Node::new_binary(ND_LE, node, add());
            }else if(TokenParser::consume(&token, ">")) {
                node = Node::new_binary(ND_LT, add(), node);
            }else if(TokenParser::consume(&token, ">=")) {
                node = Node::new_binary(ND_LE, add(), node);
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
            Node *node = Node::new_node(ND_LVAR);

            const LVar *lvar = LVar::find_lvar(tok, this->locals);
            if(lvar) {
                node->offset = lvar->offset;
            }else {
                lvar = static_cast<LVar *>(calloc(1, sizeof(LVar)));
                lvar->next = this->locals;
                lvar->name = tok->str;
                lvar->len = tok->len;
                lvar->offset = this->locals->offset + 16;
                node->offset = lvar->offset;
                this->locals = lvar;
            }
            return node;
        }

        return Node::new_num(TokenParser::expect_number(&token));
    }

    static void gen_lval(const Node *node, CodeGenerator &codegen) {
        if (node->kind != ND_LVAR) {
            throw std::runtime_error("lval is not a variable");
        }

        codegen.COMMENT("generate lval address start");
        codegen.MOV("x0", "x29");
        codegen.SUB("x0", "x0", std::to_string(node->offset).c_str());
        codegen.PUSH("x0");
        codegen.COMMENT("generate lval address end");
    }

    void gen(const Node *node, CodeGenerator &codegen) {
        switch(node->kind) {
            case ND_IF: {
                codegen.COMMENT("if clause start");
                gen(node->lhs, this->main_func);
                codegen.POP("x0");
                codegen.CMP("x0", "0");
                if(node->rhs->kind == ND_ELSE) {
                    int i = 0;
                    for(;!label_names[i].empty();i++){}
                    label_names[i] = ".Lelse_" + std::to_string(i);
                    int j = 0;
                    for(;!label_names[j].empty();j++){}
                    label_names[j] = ".Lend_" + std::to_string(j);
                    codegen.B_EQ(label_names[i].c_str());
                    gen(node->rhs->lhs, this->main_func);
                    codegen.B(label_names[j].c_str());
                    codegen.LABEL(label_names[i].c_str());
                    gen(node->rhs->rhs, this->main_func);
                    codegen.LABEL(label_names[j].c_str());
                }else {
                    int i = 0;
                    for(;!label_names[i].empty();i++){}
                    label_names[i] = ".Lend_" + std::to_string(i);
                    codegen.B_EQ(label_names[i].c_str());
                    gen(node->rhs, this->main_func);
                    codegen.LABEL(label_names[i].c_str());
                }
                codegen.COMMENT("if clause end");
                return;
            }

            case ND_WHILE: {
                int i = 0;
                for(;!label_names[i].empty();i++){}
                label_names[i] = ".Lbegin_" + std::to_string(i);
                int j = 0;
                for(;!label_names[j].empty();j++){}
                label_names[j] = ".Lend_" + std::to_string(j);
                codegen.COMMENT("while clause start");
                codegen.LABEL(label_names[i].c_str());
                gen(node->rhs->lhs, this->main_func);
                codegen.POP("x0");
                codegen.CMP("x0", "0");
                codegen.B_EQ(label_names[j].c_str());
                gen(node->rhs->rhs, this->main_func);
                codegen.B(label_names[i].c_str());
                codegen.LABEL(label_names[j].c_str());
                codegen.COMMENT("while clause end");
                return;
            }

            case ND_RETURN:
                codegen.COMMENT("return start");
                gen(node->lhs, this->main_func);
                codegen.POP("x0");
                codegen.MOV("sp", "x29");
                codegen.POP("x29");
                codegen.RET();
                codegen.COMMENT("return end");
                return;

            case ND_NUM:
                codegen.MOV("x0", std::to_string(node->val).c_str());
                codegen.PUSH("x0");
                return;

            case ND_LVAR:
                codegen.COMMENT("load value start");
                gen_lval(node, main_func); // Generate the variable address
                codegen.POP("x0");
                codegen.LDR("x0", "x0"); // Load the value from the variable's address
                codegen.PUSH("x0");
                codegen.COMMENT("load value end");
                return;

            case ND_ASSIGN:
                gen_lval(node->lhs, this->main_func); // Left-hand side is the variable
                gen(node->rhs, this->main_func); // Right-hand side is the value to assign
                codegen.COMMENT("assign start");
                codegen.POP("x1"); // The value to assign
                codegen.POP("x0"); // The address of the variable
                codegen.STR("x1", "x0"); // Store the value in the address
                codegen.PUSH("x1"); // Push the result (assigned value) back to the stack
                codegen.COMMENT("assign end");
                return;

            default:
                break;
        }

        // Generate code for the left-hand side
        gen(node->lhs, this->main_func);
        // Generate code for the right-hand side
        gen(node->rhs, this->main_func);

        // Pop the right and left-hand side values from the stack
        codegen.POP("x1");
        codegen.POP("x0");

        switch(node->kind) {
            case ND_ADD:
                codegen.ADD("x0", "x0", "x1");
            break;
            case ND_SUB:
                codegen.SUB("x0", "x0", "x1");
            break;
            case ND_MUL:
                codegen.MUL("x0", "x0", "x1");
            break;
            case ND_DIV:
                codegen.SDIV("x0", "x0", "x1");
                break;
            case ND_EQ:
                codegen.CMP("x0", "x1");
                codegen.CSET("x0", "eq");
                break;
            case ND_NE:
                codegen.CMP("x0", "x1");
                codegen.CSET("x0", "ne");
                break;
            case ND_LT:
                codegen.CMP("x0", "x1");
                codegen.CSET("x0", "lt");
                break;
            case ND_LE:
                codegen.CMP("x0", "x1");
                codegen.CSET("x0", "le");
                break;
            default:
                break;
        }

        // Push the result of the operation back onto the stack
        codegen.PUSH("x0");
    }

    std::string get_integrated_code() {
        std::string code = this->main_func.get_code();
        for(auto & label : labels) {
            code += label.get_code();
        }
        return code;
    }

private:
    Token *token = {};
    const LVar *locals = LVar::new_var();
};

#endif //NODEPARSER_H