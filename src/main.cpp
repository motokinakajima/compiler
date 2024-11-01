//
// Created by 中嶋幹 on 10/17/24.
//

#include "main.h"

#include <iostream>
#include <fstream>
#include "NodeParser.h"
#include "CodeGenerator.h"

const std::string basicFunctions = R"(
#include <cstdio>
#include <string>

extern "C" void basic_print(int a) {
    printf("%s\n", std::to_string(a).c_str());
    return;
}
)";

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

    for(int i = 0; i < nodeParser.code.size() ; i++) {
        nodeParser.gen(nodeParser.code[i], nodeParser.main_func);
        nodeParser.main_func.POP("x0");
    }

    nodeParser.main_func.CODE(".extern _basic_print");

    std::ofstream output(outputFile);
    if (!output.is_open()) {
        std::cerr << "Error: Could not open output file " << outputFile << std::endl;
        return 1;
    }

    const std::string tempFile = "basic_functions.cpp";
    const std::string assemblyFile = "temp.s";

    std::ofstream tempOut(tempFile);
    if (!tempOut) {
        std::cerr << "Error: Could not create temporary file." << std::endl;
        return 1;
    }
    tempOut << basicFunctions;
    tempOut.close();

    std::string command = "cc -c " + tempFile + " -o basic_functions.o";
    if (system(command.c_str()) != 0) {
        std::cerr << "Error: Compilation of basic functions failed." << std::endl;
        return 1;
    }

    std::ofstream asmOut(assemblyFile);
    if (!asmOut) {
        std::cerr << "Error: Could not create temporary assembly file." << std::endl;
        return 1;
    }
    asmOut << nodeParser.get_integrated_code();
    asmOut.close();

    command = "as " + assemblyFile + " -o temp.o"; // Use 'as' for assembly compilation
    if (system(command.c_str()) != 0) {
        std::cerr << "Error: Assembly compilation failed." << std::endl;
        return 1;
    }

    command = "c++ temp.o basic_functions.o -o " + outputFile;
    if (system(command.c_str()) != 0) {
        std::cerr << "Error: Linking failed." << std::endl;
        return 1;
    }

    remove(assemblyFile.c_str());
    remove(tempFile.c_str());
    remove("temp.o");
    remove("basic_functions.o");

    std::cout << "Compilation succeeded. Executable created as '" << outputFile << "'" << std::endl;
    return 0;
}
