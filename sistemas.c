#include <stdio.h>

#include "sistemas.h"

/*
==========================================================
Funcoes privadas
==========================================================
*/

static void montarMatrizCoeficientes(const Matriz *aumentada,Matriz *coeficientes);

static void reduzirGaussJordan(Matriz *m,int quantidadeVariaveis);

static int primeiraColunaNaoZero(const Matriz *m,int linha,int quantidadeVariaveis);

static void identificarPivos(const Matriz *m,int quantidadeVariaveis,int pivoPorLinha[MAX_LINHAS], int colunaEhPivo[MAX_VARIAVEIS]);

static void imprimirSolucaoSPD(const Matriz *reduzida,
                               const Variaveis *vars);

static void imprimirSolucaoSPI(const Matriz *reduzida,
                               const Variaveis *vars);

static void zerarValoresPequenos(Matriz *m);


/*
==========================================================
Escalona uma matriz.

Essa funcao e generica, ou seja, pode ser usada tanto para
matriz de coeficientes A quanto para matriz aumentada A|b.
==========================================================
*/

void escalonarMatriz(Matriz *m)
{
    int linhaPivo = 0;

    for(int coluna = 0;
        coluna < m->colunas && linhaPivo < m->linhas;
        coluna++)
    {
        int melhorLinha = -1;

        for(int linha = linhaPivo; linha < m->linhas; linha++)
        {
            if(!ehZero(m->dados[linha][coluna]))
            {
                melhorLinha = linha;
                break;
            }
        }

        if(melhorLinha == -1)
        {
            continue;
        }

        if(melhorLinha != linhaPivo)
        {
            trocarLinhas(m, melhorLinha, linhaPivo);
        }

        double pivo = m->dados[linhaPivo][coluna];

        if(!ehZero(pivo))
        {
            multiplicarLinha(m, linhaPivo, 1.0 / pivo);
        }

        for(int linha = linhaPivo + 1; linha < m->linhas; linha++)
        {
            double fator = -m->dados[linha][coluna];

            if(!ehZero(fator))
            {
                adicionarMultiplo(m, linha, linhaPivo, fator);
            }
        }

        linhaPivo++;
    }

    zerarValoresPequenos(m);
}


/*
==========================================================
Calcula o posto de uma matriz.

Posto = quantidade de linhas nao nulas apos escalonamento.
==========================================================
*/

int calcularPosto(const Matriz *m)
{
    Matriz copia;
    int posto = 0;

    copiarMatriz(m, &copia);

    escalonarMatriz(&copia);

    for(int i = 0; i < copia.linhas; i++)
    {
        int linhaNaoNula = 0;

        for(int j = 0; j < copia.colunas; j++)
        {
            if(!ehZero(copia.dados[i][j]))
            {
                linhaNaoNula = 1;
                break;
            }
        }

        if(linhaNaoNula)
        {
            posto++;
        }
    }

    return posto;
}


/*
==========================================================
Classifica o sistema.

Teoria usada:

Se posto(A) != posto(A|b)
    SI

Se posto(A) == posto(A|b) == numero de variaveis
    SPD

Se posto(A) == posto(A|b) < numero de variaveis
    SPI
==========================================================
*/

TipoSistema classificarSistema(const Matriz *aumentada)
{
    Matriz coeficientes;

    int numeroVariaveis = aumentada->colunas - 1;

    montarMatrizCoeficientes(aumentada, &coeficientes);

    int postoA = calcularPosto(&coeficientes);
    int postoAB = calcularPosto(aumentada);

    if(postoA != postoAB)
    {
        return SISTEMA_SI;
    }

    if(postoA == numeroVariaveis)
    {
        return SISTEMA_SPD;
    }

    return SISTEMA_SPI;
}


/*
==========================================================
Resolve o sistema.

Primeiro classifica.
Depois aplica Gauss-Jordan para imprimir a solucao.
==========================================================
*/

