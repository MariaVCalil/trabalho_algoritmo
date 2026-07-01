#ifndef PARSER_H
#define PARSER_H

#include "config.h"
#include "matriz.h"

typedef struct
{
    char nome;
    int coluna;

} Variavel;

typedef struct
{
    Variavel lista[MAX_VARIAVEIS];
    int quantidade;

} Variaveis;


typedef struct
{
    double coeficientes[MAX_VARIAVEIS];
    double resultado;

    int valida;

} Equacao;



typedef struct
{
    char texto[MAX_EQUACOES][MAX_TAMANHO];
    int quantidade;

} SistemaEntrada;



void lerSistema(SistemaEntrada *entrada);


int identificarVariaveis(const SistemaEntrada *entrada,Variaveis *vars);



int montarSistema(const SistemaEntrada *entrada,const Variaveis *vars, Matriz *matriz);


void imprimirVariaveis(const Variaveis *vars);

#endif