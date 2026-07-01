#ifndef MATRIZ_H
#define MATRIZ_H

#include "config.h"
// apenas define a estrutura e funções basicas
/*
    Estrutura usada para representar matrizes no projeto.

    linhas  -> quantidade de linhas usadas
    colunas -> quantidade de colunas usadas
    dados   -> valores armazenados na matriz
*/

typedef struct
{
    int linhas;
    int colunas;
    double dados[MAX_LINHAS][MAX_COLUNAS];

} Matriz;

/* Inicializacao */

void inicializarMatriz(Matriz *m, int linhas, int colunas);

/* Operacoes elementares de linha */

void trocarLinhas(Matriz *m, int linha1, int linha2);

void multiplicarLinha(Matriz *m, int linha, double escalar);

void adicionarMultiplo(Matriz *m, int destino, int origem, double fator);

/* Utilidades */

void copiarMatriz(const Matriz *origem, Matriz *destino);

void imprimirMatriz(const Matriz *m);

double absoluto(double numero);

int ehZero(double numero);

#endif