void resolverSistema(const Matriz *aumentada, const Variaveis *vars)
{
    TipoSistema tipo;
    Matriz reduzida;

    tipo = classificarSistema(aumentada);

    imprimirTipoSistema(tipo);

    if(tipo == SISTEMA_SI)
    {
        printf("\nO sistema nao possui solucao.\n");
        return;
    }

    copiarMatriz(aumentada, &reduzida);

    reduzirGaussJordan(&reduzida, vars->quantidade);

    printf("\nMatriz reduzida:\n");
    imprimirMatriz(&reduzida);

    if(tipo == SISTEMA_SPD)
    {
        imprimirSolucaoSPD(&reduzida, vars);
    }
    else
    {
        imprimirSolucaoSPI(&reduzida, vars);
    }
}


/*
==========================================================
Imprime o tipo do sistema.
==========================================================
*/

void imprimirTipoSistema(TipoSistema tipo)
{
    printf("\nClassificacao do sistema:\n");

    if(tipo == SISTEMA_SI)
    {
        printf("SI - Sistema Impossivel\n");
    }
    else if(tipo == SISTEMA_SPI)
    {
        printf("SPI - Sistema Possivel e Indeterminado\n");
    }
    else
    {
        printf("SPD - Sistema Possivel e Determinado\n");
    }
}


/*
==========================================================
Monta a matriz A a partir da matriz aumentada A|b.

Exemplo:
    A|b:
        1 2 5 | 10

    A:
        1 2 5
==========================================================
*/

static void montarMatrizCoeficientes(const Matriz *aumentada, Matriz *coeficientes)
{
    int numeroVariaveis = aumentada->colunas - 1;

    inicializarMatriz(coeficientes,aumentada->linhas,numeroVariaveis);

    for(int i = 0; i < aumentada->linhas; i++)
    {
        for(int j = 0; j < numeroVariaveis; j++)
        {
            coeficientes->dados[i][j] = aumentada->dados[i][j];
        }
    }
}


/*
==========================================================
Reduz a matriz pelo metodo de Gauss-Jordan.

Diferenca para Gauss comum:
    Gauss apenas zera abaixo do pivo.
    Gauss-Jordan zera abaixo e acima do pivo.

Isso facilita imprimir a solucao.
==========================================================
*/

static void reduzirGaussJordan(Matriz *m,int quantidadeVariaveis)
{
    int linhaPivo = 0;

    for(int coluna = 0;
        coluna < quantidadeVariaveis && linhaPivo < m->linhas;
        coluna++)
    {
        int melhorLinha = -1;

        for(int linha = linhaPivo; linha < m->linhas; linha++)
        {
            if(!ehZero(m->dados[linha][coluna]))
            {
                melhorLinha = linha;
                break;
            }
        }

        if(melhorLinha == -1)
        {
            continue;
        }

        if(melhorLinha != linhaPivo)
        {
            trocarLinhas(m, melhorLinha, linhaPivo);
        }

        double pivo = m->dados[linhaPivo][coluna];

        if(!ehZero(pivo))
        {
            multiplicarLinha(m, linhaPivo, 1.0 / pivo);
        }

        for(int linha = 0; linha < m->linhas; linha++)
        {
            if(linha != linhaPivo)
            {
                double fator = -m->dados[linha][coluna];

                if(!ehZero(fator))
                {
                    adicionarMultiplo(m, linha, linhaPivo, fator);
                }
            }
        }

        linhaPivo++;
    }

    zerarValoresPequenos(m);
}


/*
==========================================================
Encontra a primeira coluna nao nula de uma linha.
==========================================================
*/

static int primeiraColunaNaoZero(const Matriz *m,int linha,int quantidadeVariaveis)
{
    for(int coluna = 0; coluna < quantidadeVariaveis; coluna++)
    {
        if(!ehZero(m->dados[linha][coluna]))
        {
            return coluna;
        }
    }

    return -1;
}


/*
==========================================================
Identifica quais colunas possuem pivo.

Isso e importante para SPI:
    variaveis com pivo  -> dependentes
    variaveis sem pivo  -> livres
==========================================================
*/

