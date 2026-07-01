#ifndef BASE_H
#define BASE_H

#include "matriz.h"

/*
Questao 3 - Bases em R2 e R3
    - le um conjunto de vetores
    - identifica automaticamente se esta em R2 ou R3
    - verifica se forma base
    - se nao formar, indica subconjunto/completacao
*/

void executarBases(void);

int conjuntoFormaBase(const Matriz *matrizVetores);

#endif