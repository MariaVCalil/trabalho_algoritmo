#include <stdio.h>
#include <string.h>

#include "transf_l.h"
#include "sistemas.h"
#include "config.h"

/* Funcoes privadas */

static void removerQuebraLinha(char texto[]);
static void removerEspacos(char texto[]);

static int caractereLetra(char c);
static int caractereDigito(char c);
static int caractereSinal(char c);

static char paraMinusculo(char c);

static int lerNumeroBasico(const char texto[], int *indice, double *numero);
static int lerCoeficiente(const char texto[], int *indice, double *numero);

static int indiceVariavel(const char variaveis[], int quantidade, char letra);

static int extrairVariaveisDominio(const char texto[], char variaveis[], int *quantidadeVariaveis);
static int extrairExpressoesResultado(const char texto[], char expressoes[MAX_LINHAS][MAX_TAMANHO], int *quantidadeExpressoes);

static int interpretarExpressao(const char expressao[], const char variaveis[], int quantidadeVariaveis, double coeficientes[MAX_VARIAVEIS]);
static int montarMatrizTransformacao(const char texto[], Matriz *matriz);

static void pausarTransformacao(void);

/* Executa a questao 2 */

void executarTransformacaoLinear(void)
{
    char texto[MAX_TAMANHO];
    Matriz matriz;

    printf("\nQUESTAO 2 - TRANSFORMACOES LINEARES\n");

    printf("\nDigite a transformacao no formato:\n");
    printf("  T(x,y)=(y+2x,3x+5y)\n");
    printf("  T(x,y,z)=(z+x-y,3x+y+2z)\n");
    printf("  T(u,v)=(2u-v,v+u,3u)\n");

    printf("\nTransformacao: ");

    if(fgets(texto, MAX_TAMANHO, stdin) == NULL)
    {
        printf("\nErro de leitura.\n");
        pausarTransformacao();
        return;
    }

    removerQuebraLinha(texto);
    removerEspacos(texto);

    if(strlen(texto) == 0)
    {
        printf("\nNenhuma transformacao foi digitada.\n");
        pausarTransformacao();
        return;
    }

    if(!montarMatrizTransformacao(texto, &matriz))
    {
        printf("\nErro: transformacao invalida.\n");
        printf("Use o formato T(x,y)=(..., ...) ou T(x,y,z)=(..., ..., ...).\n");
        pausarTransformacao();
        return;
    }

    printf("\nResultado:\n");
    printf("Dimensao do nucleo: %d\n", dimensaoNucleo(&matriz));
    printf("Dimensao da imagem: %d\n", dimensaoImagem(&matriz));
    printf("Injetora: %s\n", transformacaoInjetiva(&matriz) ? "sim" : "nao");
    printf("Sobrejetora: %s\n", transformacaoSobrejetiva(&matriz) ? "sim" : "nao");
    printf("Bijetora: %s\n", transformacaoBijetiva(&matriz) ? "sim" : "nao");

    pausarTransformacao();
}

/* Dimensao da imagem = posto da matriz */

int dimensaoImagem(const Matriz *matriz)
{
    return calcularPosto(matriz);
}

/* Dimensao do nucleo = dimensao do dominio - posto */

int dimensaoNucleo(const Matriz *matriz)
{
    return matriz->colunas - calcularPosto(matriz);
}

/* Injetora quando o nucleo tem dimensao zero */

int transformacaoInjetiva(const Matriz *matriz)
{
    return dimensaoNucleo(matriz) == 0;
}

/* Sobrejetora quando a imagem tem a dimensao do contradominio */

int transformacaoSobrejetiva(const Matriz *matriz)
{
    return dimensaoImagem(matriz) == matriz->linhas;
}

/* Bijetora quando e injetora e sobrejetora */

