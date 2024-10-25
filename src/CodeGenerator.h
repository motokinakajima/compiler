//
// Created by 中嶋幹 on 10/17/24.
//

#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include <iostream>

class CodeGenerator {
public:
    CodeGenerator() = default;

    CodeGenerator(const std::string& label_name, const bool is_global) {
        if(is_global) {
            code += ".global _" + label_name + "\n";
            code += "_" + label_name + ":\n";
        }else {
            code += label_name + ":\n";
        }
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

    void B_EQ(const char* label) {
        code += "        b.eq " + std::string(label) + "\n";
    }

    void B_NE(const char* label) {
        code += "        b.ne " + std::string(label) + "\n";
    }

    void B_LT(const char* label) {
        code += "        b.lt " + std::string(label) + "\n";
    }

    void B_LE(const char* label) {
        code += "        b.le " + std::string(label) + "\n";
    }

    void B_GT(const char* label) {
        code += "        b.gt " + std::string(label) + "\n";
    }

    void B_GE(const char* label) {
        code += "        b.ge " + std::string(label) + "\n";
    }

    void B(const char * label) {
        code += "        b " + std::string(label) + "\n";
    }

    void LABEL(const char* label) {
        code += "        " + std::string(label) + ":\n";
    }

    void RET() {
        code += "        ret\n";
    }

    // Method to emulate PUSH
    void PUSH(const char *value) {
        COMMENT("PUSH START");
        code += "        sub sp, sp, #16\n"; // Decrement stack pointer
        code += "        str " + std::string(value) + ", [sp]\n"; // Store value at stack pointer
        COMMENT("PUSH END");
    }

    // Method to emulate POP
    void POP(const char *return_resistor) {
        COMMENT("POP START");
        code += "        ldr " + std::string(return_resistor) + ", [sp]\n"; // Load value from stack pointer
        code += "        add sp, sp, #16\n"; // Increment stack pointer
        COMMENT("POP END");
    }

    void COMMENT(const char *comment) {
        code += "\n        //" + std::string(comment) + "\n\n";
    }

    std::string get_code() { return code; }

private:
    std::string code;
};

#endif //CODEGENERATOR_H