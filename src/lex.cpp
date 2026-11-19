#include <cctype>
#include <cstdio>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "glob.h"
#include "lex.h"

namespace {

class Lexer {
public:
    explicit Lexer(std::string fonte) : fonte_(std::move(fonte)) {}

    std::vector<Token> executar()
    {
        while (posicao_ < fonte_.size()) {
            if (inicioLinha_) {
                processarIndentacao();
                if (posicao_ >= fonte_.size()) {
                    break;
                }
                if (fonte_[posicao_] == '\n') {
                    ++posicao_;
                    ++linha_;
                    continue;
                }
            }

            const char atual = fonte_[posicao_];
            if (atual == ' ' || atual == '\t' || atual == '\r') {
                ++posicao_;
            } else if (atual == '\n') {
                tokens_.push_back({T_NOVA_LINHA, "\\n", linha_});
                ++posicao_;
                ++linha_;
                inicioLinha_ = true;
            } else if (atual == '#') {
                while (posicao_ < fonte_.size() && fonte_[posicao_] != '\n') {
                    ++posicao_;
                }
            } else {
                inicioLinha_ = false;
                lerToken();
            }
        }

        if (!tokens_.empty() && tokens_.back().tipo != T_NOVA_LINHA) {
            tokens_.push_back({T_NOVA_LINHA, "\\n", linha_});
        }
        while (niveisIndentacao_.size() > 1) {
            niveisIndentacao_.pop_back();
            tokens_.push_back({T_DEDENT, "", linha_});
        }
        return tokens_;
    }

private:
    std::string fonte_;
    std::size_t posicao_ = 0;
    int linha_ = 1;
    bool inicioLinha_ = true;
    std::vector<int> niveisIndentacao_{0};
    std::vector<Token> tokens_;

    void processarIndentacao()
    {
        int espacos = 0;
        while (posicao_ < fonte_.size()) {
            if (fonte_[posicao_] == ' ') {
                ++espacos;
                ++posicao_;
            } else if (fonte_[posicao_] == '\t') {
                espacos += 4 - espacos % 4;
                ++posicao_;
            } else {
                break;
            }
        }

        if (posicao_ >= fonte_.size() || fonte_[posicao_] == '\n' ||
            fonte_[posicao_] == '#') {
            return;
        }

        const int indentacaoAtual = niveisIndentacao_.back();
        if (espacos > indentacaoAtual) {
            niveisIndentacao_.push_back(espacos);
            tokens_.push_back({T_INDENT, "", linha_});
        } else if (espacos < indentacaoAtual) {
            while (niveisIndentacao_.size() > 1 &&
                   espacos < niveisIndentacao_.back()) {
                niveisIndentacao_.pop_back();
                tokens_.push_back({T_DEDENT, "", linha_});
            }
            if (espacos != niveisIndentacao_.back()) {
                throw LexicalError(
                    "Indentacao inconsistente na linha " +
                    std::to_string(linha_) + ".");
            }
        }
        inicioLinha_ = false;
    }

    void adicionar(int tipo, std::string lexema)
    {
        tokens_.push_back({tipo, std::move(lexema), linha_});
    }

