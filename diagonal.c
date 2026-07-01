#include <stdio.h>
#include <string.h>

#include "diagonal.h"
#include "autovalores.h"
#include "matriz.h"
#include "config.h"

/*
==========================================================
Funcoes privadas
==========================================================
*/

static void removerQuebraLinha(char texto[]);
static void removerEspacos(char texto[]);

static int ehMatrizLambdaIdentidade(const Matriz *matriz,
                                    double lambda);

static void pausarDiagonal(void);


/*
==========================================================
Funcao principal da Questao 5.
==========================================================
*/

void executarDiagonalizacao(void)
{
    char texto[MAX_TAMANHO];
    char variaveis[2];

    Matriz matriz;
    Matriz diagonal;

    double autovalores[2];
    int multiplicidades[2];
    int quantidadeAutovalores = 0;

    printf("\nQUESTAO 5 - DIAGONALIZACAO\n");

    printf("\nDigite o operador no formato:\n");
    printf("  T(x,y)=(3x-5/2y,1/2y)\n");
    printf("  T(x,y)=(3x+y,3y)\n");
    printf("  T(x,y)=(-y,x)\n");
    printf("  T(x,y)=(2x+2y,-x+4y)\n");

    printf("\nOperador: ");

    if(fgets(texto, MAX_TAMANHO, stdin) == NULL)
    {
        printf("\nErro de leitura.\n");
        pausarDiagonal();
        return;
    }

    removerQuebraLinha(texto);
    removerEspacos(texto);

    if(strlen(texto) == 0)
    {
        printf("\nNenhum operador foi digitado.\n");
        pausarDiagonal();
        return;
    }

    if(!montarMatrizOperadorR2(texto, &matriz, variaveis))
    {
        printf("\nErro: operador invalido.\n");
        printf("Use um operador linear de R2 para R2 no formato T(x,y)=(..., ...).\n");
        pausarDiagonal();
        return;
    }

    if(!calcularAutovalores2x2(&matriz,
                               autovalores,
                               multiplicidades,
                               &quantidadeAutovalores))
    {
        printf("\nResultado:\n");
        printf("O operador nao e diagonalizavel em R.\n");
        printf("Motivo: nao possui autovalores reais.\n");

        pausarDiagonal();
        return;
    }

    if(!ehDiagonalizavelR2(&matriz))
    {
        printf("\nResultado:\n");
        printf("O operador nao e diagonalizavel em R.\n");
        printf("Motivo: nao possui autovetores suficientes para formar uma base de R2.\n");

        pausarDiagonal();
        return;
    }

    montarMatrizDiagonal(&matriz, &diagonal);

    printf("\nResultado:\n");
    printf("O operador e diagonalizavel em R.\n");

    printf("\nMatriz diagonal D:\n");
    imprimirMatriz(&diagonal);

    pausarDiagonal();
}


/*
==========================================================
Verifica se uma matriz 2x2 e diagonalizavel em R.

Regras:
    - Se nao tem autovalores reais, nao e diagonalizavel em R.
    - Se tem dois autovalores reais distintos, e diagonalizavel.
    - Se tem um autovalor repetido, so e diagonalizavel se
      A = lambda I.
==========================================================
*/

int ehDiagonalizavelR2(const Matriz *matriz)
{
    double autovalores[2];
    int multiplicidades[2];
    int quantidadeAutovalores = 0;

    if(!calcularAutovalores2x2(matriz,
                               autovalores,
                               multiplicidades,
                               &quantidadeAutovalores))
    {
        return 0;
    }

    if(quantidadeAutovalores == 2)
    {
        return 1;
    }

    if(quantidadeAutovalores == 1 && multiplicidades[0] == 2)
    {
        return ehMatrizLambdaIdentidade(matriz, autovalores[0]);
    }

    return 0;
}


/*
==========================================================
Monta a matriz diagonal D.

Se os autovalores forem distintos:
    D = [lambda1   0    ]
        [   0    lambda2]

Se o autovalor for repetido e a matriz for lambda I:
    D = [lambda  0]
        [0  lambda]
==========================================================
*/

int montarMatrizDiagonal(const Matriz *matriz,
                         Matriz *diagonal)
{
    double autovalores[2];
    int multiplicidades[2];
    int quantidadeAutovalores = 0;

    inicializarMatriz(diagonal, 2, 2);

    if(!calcularAutovalores2x2(matriz,
                               autovalores,
                               multiplicidades,
                               &quantidadeAutovalores))
    {
        return 0;
    }

    if(quantidadeAutovalores == 2)
    {
        diagonal->dados[0][0] = autovalores[0];
        diagonal->dados[1][1] = autovalores[1];

        return 1;
    }

    if(quantidadeAutovalores == 1 &&
       multiplicidades[0] == 2 &&
       ehMatrizLambdaIdentidade(matriz, autovalores[0]))
    {
        diagonal->dados[0][0] = autovalores[0];
        diagonal->dados[1][1] = autovalores[0];

        return 1;
    }

    return 0;
}


/*
==========================================================
Verifica se A = lambda I.

Exemplo:
    [3 0]
    [0 3]
==========================================================
*/

static int ehMatrizLambdaIdentidade(const Matriz *matriz,
                                    double lambda)
{
    if(!ehZero(matriz->dados[0][0] - lambda))
    {
        return 0;
    }

    if(!ehZero(matriz->dados[1][1] - lambda))
    {
        return 0;
    }

    if(!ehZero(matriz->dados[0][1]))
    {
        return 0;
    }

    if(!ehZero(matriz->dados[1][0]))
    {
        return 0;
    }

    return 1;
}


/*
==========================================================
Remove o \n do fgets.
==========================================================
*/

static void removerQuebraLinha(char texto[])
{
    for(int i = 0; texto[i] != '\0'; i++)
    {
        if(texto[i] == '\n')
        {
            texto[i] = '\0';
            return;
        }
    }
}


/*
==========================================================
Remove espacos e tabulacoes.
==========================================================
*/

static void removerEspacos(char texto[])
{
    int i = 0;
    int j = 0;

    while(texto[i] != '\0')
    {
        if(texto[i] != ' ' && texto[i] != '\t')
        {
            texto[j] = texto[i];
            j++;
        }

        i++;
    }

    texto[j] = '\0';
}


/*
==========================================================
Pausa.
==========================================================
*/

static void pausarDiagonal(void)
{
    char linha[20];

    printf("\nPressione ENTER para continuar...");
    fgets(linha, sizeof(linha), stdin);
}