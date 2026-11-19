#include <stdlib.h>
#include <iostream>
#include "lex.h"
#include "glob.h"
#include "parser.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Uso: " << argv[0] << " <arquivo>\n";
        return EXIT_FAILURE;
    }

    FILE *file = fopen(argv[1], "r");
    if (file == nullptr)
    {
        std::cerr << "Erro ao abrir o arquivo: " << argv[1] << "\n";
        exit(EXIT_FAILURE);
    }
 
    try {
        const std::vector<Token> tokens = proximo_token(file);
        fclose(file);
        return analisar(tokens) ? EXIT_SUCCESS : EXIT_FAILURE;
    } catch (const LexicalError& erro) {
        fclose(file);
        std::cerr << "Erro: " << erro.what() << "\n";
        return EXIT_FAILURE;
    }
}