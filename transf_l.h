#ifndef TRANSF_L_H
#define TRANSF_L_H

#include "matriz.h"

/*
==========================================================
Questao 2 - Transformacoes lineares

O programa:
    - le uma transformacao linear
    - identifica automaticamente dominio e contradominio
    - monta a matriz associada
    - calcula dimensao do nucleo
    - calcula dimensao da imagem
    - verifica injetividade, sobrejetividade e bijetividade
==========================================================
*/

void executarTransformacaoLinear(void);

int dimensaoImagem(const Matriz *matriz);

int dimensaoNucleo(const Matriz *matriz);

int ehTransformacaoInjetiva(const Matriz *matriz);

int ehTransformacaoSobrejetiva(const Matriz *matriz);

int ehTransformacaoBijetiva(const Matriz *matriz);

#endif
