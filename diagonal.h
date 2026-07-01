#ifndef DIAGONAL_H
#define DIAGONAL_H

#include "matriz.h"

/* Questao 5 - Diagonalizacao em R 
    - le um operador T: R2 -> R2
    - verifica se e diagonalizavel em R
    - se for, apresenta a matriz diagonal D */

void executarDiagonalizacao(void);

int diagonalizavelR2(const Matriz *matriz);

int montarMatrizDiagonal(const Matriz *matriz, Matriz *diagonal);

#endif