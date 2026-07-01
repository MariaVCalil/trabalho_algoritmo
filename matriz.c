#include <stdio.h>

#include "matriz.h"

void inicializarMatriz(Matriz *m,
                       int linhas,
                       int colunas)
{
    m->linhas = linhas;
    m->colunas = colunas;

    for(int i = 0; i < MAX_LINHAS; i++)
    {
        for(int j = 0; j < MAX_COLUNAS; j++)
        {
            m->dados[i][j] = 0.0;
        }
    }
}

void copiarMatriz(const Matriz *origem,
                  Matriz *destino)
{
    destino->linhas = origem->linhas;
    destino->colunas = origem->colunas;

    for(int i = 0; i < MAX_LINHAS; i++)
    {
        for(int j = 0; j < MAX_COLUNAS; j++)
        {
            destino->dados[i][j] = origem->dados[i][j];
        }
    }
}

void imprimirMatriz(const Matriz *m)
{
    printf("\n");

    for(int i = 0; i < m->linhas; i++)
    {
        for(int j = 0; j < m->colunas; j++)
        {
            printf("%10.4lf", m->dados[i][j]);
        }

        printf("\n");
    }

    printf("\n");
}

double absoluto(double numero)
{
    if(numero < 0)
    {
        return -numero;
    }

    return numero;
}

int ehZero(double numero)
{
    return absoluto(numero) < EPSILON;
}

void trocarLinhas(Matriz *m,
                  int linha1,
                  int linha2)
{
    double auxiliar;

    for(int coluna = 0; coluna < m->colunas; coluna++)
    {
        auxiliar = m->dados[linha1][coluna];

        m->dados[linha1][coluna] = m->dados[linha2][coluna];

        m->dados[linha2][coluna] = auxiliar;
    }
}

void multiplicarLinha(Matriz *m,
                      int linha,
                      double escalar)
{
    for(int coluna = 0; coluna < m->colunas; coluna++)
    {
        m->dados[linha][coluna] *= escalar;
    }
}

void adicionarMultiplo(Matriz *m,
                       int destino,
                       int origem,
                       double fator)
{
    for(int coluna = 0; coluna < m->colunas; coluna++)
    {
        m->dados[destino][coluna] += fator * m->dados[origem][coluna];
    }
}