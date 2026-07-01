#include <stdio.h>
#include <string.h>

#include "base.h"
#include "sistemas.h"
#include "config.h"

static void removerQuebraLinha(char texto[]);
static void removerEspacos(char texto[]);

static int caractereDigito(char c);
static int caractereSinal(char c);

static int lerNumeroComSinal(const char texto[], int *indice, double *numero);

static int extrairVetores(const char texto[], double vetores[MAX_VARIAVEIS][MAX_VARIAVEIS], int *quantidadeVetores, int *dimensao);

static void montarMatrizVetores(const double vetores[MAX_VARIAVEIS][MAX_VARIAVEIS], int quantidadeVetores, int dimensao, Matriz *matriz);

static void analisarConjunto(const Matriz *matrizVetores, const double vetores[MAX_VARIAVEIS][MAX_VARIAVEIS], int quantidadeVetores, int dimensao);

static int subconjuntoFormaBase(const Matriz *matrizVetores, int dimensao, int selecionados[MAX_VARIAVEIS]);

static void montarMatrizComSelecionados(const Matriz *matrizOriginal, const int selecionados[MAX_VARIAVEIS], int quantidadeSelecionados, Matriz *resultado);

static void encontrarSubconjuntoIndependente(const Matriz *matrizVetores, int selecionados[MAX_VARIAVEIS], int *quantidadeSelecionados);

static void completarParaBase(const Matriz *matrizVetores, const double vetores[MAX_VARIAVEIS][MAX_VARIAVEIS], int dimensao);

static void imprimirVetor(const double vetor[], int dimensao);

static void pausarBase(void);


void executarBases(void)
{
    char texto[MAX_TAMANHO];

    double vetores[MAX_VARIAVEIS][MAX_VARIAVEIS];

    int quantidadeVetores = 0;
    int dimensao = 0;

    Matriz matrizVetores;

    printf("\nQUESTAO 3 - BASES EM R2 E R3\n");

    printf("\nDigite o conjunto de vetores.\n");

    printf("\nFormatos aceitos:\n");
    printf("  {(2,1),(1,3)}\n");
    printf("  V1={(2,1),(1,3)}\n");
    printf("  V2={(1,1),(2,2),(1,0)}\n");
    printf("  V3={(1,0,2),(2,1,3)}\n");

    printf("\nConjunto: ");

    if(fgets(texto, MAX_TAMANHO, stdin) == NULL)
    {
        printf("\nErro de leitura.\n");
        pausarBase();
        return;
    }

    removerQuebraLinha(texto);
    removerEspacos(texto);

    if(strlen(texto) == 0)
    {
        printf("\nNenhum conjunto foi digitado.\n");
        pausarBase();
        return;
    }

    if(!extrairVetores(texto, vetores, &quantidadeVetores, &dimensao))
    {
        printf("\nErro: conjunto invalido.\n");
        printf("Use o formato {(2,1),(1,3)} ou V1={(2,1),(1,3)}.\n");
        pausarBase();
        return;
    }

    montarMatrizVetores(vetores, quantidadeVetores, dimensao, &matrizVetores);

    analisarConjunto(&matrizVetores, vetores, quantidadeVetores, dimensao);

    pausarBase();
}

/* Verifica se o conjunto forma base */

int conjuntoFormaBase(const Matriz *matrizVetores)
{
    int dimensao = matrizVetores->linhas;
    int quantidadeVetores = matrizVetores->colunas;
    int posto = calcularPosto(matrizVetores);

    return quantidadeVetores == dimensao && posto == dimensao;
}

/* Remove o \n do fgets */

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

/* Remove espacos e tabulacoes */

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

static int caractereDigito(char c)
{
    return c >= '0' && c <= '9';
}

static int caractereSinal(char c)
{
    return c == '+' || c == '-';
}

/* Le numero positivo ou negativo */

static int lerNumeroComSinal(const char texto[], int *indice, double *numero)
{
    int i = *indice;
    int sinal = 1;

    double valor = 0.0;
    double casaDecimal = 0.1;

    int possuiDigito = 0;

    if(caractereSinal(texto[i]))
    {
        if(texto[i] == '-')
        {
            sinal = -1;
        }

        i++;
    }

    while(caractereDigito(texto[i]))
    {
        valor = valor * 10.0 + (texto[i] - '0');
        i++;
        possuiDigito = 1;
    }

    if(texto[i] == '.')
    {
        i++;

        while(caractereDigito(texto[i]))
        {
            valor += (texto[i] - '0') * casaDecimal;
            casaDecimal /= 10.0;
            i++;
            possuiDigito = 1;
        }
    }

    if(!possuiDigito)
    {
        return 0;
    }

    *numero = sinal * valor;
    *indice = i;

    return 1;
}

/* Extrai os vetores digitados */

