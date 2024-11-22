//
// Created by 中嶋幹 on 10/17/24.
//

#ifndef NODEPARSER_H
#define NODEPARSER_H

#include <vector>
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
    ND_FOR_1,
    ND_FOR_2,
    ND_FOR_3,
    ND_FUNC,
    ND_BLOCK,
    ND_NONE,
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

    Token();

    static Token *new_token(TokenKind kind, Token *cur, char *str, unsigned long int len);
};

extern std::string reserved[];

class TokenParser {
public:
    explicit TokenParser();

    static Token *tokenize(char *p);

    static bool consume(Token **current, const char *op);

    static bool consume(Token **current, TokenKind kind);

    static bool can_consume(Token **current, const char *op);

    static Token *consume_ident(Token **current);

    static void expect(Token **current, const char *op);

    static long int expect_number(Token **current);

    static bool at_eof(const Token *current);

    static bool starts_with(const char *p, const char *q);

    static bool is_alnum(char c);
};

class Node {
public:
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    long int val;
    long int offset;
    char *str;
    unsigned long int len;
    std::vector<Node *> stmts;
    std::vector<Node *> args;

    Node();

    static Node *new_node(NodeKind kind);

    static Node *new_binary(NodeKind kind, Node *lhs, Node *rhs);

    static Node *new_num(long int val);
};

class LVar {
public:
    mutable const LVar *next;
    mutable char *name;
    mutable unsigned long int len;
    mutable long int offset;

    LVar();

    static LVar *new_var();

    static const LVar *find_lvar(const Token *tok, const LVar *head);
};

class NodeParser {
public:
    //CodeGenerator main_func = CodeGenerator("main", true);
    //std::vector<CodeGenerator> labels;
    std::vector<std::string> label_names;
    std::vector<Node *> code;

    explicit NodeParser(Token &token);

    void program();

    Node *func();

    Node *block();

    Node *stmt();

    Node *expr();

    Node *assign();

    Node *equality();

    Node *relational();

    Node *add();

    Node *mul();

    Node *unary();

    Node *primary();

    //static void gen_lval(const Node *node, CodeGenerator &codegen);

    //void gen(const Node *node, CodeGenerator &codegen);

    std::string get_integrated_code();

    long int get_total_offset() const;

private:
    Token *token = {};
    const LVar *locals = LVar::new_var();
    long int largest_offset = 0;
};

#endif //NODEPARSER_H