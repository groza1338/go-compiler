#include <iostream>
#include <fstream>
#include "golang_parser.hpp"

yyFlexLexer *lexer;

int yylex() {
    return lexer->yylex();
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <file_path>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << argv[1] << std::endl;
        return 1;
    }

    lexer = new yyFlexLexer();      // создаём объект лексера
    lexer->switch_streams(&file, &std::cout);  // Передаем файл для анализа
    yyparse();

    return 0;
}
