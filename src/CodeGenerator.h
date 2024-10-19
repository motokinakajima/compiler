//
// Created by 中嶋幹 on 10/17/24.
//

#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include <iostream>

class CodeGenerator {
public:
    CodeGenerator() {
        code += ".global _main\n";
        code += "_main:\n";
    };

    void ADD(const char *return_resistor, const char *first_input, const char *second_input) {
        code += "        ";
        code += "add " + std::string(return_resistor) + ", " + std::string(first_input) + ", " + std::string(second_input) + "\n";
    }

    void SUB(const char *return_resistor, const char *first_input, const char *second_input) {
        code += "        ";
        code += "sub " + std::string(return_resistor) + ", " + std::string(first_input) + ", " + std::string(second_input) + "\n";
    }

    void MUL(const char *return_resistor, const char *first_input, const char *second_input) {
        code += "        ";
        code += "mul " + std::string(return_resistor) + ", " + std::string(first_input) + ", " + std::string(second_input) + "\n";
    }

    void SDIV(const char *return_resistor, const char *first_input, const char *second_input) {
        code += "        ";
        code += "sdiv " + std::string(return_resistor) + ", " + std::string(first_input) + ", " + std::string(second_input) + "\n";
    }

    void STR(const char *return_resistor, const char *first_input, const char *second_input) {
        code += "        ";
        code += "str " + std::string(return_resistor) + ", [" + std::string(first_input) + ", " + std::string(second_input) + "]\n";
    }

    void LDR(const char *return_resistor, const char *first_input, const char *second_input) {
        code += "        ";
        code += "ldr " + std::string(return_resistor) + ", [" + std::string(first_input) + ", " + std::string(second_input) + "]\n";
    }

    void MOV(const char *return_resistor, const char *value) {
        code += "        ";
        code += "mov " + std::string(return_resistor) + ", " + std::string(value) + "\n";
    }

    void RET() {
        code += "        ret\n";
    }

    std::string get_code() { return code; }

private:
    std::string code;
};



#endif //CODEGENERATOR_H
