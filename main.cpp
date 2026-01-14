#include <iostream>
#include <fstream>
#include <filesystem>
#include "golang_parser.hpp"

namespace fs = std::filesystem;

extern FILE *yyin;

extern int yyparse();

extern ProgramNode *root;

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

    SemanticContext semCtx;
    root->semantics(semCtx);
    const char *semDir = std::getenv("SEMANTIC_OUT_DIR");
    fs::path semPath = semDir && *semDir
        ? fs::path(semDir) / (fs::path(inputFile).filename().string() + ".sem.txt")
        : fs::path(inputFile + ".sem.txt");

    if (!semCtx.errors.empty()) {
        fs::create_directories(semPath.parent_path());
        std::ofstream semOut(semPath, std::ios::trunc);
        for (const auto &err : semCtx.errors) {
            semOut << "Semantic error: " << err << "\n";
        }
    } else {
        std::filesystem::remove(semPath);
    }

    cout << "digraph AST {\n";
    cout << root->toDot();
    cout << "}\n";

    return 0;
}