static int extrairVetores(const char texto[], double vetores[MAX_VARIAVEIS][MAX_VARIAVEIS], int *quantidadeVetores, int *dimensao)
{
    int i;
    int inicio = -1;
    int usaChaves = 0;

    *quantidadeVetores = 0;
    *dimensao = 0;

    for(int a = 0; a < MAX_VARIAVEIS; a++)
    {
        for(int b = 0; b < MAX_VARIAVEIS; b++)
        {
            vetores[a][b] = 0.0;
        }
    }

    for(int k = 0; texto[k] != '\0'; k++)
    {
        if(texto[k] == '{')
        {
            inicio = k + 1;
            usaChaves = 1;
            break;
        }
    }

    if(inicio == -1)
    {
        for(int k = 0; texto[k] != '\0'; k++)
        {
            if(texto[k] == '(')
            {
                inicio = k;
                usaChaves = 0;
                break;
            }
        }
    }

    if(inicio == -1)
    {
        return 0;
    }

    i = inicio;

    while(texto[i] != '\0')
    {
        int coordenada = 0;

        if(usaChaves && texto[i] == '}')
        {
            i++;
            break;
        }

        if(*quantidadeVetores >= MAX_VARIAVEIS)
        {
            return 0;
        }

        if(texto[i] != '(')
        {
            return 0;
        }

        i++;

        while(texto[i] != '\0' && texto[i] != ')')
        {
            double numero;

            if(coordenada >= MAX_VARIAVEIS)
            {
                return 0;
            }

            if(!lerNumeroComSinal(texto, &i, &numero))
            {
                return 0;
            }

            vetores[*quantidadeVetores][coordenada] = numero;
            coordenada++;

            if(texto[i] == ',')
            {
                i++;

                if(texto[i] == ')' || texto[i] == '\0')
                {
                    return 0;
                }
            }
            else if(texto[i] != ')')
            {
                return 0;
            }
        }

        if(texto[i] != ')')
        {
            return 0;
        }

        i++;

        if(coordenada < 2 || coordenada > 3)
        {
            return 0;
        }

        if(*dimensao == 0)
        {
            *dimensao = coordenada;
        }
        else if(coordenada != *dimensao)
        {
            return 0;
        }

        (*quantidadeVetores)++;

        if(texto[i] == ',')
        {
            i++;

            if(texto[i] == '\0')
            {
                return 0;
            }
        }
        else if(usaChaves && texto[i] == '}')
        {
            i++;
            break;
        }
        else if(!usaChaves && texto[i] == '\0')
        {
            break;
        }
        else
        {
            return 0;
        }
    }

    while(texto[i] != '\0')
    {
        if(texto[i] != ';' && texto[i] != '.')
        {
            return 0;
        }

        i++;
    }

    return *quantidadeVetores > 0 && (*dimensao == 2 || *dimensao == 3);
}

/* Monta a matriz colocando os vetores em colunas */

static void montarMatrizVetores(const double vetores[MAX_VARIAVEIS][MAX_VARIAVEIS], int quantidadeVetores, int dimensao, Matriz *matriz)
{
    inicializarMatriz(matriz, dimensao, quantidadeVetores);

    for(int coluna = 0; coluna < quantidadeVetores; coluna++)
    {
        for(int linha = 0; linha < dimensao; linha++)
        {
            matriz->dados[linha][coluna] = vetores[coluna][linha];
        }
    }
}

/* Analisa se o conjunto forma base */

static void analisarConjunto(const Matriz *matrizVetores, const double vetores[MAX_VARIAVEIS][MAX_VARIAVEIS], int quantidadeVetores, int dimensao)
{
    int posto = calcularPosto(matrizVetores);

    printf("\nResultado:\n");
    printf("Espaco identificado: R%d\n", dimensao);

    if(conjuntoFormaBase(matrizVetores))
    {
        printf("O conjunto forma uma base de R%d.\n", dimensao);
        return;
    }

    printf("O conjunto NAO forma uma base de R%d.\n", dimensao);

    printf("\nMotivo:\n");

    if(quantidadeVetores < dimensao)
    {
        printf("Ha menos vetores do que a dimensao do espaco.\n");
    }
    else if(quantidadeVetores > dimensao)
    {
        printf("Ha mais vetores do que a dimensao do espaco.\n");
    }

    if(posto < dimensao)
    {
        printf("O conjunto nao gera todo o espaco R%d.\n", dimensao);
    }

    if(posto < quantidadeVetores)
    {
        printf("Ha dependencia linear entre os vetores.\n");
    }

    if(posto == dimensao && quantidadeVetores > dimensao)
    {
        int selecionados[MAX_VARIAVEIS];

        if(subconjuntoFormaBase(matrizVetores, dimensao, selecionados))
        {
            printf("\nUm subconjunto que forma base e:\n");

            for(int i = 0; i < dimensao; i++)
            {
                int indice = selecionados[i];

                printf("v%d = ", indice + 1);
                imprimirVetor(vetores[indice], dimensao);
                printf("\n");
            }
        }
    }
    else
    {
        completarParaBase(matrizVetores, vetores, dimensao);
    }
}

/* Procura um subconjunto que forme base */

