#include <stdio.h>
#include <string.h>

#include "autovalores.h"
#include "config.h"

/* Funcoes privadas */

static void removerQuebraLinha(char texto[]);
static void removerEspacos(char texto[]);

static int caractereLetra(char c);
static int caractereDigito(char c);
static int caractereSinal(char c);

static char paraMinusculo(char c);

static double raizQuadrada(double numero);

static int lerNumeroBasico(const char texto[], int *indice, double *numero);
static int lerCoeficiente(const char texto[], int *indice, double *numero);

static int indiceVariavel(const char variaveis[], int quantidade, char letra);

static int extrairVariaveisDominio(const char texto[], char variaveis[], int *quantidadeVariaveis);
static int extrairExpressoesResultado(const char texto[], char expressoes[MAX_LINHAS][MAX_TAMANHO], int *quantidadeExpressoes);

static int interpretarExpressao(const char expressao[], const char variaveis[], int quantidadeVariaveis, double coeficientes[MAX_VARIAVEIS]);

static void normalizarAutovetor(double vetor[2]);
static void pausarAutovalores(void);

/* Executa a questao 4 */

void executarAutovalores(void)
{
    char texto[MAX_TAMANHO];
    char variaveis[2];

    Matriz matriz;

    double autovalores[2];
    int multiplicidades[2];
    int quantidadeAutovalores = 0;

    printf("\nQUESTAO 4 - AUTOVALORES E AUTOVETORES\n");

    printf("\nDigite o operador no formato:\n");
    printf("  T(x,y)=(3x-5/2y,1/2y)\n");
    printf("  T(x,y)=(3x+y,3y)\n");
    printf("  T(x,y)=(-y,x)\n");
    printf("  T(x,y)=(2x+2y,-x+4y)\n");

    printf("\nOperador: ");

    if(fgets(texto, MAX_TAMANHO, stdin) == NULL)
    {
        printf("\nErro de leitura.\n");
        pausarAutovalores();
        return;
    }

    removerQuebraLinha(texto);
    removerEspacos(texto);

    if(strlen(texto) == 0)
    {
        printf("\nNenhum operador foi digitado.\n");
        pausarAutovalores();
        return;
    }

    if(!montarMatrizOperadorR2(texto, &matriz, variaveis))
    {
        printf("\nErro: operador invalido.\n");
        printf("Use um operador linear de R2 para R2 no formato T(x,y)=(..., ...).\n");
        pausarAutovalores();
        return;
    }

    if(!calcularAutovalores2x2(&matriz, autovalores, multiplicidades, &quantidadeAutovalores))
    {
        printf("\nResultado:\n");
        printf("Nao existem autovalores reais.\n");
        pausarAutovalores();
        return;
    }

    printf("\nResultado:\n");

    for(int i = 0; i < quantidadeAutovalores; i++)
    {
        double vetor[2];

        calcularAutovetor2x2(&matriz, autovalores[i], vetor);

        printf("\nAutovalor %d: lambda = %.4lf", i + 1, autovalores[i]);

        if(multiplicidades[i] == 2)
        {
            printf("  multiplicidade algebrica 2");
        }

        printf("\n");

        if(ehZero(vetor[0]) && ehZero(vetor[1]))
        {
            printf("Autoespaco: qualquer vetor nao nulo de R2.\n");
            printf("Uma base para o autoespaco: {(1,0), (0,1)}\n");
        }
        else
        {
            normalizarAutovetor(vetor);
            printf("Autovetor associado: (%.4lf, %.4lf)\n", vetor[0], vetor[1]);
        }
    }

    pausarAutovalores();
}

/* Calcula os autovalores reais de uma matriz 2x2 */

int calcularAutovalores2x2(const Matriz *matriz, double autovalores[2], int multiplicidades[2], int *quantidade)
{
    double a = matriz->dados[0][0];
    double b = matriz->dados[0][1];
    double c = matriz->dados[1][0];
    double d = matriz->dados[1][1];

    double traco = a + d;
    double determinante = a * d - b * c;
    double delta = traco * traco - 4.0 * determinante;

    *quantidade = 0;

    if(delta < -EPSILON)
    {
        return 0;
    }

    if(ehZero(delta))
    {
        autovalores[0] = traco / 2.0;
        multiplicidades[0] = 2;
        *quantidade = 1;
        return 1;
    }

    double raiz = raizQuadrada(delta);

    autovalores[0] = (traco + raiz) / 2.0;
    autovalores[1] = (traco - raiz) / 2.0;

    multiplicidades[0] = 1;
    multiplicidades[1] = 1;

    *quantidade = 2;

    return 1;
}

