#ifndef art_glob_h
#define art_glob_h

#include <stdio.h>
#include <stdlib.h>
#include <string>

enum {
    // palavras chave
    PC_PRINT, PC_INPUT, PC_VAR, PC_IF, PC_ELSE,

    // numeros
    T_INT,
    T_STRING,

    // operadores
    OP_MAIS,
    OP_MENOS,
    OP_MULT,
    OP_DIVI,
    OP_EXP,
    OP_RES,

    // ( ) := < > <= >=  =
    T_LPARENT, T_RPARENT, T_ATRIB, T_MENOR, T_MAIOR, T_MENOR_I,
    T_MAIOR_I, T_IGUAL, T_DIFERENTE, T_DOIS_PONTOS,

    // estrutura de linhas e blocos
    T_NOVA_LINHA, T_INDENT, T_DEDENT,

    // identificador
    ID
};

typedef struct {
    int tipo;
    std::string lexema;
    int linha;
} Token;

#endif