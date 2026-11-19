#ifndef art_lex_h
#define art_lex_h

#include <cstdio>
#include <stdexcept>
#include <vector>
#include "glob.h"

class LexicalError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

std::vector<Token> proximo_token(FILE *arquivo);
#endif
