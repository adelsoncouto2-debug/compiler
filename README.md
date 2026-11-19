# Compiler

Este projeto contém o lexer e o início de um parser para uma linguagem pequena
com sintaxe inspirada em Python.

## Compilar e validar um arquivo

```sh
make
./exe/compiler teste.txt
```

O parser valida a sintaxe, mas ainda não executa o programa nem constrói uma
árvore sintática.

## Sintaxe reconhecida

```text
print(expressao)
print("texto")
nome = expressao
nome = input()
var nome
var nome = expressao
input(nome)

if expressao:
    print(expressao)
else:
    print(expressao)
```

Também são aceitos `print expressao`, `input nome` e `:=` como forma legada de
atribuição. As palavras-chave são minúsculas. Expressões aceitam inteiros,
strings entre aspas simples ou duplas, identificadores, chamadas a `input`,
parênteses, `+`, `-`, `*`, `/`, `%`, `**`, `^` e comparações `<`, `<=`, `>`,
`>=`, `==`, `!=`. Linhas iniciadas por `#` são comentários.