static int subconjuntoFormaBase(const Matriz *matrizVetores, int dimensao, int selecionados[MAX_VARIAVEIS])
{
    int quantidadeVetores = matrizVetores->colunas;

    if(dimensao == 2)
    {
        for(int a = 0; a < quantidadeVetores; a++)
        {
            for(int b = a + 1; b < quantidadeVetores; b++)
            {
                Matriz teste;
                int temp[MAX_VARIAVEIS];

                temp[0] = a;
                temp[1] = b;

                montarMatrizComSelecionados(matrizVetores, temp, 2, &teste);

                if(calcularPosto(&teste) == 2)
                {
                    selecionados[0] = a;
                    selecionados[1] = b;
                    return 1;
                }
            }
        }
    }

    if(dimensao == 3 && quantidadeVetores >= 3)
    {
        Matriz teste;
        int temp[MAX_VARIAVEIS];

        temp[0] = 0;
        temp[1] = 1;
        temp[2] = 2;

        montarMatrizComSelecionados(matrizVetores, temp, 3, &teste);

        if(calcularPosto(&teste) == 3)
        {
            selecionados[0] = 0;
            selecionados[1] = 1;
            selecionados[2] = 2;
            return 1;
        }
    }

    return 0;
}

/* Monta matriz com colunas selecionadas */

static void montarMatrizComSelecionados(const Matriz *matrizOriginal, const int selecionados[MAX_VARIAVEIS], int quantidadeSelecionados, Matriz *resultado)
{
    inicializarMatriz(resultado, matrizOriginal->linhas, quantidadeSelecionados);

    for(int colunaNova = 0; colunaNova < quantidadeSelecionados; colunaNova++)
    {
        int colunaOriginal = selecionados[colunaNova];

        for(int linha = 0; linha < matrizOriginal->linhas; linha++)
        {
            resultado->dados[linha][colunaNova] = matrizOriginal->dados[linha][colunaOriginal];
        }
    }
}

/* Encontra um subconjunto independente */

static void encontrarSubconjuntoIndependente(const Matriz *matrizVetores, int selecionados[MAX_VARIAVEIS], int *quantidadeSelecionados)
{
    Matriz teste;
    int temp[MAX_VARIAVEIS];

    int postoAtual = 0;

    *quantidadeSelecionados = 0;

    for(int i = 0; i < MAX_VARIAVEIS; i++)
    {
        selecionados[i] = -1;
        temp[i] = -1;
    }

    for(int coluna = 0; coluna < matrizVetores->colunas; coluna++)
    {
        temp[*quantidadeSelecionados] = coluna;

        montarMatrizComSelecionados(matrizVetores, temp, *quantidadeSelecionados + 1, &teste);

        int novoPosto = calcularPosto(&teste);

        if(novoPosto > postoAtual)
        {
            selecionados[*quantidadeSelecionados] = coluna;
            (*quantidadeSelecionados)++;
            postoAtual = novoPosto;
        }

        if(postoAtual == matrizVetores->linhas)
        {
            return;
        }
    }
}

/* Completa para base usando vetores canonicos */

static void completarParaBase(const Matriz *matrizVetores, const double vetores[MAX_VARIAVEIS][MAX_VARIAVEIS], int dimensao)
{
    int selecionados[MAX_VARIAVEIS];
    int quantidadeSelecionados;

    double baseCompleta[MAX_VARIAVEIS][MAX_VARIAVEIS];
    int quantidadeBase = 0;

    encontrarSubconjuntoIndependente(matrizVetores, selecionados, &quantidadeSelecionados);

    for(int i = 0; i < quantidadeSelecionados; i++)
    {
        int indice = selecionados[i];

        for(int j = 0; j < dimensao; j++)
        {
            baseCompleta[quantidadeBase][j] = vetores[indice][j];
        }

        quantidadeBase++;
    }

    for(int candidato = 0; candidato < dimensao; candidato++)
    {
        Matriz teste;

        if(quantidadeBase == dimensao)
        {
            break;
        }

        for(int j = 0; j < dimensao; j++)
        {
            if(j == candidato)
            {
                baseCompleta[quantidadeBase][j] = 1.0;
            }
            else
            {
                baseCompleta[quantidadeBase][j] = 0.0;
            }
        }

        montarMatrizVetores(baseCompleta, quantidadeBase + 1, dimensao, &teste);

        if(calcularPosto(&teste) == quantidadeBase + 1)
        {
            quantidadeBase++;
        }
    }

    if(quantidadeBase == dimensao)
    {
        printf("\nUma forma de completar para uma base de R%d e:\n", dimensao);

        for(int i = 0; i < quantidadeBase; i++)
        {
            printf("v%d = ", i + 1);
            imprimirVetor(baseCompleta[i], dimensao);
            printf("\n");
        }
    }
    else
    {
        printf("\nNao foi possivel completar automaticamente para uma base.\n");
    }
}

/* Imprime um vetor */

static void imprimirVetor(const double vetor[], int dimensao)
{
    printf("(");

    for(int i = 0; i < dimensao; i++)
    {
        printf("%.4lf", vetor[i]);

        if(i < dimensao - 1)
        {
            printf(", ");
        }
    }

    printf(")");
}

/* Pausa a tela */

static void pausarBase(void)
{
    char linha[20];

    printf("\nPressione ENTER para continuar...");
    fgets(linha, sizeof(linha), stdin);
}