    void lerToken()
    {
        const char atual = fonte_[posicao_];
        if (std::isalpha(static_cast<unsigned char>(atual)) || atual == '_') {
            const std::size_t inicio = posicao_++;
            while (posicao_ < fonte_.size()) {
                const unsigned char c =
                    static_cast<unsigned char>(fonte_[posicao_]);
                if (!std::isalnum(c) && c != '_') {
                    break;
                }
                ++posicao_;
            }

            std::string palavra = fonte_.substr(inicio, posicao_ - inicio);
            if (palavra == "print") {
                adicionar(PC_PRINT, palavra);
            } else if (palavra == "input") {
                adicionar(PC_INPUT, palavra);
            } else if (palavra == "var") {
                adicionar(PC_VAR, palavra);
            } else if (palavra == "if") {
                adicionar(PC_IF, palavra);
            } else if (palavra == "else") {
                adicionar(PC_ELSE, palavra);
            } else {
                adicionar(ID, palavra);
            }
            return;
        }

        if (std::isdigit(static_cast<unsigned char>(atual))) {
            const std::size_t inicio = posicao_++;
            while (posicao_ < fonte_.size() &&
                   std::isdigit(static_cast<unsigned char>(fonte_[posicao_]))) {
                ++posicao_;
            }
            adicionar(T_INT, fonte_.substr(inicio, posicao_ - inicio));
            return;
        }

        if (atual == '"' || atual == '\'') {
            const char aspas = atual;
            const std::size_t inicio = posicao_++;
            while (posicao_ < fonte_.size() && fonte_[posicao_] != aspas &&
                   fonte_[posicao_] != '\n') {
                if (fonte_[posicao_] == '\\') {
                    ++posicao_;
                    if (posicao_ >= fonte_.size() ||
                        fonte_[posicao_] == '\n') {
                        break;
                    }
                }
                ++posicao_;
            }
            if (posicao_ >= fonte_.size() || fonte_[posicao_] != aspas) {
                throw LexicalError(
                    "String sem fechamento na linha " +
                    std::to_string(linha_) + ".");
            }
            ++posicao_;
            adicionar(T_STRING, fonte_.substr(inicio, posicao_ - inicio));
            return;
        }

        ++posicao_;
        switch (atual) {
        case '+': adicionar(OP_MAIS, "+"); return;
        case '-': adicionar(OP_MENOS, "-"); return;
        case '*':
            if (posicao_ < fonte_.size() && fonte_[posicao_] == '*') {
                ++posicao_;
                adicionar(OP_EXP, "**");
            } else {
                adicionar(OP_MULT, "*");
            }
            return;
        case '/': adicionar(OP_DIVI, "/"); return;
        case '^': adicionar(OP_EXP, "^"); return;
        case '%': adicionar(OP_RES, "%"); return;
        case '(': adicionar(T_LPARENT, "("); return;
        case ')': adicionar(T_RPARENT, ")"); return;
        case ':':
            if (posicao_ < fonte_.size() && fonte_[posicao_] == '=') {
                ++posicao_;
                adicionar(T_ATRIB, ":=");
            } else {
                adicionar(T_DOIS_PONTOS, ":");
            }
            return;
        case '=':
            if (posicao_ < fonte_.size() && fonte_[posicao_] == '=') {
                ++posicao_;
                adicionar(T_IGUAL, "==");
            } else {
                adicionar(T_ATRIB, "=");
            }
            return;
        case '!':
            if (posicao_ < fonte_.size() && fonte_[posicao_] == '=') {
                ++posicao_;
                adicionar(T_DIFERENTE, "!=");
                return;
            }
            break;
        case '<':
            if (posicao_ < fonte_.size() && fonte_[posicao_] == '=') {
                ++posicao_;
                adicionar(T_MENOR_I, "<=");
            } else {
                adicionar(T_MENOR, "<");
            }
            return;
        case '>':
            if (posicao_ < fonte_.size() && fonte_[posicao_] == '=') {
                ++posicao_;
                adicionar(T_MAIOR_I, ">=");
            } else {
                adicionar(T_MAIOR, ">");
            }
            return;
        case '\0':
            break;
        }

        throw LexicalError(
            "Caractere '" + std::string(1, atual) + "' na linha " +
            std::to_string(linha_) + " nao reconhecido.");
    }
};

} // namespace

std::vector<Token> proximo_token(FILE *arquivo)
{
    if (arquivo == nullptr) {
        throw LexicalError("Arquivo de entrada invalido.");
    }

    std::string fonte;
    int caractere;
    while ((caractere = fgetc(arquivo)) != EOF) {
        fonte.push_back(static_cast<char>(caractere));
    }
    if (ferror(arquivo)) {
        throw LexicalError("Falha ao ler o arquivo de entrada.");
    }

    return Lexer(std::move(fonte)).executar();
}
