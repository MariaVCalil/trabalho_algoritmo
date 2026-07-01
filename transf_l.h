#ifndef TRANSF_L_H
#define TRANSF_L_H

#include "matriz.h"

/*
    questão 2 - Transformações Lineares
    - le uma transformacao linear
    - identifica automaticamente dominio e contradominio
    - monta a matriz associada
    - calcula dimensao do nucleo
    - calcula dimensao da imagem
    - verifica injetividade, sobrejetividade e bijetividade
*/

void executarTransformacaoLinear(void);

int dimensaoImagem(const Matriz *matriz);

int dimensaoNucleo(const Matriz *matriz);

int transformacaoInjetiva(const Matriz *matriz);

int transformacaoSobrejetiva(const Matriz *matriz);

int transformacaoBijetiva(const Matriz *matriz);

#endif