/* Calcula um autovetor associado a um autovalor */

void calcularAutovetor2x2(const Matriz *matriz, double lambda, double vetor[2])
{
    double p = matriz->dados[0][0] - lambda;
    double q = matriz->dados[0][1];

    double r = matriz->dados[1][0];
    double s = matriz->dados[1][1] - lambda;

    if(!ehZero(p) || !ehZero(q))
    {
        vetor[0] = -q;
        vetor[1] = p;
    }
    else if(!ehZero(r) || !ehZero(s))
    {
        vetor[0] = -s;
        vetor[1] = r;
    }
    else
    {
        vetor[0] = 0.0;
        vetor[1] = 0.0;
    }

    if(ehZero(vetor[0]))
    {
        vetor[0] = 0.0;
    }

    if(ehZero(vetor[1]))
    {
        vetor[1] = 0.0;
    }
}

/* Monta a matriz do operador T: R2 -> R2 */

int montarMatrizOperadorR2(const char texto[], Matriz *matriz, char variaveis[2])
{
    char expressoes[MAX_LINHAS][MAX_TAMANHO];

    int quantidadeVariaveis = 0;
    int quantidadeExpressoes = 0;

    double coeficientes[MAX_VARIAVEIS];

    if(!extrairVariaveisDominio(texto, variaveis, &quantidadeVariaveis))
    {
        return 0;
    }

    if(quantidadeVariaveis != 2)
    {
        return 0;
    }

    if(!extrairExpressoesResultado(texto, expressoes, &quantidadeExpressoes))
    {
        return 0;
    }

    if(quantidadeExpressoes != 2)
    {
        return 0;
    }

    inicializarMatriz(matriz, 2, 2);

    for(int linha = 0; linha < 2; linha++)
    {
        if(!interpretarExpressao(expressoes[linha], variaveis, quantidadeVariaveis, coeficientes))
        {
            return 0;
        }

        for(int coluna = 0; coluna < 2; coluna++)
        {
            matriz->dados[linha][coluna] = coeficientes[coluna];
        }
    }

    return 1;
}

/* Normaliza o autovetor para deixar a saida mais simples */

static void normalizarAutovetor(double vetor[2])
{
    double divisor;

    if(!ehZero(vetor[0]))
    {
        divisor = vetor[0];
    }
    else if(!ehZero(vetor[1]))
    {
        divisor = vetor[1];
    }
    else
    {
        return;
    }

    vetor[0] = vetor[0] / divisor;
    vetor[1] = vetor[1] / divisor;

    if(vetor[0] < 0 || (ehZero(vetor[0]) && vetor[1] < 0))
    {
        vetor[0] = -vetor[0];
        vetor[1] = -vetor[1];
    }

    if(ehZero(vetor[0]))
    {
        vetor[0] = 0.0;
    }

    if(ehZero(vetor[1]))
    {
        vetor[1] = 0.0;
    }
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

/* Raiz quadrada pelo metodo de Newton, sem usar math.h */

static double raizQuadrada(double numero)
{
    if(numero < 0.0)
    {
        return 0.0;
    }

    if(ehZero(numero))
    {
        return 0.0;
    }

    double x = numero;

    if(numero < 1.0)
    {
        x = 1.0;
    }

    for(int i = 0; i < 30; i++)
    {
        x = 0.5 * (x + numero / x);
    }

    return x;
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

/* Procura uma variavel na lista */

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

    *quantidadeVariaveis = 0;

    for(int i = 0; texto[i] != '\0'; i++)
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

        if(*quantidadeVariaveis >= 2)
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

    return *quantidadeVariaveis == 2;
}

/* Extrai as duas expressoes do resultado em T(...)=(...) */

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

            if(*quantidadeExpressoes >= 2)
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

    return *quantidadeExpressoes == 2;
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

/* Pausa a tela */

static void pausarAutovalores(void)
{
    char linha[20];

    printf("\nPressione ENTER para continuar...");
    fgets(linha, sizeof(linha), stdin);
}