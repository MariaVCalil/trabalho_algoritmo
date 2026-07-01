#include <stdio.h>
#include "sistemas.h"

/* Funcoes privadas */

static void montarMatrizCoeficientes(const Matriz *aumentada, Matriz *coeficientes);
static int buscarLinhaPivo(const Matriz *m, int linhaInicial, int coluna);
static void reduzirGaussJordan(Matriz *m, int quantidadeVariaveis);
static int primeiraColunaNaoZero(const Matriz *m, int linha, int quantidadeVariaveis);
static void identificarPivos(const Matriz *m, int quantidadeVariaveis, int pivoPorLinha[MAX_LINHAS], int colunaEhPivo[MAX_VARIAVEIS]);
static void imprimirSolucaoSPD(const Matriz *reduzida, const Variaveis *vars);
static void imprimirSolucaoSPI(const Matriz *reduzida, const Variaveis *vars);
static void zerarValoresPequenos(Matriz *m);

/* Escalona uma matriz para calcular o posto */

void escalonarMatriz(Matriz *m)
{
    int linhaPivo = 0;

    for(int coluna = 0; coluna < m->colunas && linhaPivo < m->linhas; coluna++)
    {
        int melhorLinha = buscarLinhaPivo(m, linhaPivo, coluna);

        if(melhorLinha == -1)
        {
            continue;
        }

        if(melhorLinha != linhaPivo)
        {
            trocarLinhas(m, melhorLinha, linhaPivo);
        }

        double pivo = m->dados[linhaPivo][coluna];

        multiplicarLinha(m, linhaPivo, 1.0 / pivo);

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

/* Calcula o posto pela quantidade de linhas nao nulas */

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

/* Classifica o sistema usando posto(A) e posto(A|b) */

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

/* Resolve o sistema e imprime o resultado */

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

    if(tipo == SISTEMA_SPD)
    {
        imprimirSolucaoSPD(&reduzida, vars);
    }
    else
    {
        imprimirSolucaoSPI(&reduzida, vars);
    }
}

/* Imprime a classificacao do sistema */

void imprimirTipoSistema(TipoSistema tipo)
{
    printf("\nClassificacao do sistema:\n");

    switch(tipo)
    {
        case SISTEMA_SI:
            printf("SI - Sistema Impossivel\n");
            break;

        case SISTEMA_SPI:
            printf("SPI - Sistema Possivel e Indeterminado\n");
            break;

        case SISTEMA_SPD:
            printf("SPD - Sistema Possivel e Determinado\n");
            break;
    }
}

/* Monta a matriz de coeficientes A a partir da matriz aumentada A|b */

static void montarMatrizCoeficientes(const Matriz *aumentada, Matriz *coeficientes)
{
    int numeroVariaveis = aumentada->colunas - 1;

    inicializarMatriz(coeficientes, aumentada->linhas, numeroVariaveis);

    for(int i = 0; i < aumentada->linhas; i++)
    {
        for(int j = 0; j < numeroVariaveis; j++)
        {
            coeficientes->dados[i][j] = aumentada->dados[i][j];
        }
    }
}

/* Procura uma linha que tenha valor nao nulo na coluna escolhida */

static int buscarLinhaPivo(const Matriz *m, int linhaInicial, int coluna)
{
    for(int linha = linhaInicial; linha < m->linhas; linha++)
    {
        if(!ehZero(m->dados[linha][coluna]))
        {
            return linha;
        }
    }

    return -1;
}

/* Reduz a matriz por Gauss-Jordan para encontrar a solucao */

static void reduzirGaussJordan(Matriz *m, int quantidadeVariaveis)
{
    int linhaPivo = 0;

    for(int coluna = 0; coluna < quantidadeVariaveis && linhaPivo < m->linhas; coluna++)
    {
        int melhorLinha = buscarLinhaPivo(m, linhaPivo, coluna);

        if(melhorLinha == -1)
        {
            continue;
        }

        if(melhorLinha != linhaPivo)
        {
            trocarLinhas(m, melhorLinha, linhaPivo);
        }

        double pivo = m->dados[linhaPivo][coluna];

        multiplicarLinha(m, linhaPivo, 1.0 / pivo);

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

/* Retorna a primeira coluna nao nula de uma linha */

static int primeiraColunaNaoZero(const Matriz *m, int linha, int quantidadeVariaveis)
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

/* Identifica variaveis dependentes e livres */

static void identificarPivos(const Matriz *m, int quantidadeVariaveis, int pivoPorLinha[MAX_LINHAS], int colunaEhPivo[MAX_VARIAVEIS])
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
        int colunaPivo = primeiraColunaNaoZero(m, linha, quantidadeVariaveis);

        if(colunaPivo != -1)
        {
            pivoPorLinha[linha] = colunaPivo;
            colunaEhPivo[colunaPivo] = 1;
        }
    }
}

/* Imprime a solucao unica do sistema SPD */

static void imprimirSolucaoSPD(const Matriz *reduzida, const Variaveis *vars)
{
    int quantidadeVariaveis = vars->quantidade;

    int pivoPorLinha[MAX_LINHAS];
    int colunaEhPivo[MAX_VARIAVEIS];

    identificarPivos(reduzida, quantidadeVariaveis, pivoPorLinha, colunaEhPivo);

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
            printf("%c = %.4lf\n",
                   vars->lista[coluna].nome,
                   reduzida->dados[linhaEncontrada][quantidadeVariaveis]);
        }
    }
}

/* Imprime a solucao geral do sistema SPI */

static void imprimirSolucaoSPI(const Matriz *reduzida, const Variaveis *vars)
{
    int quantidadeVariaveis = vars->quantidade;

    int pivoPorLinha[MAX_LINHAS];
    int colunaEhPivo[MAX_VARIAVEIS];

    identificarPivos(reduzida, quantidadeVariaveis, pivoPorLinha, colunaEhPivo);

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
                            printf("%c", vars->lista[coluna].nome);
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

/* Troca valores muito pequenos por zero */

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