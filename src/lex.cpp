#include <cstdio>
#include <cstring>
#include <cctype>
#include <iostream>

#include "glob.h"
#include "lex.h"

static int c; 
static int linha = 1;

void makeToken(const char *nome, int val) 
{
    std::cout << '<' << nome << ", " << val << '>';
}

void voltaPonteiro(FILE *arquivo) 
{
    if (c != EOF) {
        ungetc(c, arquivo);
    }
}

void numero(FILE *arquivo)
{
    std::string valor(1, static_cast<char>(c));

    while ((c = fgetc(arquivo)) != EOF && std::isdigit(static_cast<unsigned char>(c))) {
        valor.push_back(static_cast<char>(c));
    }

    voltaPonteiro(arquivo);
    makeToken("numero", std::stoi(valor));
}

void palavra(FILE *arquivo)
{
    std::string palavra(1, static_cast<char>(c));

    while ((c = fgetc(arquivo)) != EOF && std::isalpha(static_cast<unsigned char>(c))) {
        palavra.push_back(static_cast<char>(c));
    }

    voltaPonteiro(arquivo);

    if (palavra == "INIT") {
        makeToken("PC_INIT", 0);
    } else if (palavra == "END") {
        makeToken("PC_END", 0);
    } else if (palavra == "PRINT") {
        makeToken("PC_PRINT", 0);
    } else if (palavra == "INPUT") {
        makeToken("PC_INPUT", 0);
    } else if (palavra == "VAR") {
        makeToken("PC_VAR", 0);
    } else if (palavra == "IF") {
        makeToken("PC_IF", 0);
    } else if (palavra == "ELSE") {
        makeToken("PC_ELSE", 0);
    } else {
        makeToken("ID", 0);
    }
}

void proximo_token(FILE *arquivo)
{
    while (true)
    {
        c = fgetc(arquivo);

        if (c == EOF) {
            break;
        } else if (c == ' ' || c == '\t') {
            continue;
        } else if (c == '\n') {
            linha++;
        } else if (c == '+') {
            makeToken("+", 0);
        } else if (c == '-') {
            makeToken("-", 0);
        } else if (c == '*') {
            makeToken("*", 0);
        } else if (c == '/') {
            makeToken("/", 0);
        } else if (c == '(') {
            makeToken("(", 0);
        } else if (c == ')') {
            makeToken(")", 0);
        } else if (c == ':') {
            c = fgetc(arquivo);

            if (c == '=') {
                makeToken(":=", 0);
            } else {
                voltaPonteiro(arquivo);
                std::cerr << "O caracter ':' na linha " << linha
                          << " nao esta seguido de '=.'\n";
                exit(EXIT_FAILURE);
            }
        } else if (c == '<') {
            c = fgetc(arquivo);

            if (c == '=') {
                makeToken("<=", 0);
            } else {
                makeToken("<", 0);
                voltaPonteiro(arquivo);
            }
        } else if (c == '>') {
            c = fgetc(arquivo);

            if (c == '=') {
                makeToken(">=", 0);
            } else {
                makeToken(">", 0);
                voltaPonteiro(arquivo);
            }
        } else if (c == '=') {
            makeToken("=", 0);
        } else if (std::isdigit(static_cast<unsigned char>(c))) {
            numero(arquivo);
        } else if (std::isalpha(static_cast<unsigned char>(c))) {
            palavra(arquivo);
        } else {
            std::cerr << "O caracter '" << static_cast<char>(c)
                      << "' na linha " << linha << " nao reconhecido.\n";
            exit(EXIT_FAILURE);
        }
    }
}
