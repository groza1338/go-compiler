#include <iostream>
#include <fstream>
#include <filesystem>
#include "golang_parser.hpp"

namespace fs = std::filesystem;

extern FILE *yyin;

extern int yyparse();

ProgramNode *root;

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <file_path>" << std::endl;
        return 1;
    }

    std::string inputFile = argv[1];

    yyin = fopen(inputFile.c_str(), "r");
    if (!yyin) {
        cout << ("Could not open input file: '" + inputFile + "'");
        return 1;
    }

    int parse_result = yyparse();

    if (parse_result != 0) {
        cout << ("Parsing failed with code: '" + std::to_string(parse_result) + "'");
        return 1;
    }

    if (!root) {
        cout << ("No parse tree generated");
        return 1;
    }

    cout << "digraph AST {\n";
    cout << root->toDot();
    cout << "}\n";

    return 0;
}
