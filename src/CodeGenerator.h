//
// Created by 中嶋幹 on 10/17/24.
//

#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include <iostream>

class CodeGenerator {
public:
    CodeGenerator();

    CodeGenerator(const std::string& label_name, bool is_global);

    void CODE(const char *content);

    void ADD(const char *return_resistor, const char *first_input, const char *second_input);

    void SUB(const char *return_resistor, const char *first_input, const char *second_input);

    void MUL(const char *return_resistor, const char *first_input, const char *second_input);

    void SDIV(const char *return_resistor, const char *first_input, const char *second_input);

    void CMP(const char *first_input, const char *second_input);

    void CSET(const char *return_resistor, const char *condition);

    void STR(const char *return_resistor, const char *first_input, const char *second_input);

    void STR(const char *return_resistor, const char *address);

    void LDR(const char *return_resistor, const char *first_input, const char *second_input);

    void LDR(const char *return_resistor, const char *address);

    void STP(const char *return_resistor, const char *first_input, const char *second_input);

    void STP_exclamation(const char *return_resistor, const char *first_input, const char *address);

    void LDP(const char *return_resistor, const char *first_input, const char *second_input, const char *third_input);

    void MOV(const char *return_resistor, const char *value);

    void B_EQ(const char* label);

    void B_NE(const char* label);

    void B_LT(const char* label);

    void B_LE(const char* label);

    void B_GT(const char* label);

    void B_GE(const char* label);

    void B(const char * label);

    void BL(const char * func);

    void LABEL(const char* label);

    void RET();

    void PUSH(const char *value);

    void POP(const char *return_resistor);

    void COMMENT(const char *comment);

    std::string get_code();

private:
    std::string code;
};

#endif //CODEGENERATOR_H