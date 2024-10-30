//
// Created by 中嶋幹 on 10/17/24.
//

#include "main.h"

#include <iostream>
#include <fstream>
#include "NodeParser.h"
#include "CodeGenerator.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file_path>" << std::endl;
        return 1;
    }

    std::string outputFile = "a.out";
    std::string inputFile;

    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-o" && i + 1 < argc) {
            outputFile = argv[++i];
        } else {
            inputFile = argv[i];
        }
    }

    std::ifstream input(inputFile);
    if (!input.is_open()) {
        std::cerr << "Error: Could not open input file " << inputFile << std::endl;
        return 1;
    }

    std::string code((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    input.close();

    std::vector<char> mutable_code(code.begin(), code.end());
    mutable_code.push_back('\0');

    Token *token = TokenParser::tokenize(mutable_code.data());

    NodeParser nodeParser(*token);

    //nodeParser.codegen.MOV("x29", "xzr");
    nodeParser.main_func.PUSH("x29");
    nodeParser.main_func.MOV("x29", "sp");
    nodeParser.main_func.SUB("sp", "sp", std::to_string(nodeParser.get_total_offset()).c_str());

    for(int i = 0; nodeParser.code[i]; i++) {
        nodeParser.gen(nodeParser.code[i], nodeParser.main_func);
        nodeParser.main_func.POP("x0");
    }

    std::ofstream output(outputFile);
    if (!output.is_open()) {
        std::cerr << "Error: Could not open output file " << outputFile << std::endl;
        return 1;
    }
    std::string command = "cc -x assembler - -o " + outputFile;
    FILE* ccProcess = popen(command.c_str(), "w");

    if (!ccProcess) {
        std::cerr << "Error: Could not open pipe to compiler." << std::endl;
        return 1;
    }

    fwrite(nodeParser.get_integrated_code().c_str(), 1, nodeParser.get_integrated_code().size(), ccProcess);

    int result = pclose(ccProcess);
    if (result != 0) {
        std::cerr << "Error: Compilation failed with exit code " << result << std::endl;
        return 1;
    }

    std::cout << "Compilation succeeded. Executable created as '" << outputFile << "'" << std::endl;
    return 0;
}