int transformacaoBijetiva(const Matriz *matriz)
{
    return transformacaoInjetiva(matriz) && transformacaoSobrejetiva(matriz);
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

static int caractereLetra(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static int caractereDigito(char c)
{
    return c >= '0' && c <= '9';
}

static int caractereSinal(char c)
{
    return c == '+' || c == '-';
}

static char paraMinusculo(char c)
{
    if(c >= 'A' && c <= 'Z')
    {
        return c + 32;
    }

    return c;
}

/* Le numero inteiro ou decimal sem sinal */

static int lerNumeroBasico(const char texto[], int *indice, double *numero)
{
    double valor = 0.0;
    double casaDecimal = 0.1;

    int i = *indice;
    int possuiDigito = 0;

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

    *numero = valor;
    *indice = i;

    return 1;
}

/* Le coeficiente inteiro, decimal ou fracionario */

static int lerCoeficiente(const char texto[], int *indice, double *numero)
{
    double numerador;
    double denominador;

    if(!lerNumeroBasico(texto, indice, &numerador))
    {
        return 0;
    }

    if(texto[*indice] == '/')
    {
        (*indice)++;

        if(!lerNumeroBasico(texto, indice, &denominador))
        {
            return 0;
        }

        if(ehZero(denominador))
        {
            return 0;
        }

        *numero = numerador / denominador;
    }
    else
    {
        *numero = numerador;
    }

    return 1;
}

/* Procura uma variavel na lista do dominio */

static int indiceVariavel(const char variaveis[], int quantidade, char letra)
{
    letra = paraMinusculo(letra);

    for(int i = 0; i < quantidade; i++)
    {
        if(variaveis[i] == letra)
        {
            return i;
        }
    }

    return -1;
}

/* Extrai as variaveis do dominio em T(x,y) */

static int extrairVariaveisDominio(const char texto[], char variaveis[], int *quantidadeVariaveis)
{
    int abre = -1;
    int fecha = -1;
    int igual = -1;

    *quantidadeVariaveis = 0;

    for(int i = 0; texto[i] != '\0'; i++)
    {
        if(texto[i] == '=')
        {
            igual = i;
            break;
        }
    }

    if(igual == -1)
    {
        return 0;
    }

    for(int i = 0; i < igual; i++)
    {
        if(texto[i] == '(')
        {
            abre = i;
            break;
        }
    }

    if(abre == -1)
    {
        return 0;
    }

    for(int i = abre + 1; i < igual; i++)
    {
        if(texto[i] == ')')
        {
            fecha = i;
            break;
        }
    }

    if(fecha == -1 || fecha <= abre + 1)
    {
        return 0;
    }

    int i = abre + 1;

    while(i < fecha)
    {
        if(!caractereLetra(texto[i]))
        {
            return 0;
        }

        char letra = paraMinusculo(texto[i]);

        if(letra == 'i' || letra == 'j')
        {
            return 0;
        }

        if(indiceVariavel(variaveis, *quantidadeVariaveis, letra) != -1)
        {
            return 0;
        }

        if(*quantidadeVariaveis >= MAX_VARIAVEIS)
        {
            return 0;
        }

        variaveis[*quantidadeVariaveis] = letra;
        (*quantidadeVariaveis)++;

        i++;

        if(i < fecha)
        {
            if(texto[i] != ',')
            {
                return 0;
            }

            i++;

            if(i >= fecha)
            {
                return 0;
            }
        }
    }

    return *quantidadeVariaveis > 0;
}

/* Extrai as expressoes do resultado em T(...)=(...) */

static int extrairExpressoesResultado(const char texto[], char expressoes[MAX_LINHAS][MAX_TAMANHO], int *quantidadeExpressoes)
{
    int igual = -1;
    int abre = -1;
    int fecha = -1;

    *quantidadeExpressoes = 0;

    for(int i = 0; texto[i] != '\0'; i++)
    {
        if(texto[i] == '=')
        {
            igual = i;
            break;
        }
    }

    if(igual == -1)
    {
        return 0;
    }

    for(int i = igual + 1; texto[i] != '\0'; i++)
    {
        if(texto[i] == '(')
        {
            abre = i;
            break;
        }
    }

    if(abre == -1)
    {
        return 0;
    }

    for(int i = abre + 1; texto[i] != '\0'; i++)
    {
        if(texto[i] == ')')
        {
            fecha = i;
            break;
        }
    }

    if(fecha == -1 || fecha <= abre + 1)
    {
        return 0;
    }

    if(texto[fecha + 1] != '\0')
    {
        return 0;
    }

    int posExpressao = 0;

    for(int i = abre + 1; i <= fecha; i++)
    {
        if(texto[i] == ',' || i == fecha)
        {
            if(posExpressao == 0)
            {
                return 0;
            }

            if(*quantidadeExpressoes >= MAX_LINHAS)
            {
                return 0;
            }

            expressoes[*quantidadeExpressoes][posExpressao] = '\0';
            (*quantidadeExpressoes)++;

            posExpressao = 0;
        }
        else
        {
            if(posExpressao >= MAX_TAMANHO - 1)
            {
                return 0;
            }

            expressoes[*quantidadeExpressoes][posExpressao] = texto[i];
            posExpressao++;
        }
    }

    return *quantidadeExpressoes > 0;
}

/* Interpreta uma expressao linear */

static int interpretarExpressao(const char expressao[], const char variaveis[], int quantidadeVariaveis, double coeficientes[MAX_VARIAVEIS])
{
    int i = 0;

    for(int k = 0; k < MAX_VARIAVEIS; k++)
    {
        coeficientes[k] = 0.0;
    }

    if(expressao[0] == '\0')
    {
        return 0;
    }

    while(expressao[i] != '\0')
    {
        int sinal = 1;
        double numero = 1.0;
        int temNumero = 0;

        if(caractereSinal(expressao[i]))
        {
            if(expressao[i] == '-')
            {
                sinal = -1;
            }

            i++;

            if(expressao[i] == '\0')
            {
                return 0;
            }
        }

        if(lerCoeficiente(expressao, &i, &numero))
        {
            temNumero = 1;
        }
        else
        {
            numero = 1.0;
        }

        if(caractereLetra(expressao[i]))
        {
            char letra = paraMinusculo(expressao[i]);
            int coluna;

            if(letra == 'i' || letra == 'j')
            {
                return 0;
            }

            coluna = indiceVariavel(variaveis, quantidadeVariaveis, letra);

            if(coluna == -1)
            {
                return 0;
            }

            coeficientes[coluna] += sinal * numero;
            i++;

            if(expressao[i] != '\0' && !caractereSinal(expressao[i]))
            {
                return 0;
            }
        }
        else if(temNumero)
        {
            if(!ehZero(numero))
            {
                return 0;
            }

            if(expressao[i] != '\0' && !caractereSinal(expressao[i]))
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }

    return 1;
}

/* Monta a matriz associada da transformacao */

static int montarMatrizTransformacao(const char texto[], Matriz *matriz)
{
    char variaveis[MAX_VARIAVEIS];
    char expressoes[MAX_LINHAS][MAX_TAMANHO];

    int quantidadeVariaveis = 0;
    int quantidadeExpressoes = 0;

    double coeficientes[MAX_VARIAVEIS];

    if(!extrairVariaveisDominio(texto, variaveis, &quantidadeVariaveis))
    {
        return 0;
    }

    if(!extrairExpressoesResultado(texto, expressoes, &quantidadeExpressoes))
    {
        return 0;
    }

    inicializarMatriz(matriz, quantidadeExpressoes, quantidadeVariaveis);

    for(int linha = 0; linha < quantidadeExpressoes; linha++)
    {
        if(!interpretarExpressao(expressoes[linha], variaveis, quantidadeVariaveis, coeficientes))
        {
            return 0;
        }

        for(int coluna = 0; coluna < quantidadeVariaveis; coluna++)
        {
            matriz->dados[linha][coluna] = coeficientes[coluna];
        }
    }

    return 1;
}

/* Pausa a tela */

static void pausarTransformacao(void)
{
    char linha[20];

    printf("\nPressione ENTER para continuar...");
    fgets(linha, sizeof(linha), stdin);
}