#include <stdlib.h>
#include <iostream>

#include "lex.h"
#include "glob.h"

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
 
    proximo_token(file);
 
    fclose(file);
    return 0; 
}