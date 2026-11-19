#include <iostream>
#include <string>
#include <vector>

#include "glob.h"
#include "parser.h"

namespace {

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens) : tokens_(tokens) {}

    bool executar()
    {
        while (!fim()) {
            if (corresponde(T_NOVA_LINHA)) {
                continue;
            }
            if (!comando()) {
                return false;
            }
            if (!separadorConsumido_ && !fim() &&
                !verifica(T_NOVA_LINHA) && !verifica(T_DEDENT)) {
                return erro("esperado fim de linha");
            }
        }
        return true;
    }

private:
    const std::vector<Token>& tokens_;
    std::size_t posicao_ = 0;
    bool separadorConsumido_ = false;

    bool fim() const { return posicao_ >= tokens_.size(); }

    const Token* atual() const
    {
        return fim() ? nullptr : &tokens_[posicao_];
    }

    bool verifica(int tipo) const
    {
        return atual() != nullptr && atual()->tipo == tipo;
    }

    bool proximoEh(int tipo) const
    {
        return posicao_ + 1 < tokens_.size() &&
               tokens_[posicao_ + 1].tipo == tipo;
    }

    bool corresponde(int tipo)
    {
        if (!verifica(tipo)) {
            return false;
        }
        ++posicao_;
        return true;
    }

    bool exigir(int tipo, const std::string& esperado)
    {
        if (corresponde(tipo)) {
            return true;
        }
        return erro("esperado " + esperado);
    }

    bool erro(const std::string& mensagem) const
    {
        const int linha = atual() == nullptr ? ultimaLinha() : atual()->linha;
        std::cerr << "Erro sintatico na linha " << linha << ": "
                  << mensagem << ".\n";
        return false;
    }

    int ultimaLinha() const
    {
        return tokens_.empty() ? 1 : tokens_.back().linha;
    }

    bool comando()
    {
        separadorConsumido_ = false;
        if (corresponde(PC_PRINT)) {
            return imprimir();
        }
        if (corresponde(PC_VAR)) {
            if (!exigir(ID, "um nome de variavel")) {
                return false;
            }
            if (corresponde(T_ATRIB) && !expressao()) {
                return false;
            }
            return true;
        }
        if (corresponde(PC_INPUT)) {
            const bool comParenteses = corresponde(T_LPARENT);
            if (!exigir(ID, "um nome de variavel")) {
                return false;
            }
            return !comParenteses || exigir(T_RPARENT, "')'");
        }
        if (corresponde(PC_IF)) {
            if (!condicional()) {
                return false;
            }
            separadorConsumido_ = true;
            return true;
        }
        if (verifica(ID)) {
            ++posicao_;
            if (!exigir(T_ATRIB, "'='")) {
                return false;
            }
            return expressao();
        }
        return erro("esperado 'print', 'var', 'input', 'if' ou atribuicao");
    }

    bool imprimir()
    {
        if (verifica(T_NOVA_LINHA) || verifica(T_DEDENT) || fim()) {
            return true;
        }
        if (corresponde(T_LPARENT)) {
            if (corresponde(T_RPARENT)) {
                return true;
            }
            if (!expressao()) {
                return false;
            }
            return exigir(T_RPARENT, "')'");
        }
        return expressao();
    }

    bool condicional()
    {
        if (!expressao() || !exigir(T_DOIS_PONTOS, "':'") ||
            !exigir(T_NOVA_LINHA, "uma nova linha depois de ':'") ||
            !exigir(T_INDENT, "um bloco indentado")) {
            return false;
        }

        if (!bloco()) {
            return false;
        }
        if (corresponde(PC_ELSE)) {
            if (!exigir(T_DOIS_PONTOS, "':'") ||
                !exigir(T_NOVA_LINHA, "uma nova linha depois de ':'") ||
                !exigir(T_INDENT, "um bloco indentado")) {
                return false;
            }
            return bloco();
        }
        return true;
    }

    bool bloco()
    {
        bool encontrouComando = false;
        while (!fim() && !verifica(T_DEDENT)) {
            if (verifica(T_NOVA_LINHA) && proximoEh(T_DEDENT)) {
                corresponde(T_NOVA_LINHA);
                break;
            }
            if (corresponde(T_NOVA_LINHA)) {
                continue;
            }
            if (!comando()) {
                return false;
            }
            encontrouComando = true;
            if (!separadorConsumido_ && !fim() &&
                !verifica(T_NOVA_LINHA) && !verifica(T_DEDENT)) {
                return erro("esperado fim de linha dentro do bloco");
            }
        }
        if (!encontrouComando) {
            return erro("o bloco precisa conter pelo menos um comando");
        }
        return exigir(T_DEDENT, "o fim do bloco indentado");
    }

    bool expressao()
    {
        return comparacao();
    }

    bool comparacao()
    {
        if (!adicao()) {
            return false;
        }
        while (corresponde(T_MENOR) || corresponde(T_MENOR_I) ||
               corresponde(T_MAIOR) || corresponde(T_MAIOR_I) ||
               corresponde(T_IGUAL) || corresponde(T_DIFERENTE)) {
            if (!adicao()) {
                return false;
            }
        }
        return true;
    }

    bool adicao()
    {
        if (!multiplicacao()) {
            return false;
        }
        while (corresponde(OP_MAIS) || corresponde(OP_MENOS)) {
            if (!multiplicacao()) {
                return false;
            }
        }
        return true;
    }

    bool multiplicacao()
    {
        if (!unario()) {
            return false;
        }
        while (corresponde(OP_MULT) || corresponde(OP_DIVI) ||
               corresponde(OP_RES)) {
            if (!unario()) {
                return false;
            }
        }
        return true;
    }

    bool unario()
    {
        if (corresponde(OP_MAIS) || corresponde(OP_MENOS)) {
            return unario();
        }
        return potencia();
    }

    bool potencia()
    {
        if (!primario()) {
            return false;
        }
        if (corresponde(OP_EXP)) {
            return unario();
        }
        return true;
    }

    bool primario()
    {
        if (corresponde(T_INT) || corresponde(T_STRING) || corresponde(ID)) {
            return true;
        }
        if (corresponde(PC_INPUT)) {
            if (!exigir(T_LPARENT, "'('")) {
                return false;
            }
            if (!corresponde(T_RPARENT)) {
                if (!expressao() || !exigir(T_RPARENT, "')'")) {
                    return false;
                }
            }
            return true;
        }
        if (corresponde(T_LPARENT)) {
            if (!expressao()) {
                return false;
            }
            return exigir(T_RPARENT, "')'");
        }
        return erro("esperado numero, string, identificador ou '('");
    }
};

} // namespace

bool analisar(const std::vector<Token>& tokens)
{
    return Parser(tokens).executar();
}