static void identificarPivos(const Matriz *m, int quantidadeVariaveis,int pivoPorLinha[MAX_LINHAS], int colunaEhPivo[MAX_VARIAVEIS])
{
    for(int i = 0; i < MAX_LINHAS; i++)
    {
        pivoPorLinha[i] = -1;
    }

    for(int i = 0; i < MAX_VARIAVEIS; i++)
    {
        colunaEhPivo[i] = 0;
    }

    for(int linha = 0; linha < m->linhas; linha++)
    {
        int colunaPivo = primeiraColunaNaoZero(m,
                                               linha,
                                               quantidadeVariaveis);

        if(colunaPivo != -1)
        {
            pivoPorLinha[linha] = colunaPivo;
            colunaEhPivo[colunaPivo] = 1;
        }
    }
}


/*
==========================================================
Imprime solucao unica do sistema SPD.
==========================================================
*/

static void imprimirSolucaoSPD(const Matriz *reduzida,
                               const Variaveis *vars)
{
    int quantidadeVariaveis = vars->quantidade;
    int pivoPorLinha[MAX_LINHAS];
    int colunaEhPivo[MAX_VARIAVEIS];

    identificarPivos(reduzida,quantidadeVariaveis,pivoPorLinha,colunaEhPivo);

    printf("\nSolucao unica:\n");

    for(int coluna = 0; coluna < quantidadeVariaveis; coluna++)
    {
        int linhaEncontrada = -1;

        for(int linha = 0; linha < reduzida->linhas; linha++)
        {
            if(pivoPorLinha[linha] == coluna)
            {
                linhaEncontrada = linha;
                break;
            }
        }

        if(linhaEncontrada != -1)
        {
            printf("%c = %.4lf\n",vars->lista[coluna].nome,reduzida->dados[linhaEncontrada][quantidadeVariaveis]);
        }
    }
}


/*
==========================================================
Imprime solucao geral do sistema SPI.

Exemplo:
    x + y = 4

Pode virar:
    y = t1
    x = 4 - 1*t1
==========================================================
*/
static void imprimirSolucaoSPI(const Matriz *reduzida,
                               const Variaveis *vars)
{
    int quantidadeVariaveis = vars->quantidade;

    int pivoPorLinha[MAX_LINHAS];
    int colunaEhPivo[MAX_VARIAVEIS];

    identificarPivos(reduzida,quantidadeVariaveis,pivoPorLinha,colunaEhPivo);

    printf("\nSolucao geral:\n");

    printf("\nVariaveis livres:\n");

    for(int coluna = 0; coluna < quantidadeVariaveis; coluna++)
    {
        if(!colunaEhPivo[coluna])
        {
            printf("%c pode assumir qualquer valor real.\n",
                   vars->lista[coluna].nome);
        }
    }

    printf("\nVariaveis dependentes:\n");

    for(int linha = 0; linha < reduzida->linhas; linha++)
    {
        int colunaPivo = pivoPorLinha[linha];

        if(colunaPivo != -1)
        {
            double resultado = reduzida->dados[linha][quantidadeVariaveis];

            printf("%c = %.4lf",
                   vars->lista[colunaPivo].nome,
                   resultado);

            for(int coluna = 0; coluna < quantidadeVariaveis; coluna++)
            {
                if(!colunaEhPivo[coluna])
                {
                    double coeficiente = -reduzida->dados[linha][coluna];

                    if(!ehZero(coeficiente))
                    {
                        if(coeficiente > 0)
                        {
                            printf(" + ");
                        }
                        else
                        {
                            printf(" - ");
                        }

                        if(ehZero(absoluto(coeficiente) - 1.0))
                        {
                            printf("%c",
                                   vars->lista[coluna].nome);
                        }
                        else
                        {
                            printf("%.4lf%c",
                                   absoluto(coeficiente),
                                   vars->lista[coluna].nome);
                        }
                    }
                }
            }

            printf("\n");
        }
    }
}

/*
==========================================================
Troca valores muito pequenos por zero.

Isso deixa a matriz impressa mais limpa.
==========================================================
*/

static void zerarValoresPequenos(Matriz *m)
{
    for(int i = 0; i < m->linhas; i++)
    {
        for(int j = 0; j < m->colunas; j++)
        {
            if(ehZero(m->dados[i][j]))
            {
                m->dados[i][j] = 0.0;
            }
        }
    }
}
