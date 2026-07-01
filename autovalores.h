#ifndef AUTOVALORES_H
#define AUTOVALORES_H

#include "matriz.h"

/* Questao 4 - Autovalores e autovetores em R2 
    - le um operador T: R2 -> R2
    - monta a matriz associada
    - calcula autovalores reais
    - calcula autovetores
    - informa quando nao houver autovalores reais
*/

void executarAutovalores(void);

int calcularAutovalores2x2(const Matriz *matriz, double autovalores[2], int multiplicidades[2], int *quantidade);

void calcularAutovetor2x2(const Matriz *matriz, double lambda, double vetor[2]);

int montarMatrizOperadorR2(const char texto[], Matriz *matriz, char variaveis[2]);

#endif