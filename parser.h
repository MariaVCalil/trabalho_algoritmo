#ifndef PARSER_H
#define PARSER_H

#include "config.h"
#include "matriz.h"

/* Representa uma variavel encontrada na equacao */

typedef struct
{
    char nome;
    int coluna;

} Variavel;

/* Guarda todas as variaveis do sistema */

typedef struct
{
    Variavel lista[MAX_VARIAVEIS];
    int quantidade;

} Variaveis;

/* Guarda uma equacao ja interpretada (só numeros) */

typedef struct
{
    double coeficientes[MAX_VARIAVEIS];
    double resultado;
    int valida;

} Equacao;

/* Guarda as equacoes digitadas pelo usuario */

typedef struct
{
    char texto[MAX_EQUACOES][MAX_TAMANHO];
    int quantidade;

} SistemaEntrada;

void lerSistema(SistemaEntrada *entrada);

int identificarVariaveis(const SistemaEntrada *entrada, Variaveis *vars);

int montarSistema(const SistemaEntrada *entrada, const Variaveis *vars, Matriz *matriz);

void imprimirVariaveis(const Variaveis *vars);

#endif