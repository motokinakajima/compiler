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
        code += "        add " + std::string(return_resistor) + ", " + std::string(first_input) + ", " + std::string(second_input) + "\n";
    }

    void SUB(const char *return_resistor, const char *first_input, const char *second_input) {
        code += "        sub " + std::string(return_resistor) + ", " + std::string(first_input) + ", " + std::string(second_input) + "\n";
    }

    void MUL(const char *return_resistor, const char *first_input, const char *second_input) {
        code += "        mul " + std::string(return_resistor) + ", " + std::string(first_input) + ", " + std::string(second_input) + "\n";
    }

    void SDIV(const char *return_resistor, const char *first_input, const char *second_input) {
        code += "        sdiv " + std::string(return_resistor) + ", " + std::string(first_input) + ", " + std::string(second_input) + "\n";
    }

    void CMP(const char *first_input, const char *second_input) {
        code += "        cmp " + std::string(first_input) + ", " + std::string(second_input) + "\n";
    }

    void CSET(const char *return_resistor, const char *condition) {
        code += "        cset " + std::string(return_resistor) + ", " + std::string(condition) + "\n";
    }

    void STR(const char *return_resistor, const char *first_input, const char *second_input) {
        code += "        str " + std::string(return_resistor) + ", [" + std::string(first_input) + ", " + std::string(second_input) + "]\n";
    }

    void STR(const char *return_resistor, const char *address) {
        code += "        str " + std::string(return_resistor) + ", [" + std::string(address) + "]\n";
    }

    void LDR(const char *return_resistor, const char *first_input, const char *second_input) {
        code += "        ldr " + std::string(return_resistor) + ", [" + std::string(first_input) + ", " + std::string(second_input) + "]\n";
    }

    void LDR(const char *return_resistor, const char *address) {
        code += "        ldr " + std::string(return_resistor) + ", [" + std::string(address) + "]\n";
    }

    void MOV(const char *return_resistor, const char *value) {
        code += "        mov " + std::string(return_resistor) + ", " + std::string(value) + "\n";
    }

    void RET() {
        code += "        ret\n";
    }

    // Method to emulate PUSH
    void PUSH(const char *value) {
        code += "        sub sp, sp, #16\n"; // Decrement stack pointer
        code += "        str " + std::string(value) + ", [sp]\n"; // Store value at stack pointer
    }

    // Method to emulate POP
    void POP(const char *return_resistor) {
        code += "        ldr " + std::string(return_resistor) + ", [sp]\n"; // Load value from stack pointer
        code += "        add sp, sp, #16\n"; // Increment stack pointer
    }

    std::string get_code() { return code; }

private:
    std::string code;
};

#endif //CODEGENERATOR_H