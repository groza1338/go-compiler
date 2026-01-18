#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include "golang_parser.hpp"

namespace fs = std::filesystem;

extern FILE *yyin;

// TODO Подумать над округлением float значений, (см. лс в ТГ)
// TODO Почему-то вылетает ошибка, что нельзя присвоить значение типа int во float64, хотя так можно (файл arrays.go строка 17) (запусти в компиляторе на сайте код)
// TODO Реализовать сортировку одномерного массива пузырьком, сверить результаты нашего компилятора с сайтом

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

    // const char *rawEnv = std::getenv("PRINT_RAW_AST");
    // bool wantRaw = rawEnv && *rawEnv == '1';
    // bool wantTyped = !wantRaw && semCtx.errors.empty();
    // if (wantRaw || wantTyped) {
    //     AstNode::setShowTypes(wantTyped);
    //     cout << "digraph AST {\n";
    //     cout << root->toDot();
    //     cout << "}\n";
    // }

    if (semCtx.errors.empty()) {
        fs::path outDir = fs::path("generated_classes");
        fs::path outPath = outDir / "Main.class";
        BytecodeContext bytecode;
        root->emitBytecode(bytecode);
        bytecode.writeTo(outPath);
    }

    return 0;
}
