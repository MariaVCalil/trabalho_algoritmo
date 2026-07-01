#include <stdio.h>
#include <string.h>

#include "transf_l.h"
#include "matriz.h"
#include "sistemas.h"
#include "config.h"

/*
==========================================================
Funcoes privadas
==========================================================
*/

static void removerQuebraLinha(char texto[]);
static void removerEspacos(char texto[]);

static int ehLetra(char c);
static int ehDigito(char c);
static int ehSinal(char c);

static char paraMinusculo(char c);

static int lerNumero(const char texto[],
                     int *indice,
                     double *numero);

static int indiceVariavel(const char variaveisDominio[],
                          int dimensaoDominio,
                          char letra);

static int extrairVariaveisDominio(const char texto[],
                                   char variaveisDominio[],
                                   int *dimensaoDominio);

static int extrairExpressoesContradominio(
    const char texto[],
    char expressoesContradominio[MAX_LINHAS][MAX_TAMANHO],
    int *dimensaoContradominio
);

static int interpretarExpressaoContradominio(
    const char expressao[],
    const char variaveisDominio[],
    int dimensaoDominio,
    double coeficientes[MAX_VARIAVEIS]
);

static int montarMatrizTransformacao(
    const char expressoesContradominio[MAX_LINHAS][MAX_TAMANHO],
    int dimensaoContradominio,
    const char variaveisDominio[],
    int dimensaoDominio,
    Matriz *matriz
);

static void imprimirVariaveisDominio(const char variaveisDominio[],
                                     int dimensaoDominio);

static void analisarTransformacao(const Matriz *matriz,
                                  const char variaveisDominio[],
                                  int dimensaoDominio,
                                  int dimensaoContradominio);

static void pausarTransformacao(void);


/*
==========================================================
Funcao principal chamada pelo menu.

Formato esperado:
    T(x,y)=(y+2x,3x+5y)
    T(x,y,z)=(z+x-y,3x+y+2z)
    T(u,v)=(2u-v,v+u,3u)

O programa identifica automaticamente:
    - dimensao do dominio
    - dimensao do contradominio
==========================================================
*/

void executarTransformacaoLinear(void)
{
    char texto[MAX_TAMANHO];

    char variaveisDominio[MAX_VARIAVEIS];
    int dimensaoDominio = 0;

    char expressoesContradominio[MAX_LINHAS][MAX_TAMANHO];
    int dimensaoContradominio = 0;

    Matriz matriz;

    printf("\nQUESTAO 2 - TRANSFORMACAO LINEAR\n");

    printf("\nDigite a transformacao no formato:\n");
    printf("T(x,y)=(y+2x,3x+5y)\n");
    printf("T(u,v)=(2u-v,v+u,3u)\n");
    printf("T(x,y,z)=(z+x-y,3x+y+2z)\n");
    printf("T(x,y,z)=(x+y+z,2x+2y+2z,x-y+z)\n");

    printf("\nTransformação: ");

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

    if(!extrairVariaveisDominio(texto,
                                variaveisDominio,
                                &dimensaoDominio))
    {
        printf("\nErro: nao foi possivel identificar o dominio.\n");
        printf("Use o formato: T(x,y,z)=(...)\n");
        pausarTransformacao();
        return;
    }

    if(!extrairExpressoesContradominio(texto,
                                       expressoesContradominio,
                                       &dimensaoContradominio))
    {
        printf("\nErro: nao foi possivel identificar o contradominio.\n");
        printf("Use o formato: T(x,y)=(..., ...)\n");
        pausarTransformacao();
        return;
    }

    if(!montarMatrizTransformacao(expressoesContradominio,
                                  dimensaoContradominio,
                                  variaveisDominio,
                                  dimensaoDominio,
                                  &matriz))
    {
        printf("\nErro: a transformacao digitada nao e linear ou possui sintaxe invalida.\n");
        printf("Use apenas combinacoes lineares, como: 2x-y+z\n");
        pausarTransformacao();
        return;
    }

    printf("\nTransformacao analisada:\n");
    printf("%s\n", texto);

    imprimirVariaveisDominio(variaveisDominio,
                             dimensaoDominio);

    printf("\nMatriz associada:\n");
    imprimirMatriz(&matriz);

    analisarTransformacao(&matriz,
                          variaveisDominio,
                          dimensaoDominio,
                          dimensaoContradominio);

    pausarTransformacao();
}


/*
==========================================================
Dimensao da imagem.

Teoria:
    dim(Im T) = posto da matriz
==========================================================
*/

int dimensaoImagem(const Matriz *matriz)
{
    return calcularPosto(matriz);
}


/*
==========================================================
Dimensao do nucleo.

Teoria:
    dim(Nuc T) = dimensao do dominio - posto

A dimensao do dominio corresponde ao numero de colunas
da matriz.
==========================================================
*/

int dimensaoNucleo(const Matriz *matriz)
{
    int posto = calcularPosto(matriz);

    return matriz->colunas - posto;
}


/*
==========================================================
Injetiva se dim(Nuc T) = 0
==========================================================
*/

int ehTransformacaoInjetiva(const Matriz *matriz)
{
    return dimensaoNucleo(matriz) == 0;
}


/*
==========================================================
Sobrejetiva se dim(Im T) = dimensao do contradominio

A dimensao do contradominio corresponde ao numero de
linhas da matriz.
==========================================================
*/

int ehTransformacaoSobrejetiva(const Matriz *matriz)
{
    return dimensaoImagem(matriz) == matriz->linhas;
}


/*
==========================================================
Bijetiva se for injetiva e sobrejetiva.
==========================================================
*/

int ehTransformacaoBijetiva(const Matriz *matriz)
{
    return ehTransformacaoInjetiva(matriz) &&
           ehTransformacaoSobrejetiva(matriz);
}


/*
==========================================================
Remove o \n deixado pelo fgets.
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


static int ehLetra(char c)
{
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z');
}


static int ehDigito(char c)
{
    return c >= '0' && c <= '9';
}


static int ehSinal(char c)
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


/*
==========================================================
Le numero decimal sem usar math.h.

Aceita:
    2
    2.5
    0.5
    .5
==========================================================
*/

static int lerNumero(const char texto[],
                     int *indice,
                     double *numero)
{
    double valor = 0.0;
    double casaDecimal = 0.1;

    int i = *indice;
    int possuiDigito = 0;

    while(ehDigito(texto[i]))
    {
        valor = valor * 10.0 + (texto[i] - '0');
        i++;
        possuiDigito = 1;
    }

    if(texto[i] == '.')
    {
        i++;

        while(ehDigito(texto[i]))
        {
            valor = valor + (texto[i] - '0') * casaDecimal;
            casaDecimal = casaDecimal / 10.0;
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


/*
==========================================================
Procura a posicao de uma variavel do dominio.
==========================================================
*/

static int indiceVariavel(const char variaveisDominio[],
                          int dimensaoDominio,
                          char letra)
{
    letra = paraMinusculo(letra);

    for(int i = 0; i < dimensaoDominio; i++)
    {
        if(variaveisDominio[i] == letra)
        {
            return i;
        }
    }

    return -1;
}


/*
==========================================================
Extrai as variaveis do dominio.

Exemplo:
    T(x,y,z)=(...)

Resultado:
    variaveisDominio = x, y, z
    dimensaoDominio = 3
==========================================================
*/

static int extrairVariaveisDominio(const char texto[],
                                   char variaveisDominio[],
                                   int *dimensaoDominio)
{
    int abre = -1;
    int fecha = -1;

    *dimensaoDominio = 0;

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
        if(!ehLetra(texto[i]))
        {
            return 0;
        }

        char letra = paraMinusculo(texto[i]);

        if(letra == 'i' || letra == 'j')
        {
            return 0;
        }

        if(indiceVariavel(variaveisDominio,
                          *dimensaoDominio,
                          letra) != -1)
        {
            return 0;
        }

        if(*dimensaoDominio >= MAX_VARIAVEIS)
        {
            return 0;
        }

        variaveisDominio[*dimensaoDominio] = letra;
        (*dimensaoDominio)++;

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

    return *dimensaoDominio > 0;
}


/*
==========================================================
Extrai as expressoes do contradominio.

Exemplo:
    T(x,y)=(y+2x,3x+5y)

Resultado:
    expressoesContradominio[0] = y+2x
    expressoesContradominio[1] = 3x+5y

    dimensaoContradominio = 2
==========================================================
*/

static int extrairExpressoesContradominio(
    const char texto[],
    char expressoesContradominio[MAX_LINHAS][MAX_TAMANHO],
    int *dimensaoContradominio
)
{
    int igual = -1;
    int abre = -1;
    int fecha = -1;

    *dimensaoContradominio = 0;

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

            expressoesContradominio[*dimensaoContradominio][posExpressao] = '\0';
            (*dimensaoContradominio)++;

            if(*dimensaoContradominio > MAX_LINHAS)
            {
                return 0;
            }

            posExpressao = 0;
        }
        else
        {
            if(posExpressao >= MAX_TAMANHO - 1)
            {
                return 0;
            }

            expressoesContradominio[*dimensaoContradominio][posExpressao] = texto[i];
            posExpressao++;
        }
    }

    return *dimensaoContradominio > 0;
}


/*
==========================================================
Interpreta uma expressao do contradominio.

Exemplo:
    expressao = z+x-y

Variaveis do dominio:
    x y z

Resultado:
    1 -1 1
==========================================================
*/

static int interpretarExpressaoContradominio(
    const char expressao[],
    const char variaveisDominio[],
    int dimensaoDominio,
    double coeficientes[MAX_VARIAVEIS]
)
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

        if(ehSinal(expressao[i]))
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

        if(lerNumero(expressao, &i, &numero))
        {
            temNumero = 1;
        }
        else
        {
            numero = 1.0;
        }

        if(ehLetra(expressao[i]))
        {
            char letra = paraMinusculo(expressao[i]);

            if(letra == 'i' || letra == 'j')
            {
                return 0;
            }

            int coluna = indiceVariavel(variaveisDominio,
                                        dimensaoDominio,
                                        letra);

            if(coluna == -1)
            {
                return 0;
            }

            coeficientes[coluna] += sinal * numero;

            i++;

            if(expressao[i] != '\0' && !ehSinal(expressao[i]))
            {
                return 0;
            }
        }
        else if(temNumero)
        {
            /*
                Transformacao linear nao pode ter termo constante.

                Permitimos apenas constante 0:
                    T(x,y)=(0,x+y)
            */

            if(!ehZero(numero))
            {
                return 0;
            }

            if(expressao[i] != '\0' && !ehSinal(expressao[i]))
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


/*
==========================================================
Monta a matriz da transformacao.

Numero de linhas  = dimensao do contradominio
Numero de colunas = dimensao do dominio
==========================================================
*/

static int montarMatrizTransformacao(
    const char expressoesContradominio[MAX_LINHAS][MAX_TAMANHO],
    int dimensaoContradominio,
    const char variaveisDominio[],
    int dimensaoDominio,
    Matriz *matriz
)
{
    double coeficientes[MAX_VARIAVEIS];

    inicializarMatriz(matriz,
                      dimensaoContradominio,
                      dimensaoDominio);

    for(int linha = 0; linha < dimensaoContradominio; linha++)
    {
        if(!interpretarExpressaoContradominio(
                expressoesContradominio[linha],
                variaveisDominio,
                dimensaoDominio,
                coeficientes))
        {
            return 0;
        }

        for(int coluna = 0; coluna < dimensaoDominio; coluna++)
        {
            matriz->dados[linha][coluna] = coeficientes[coluna];
        }
    }

    return 1;
}


/*
==========================================================
Mostra as variaveis do dominio.
==========================================================
*/

static void imprimirVariaveisDominio(const char variaveisDominio[],
                                     int dimensaoDominio)
{
    printf("\nVariaveis do dominio:\n");

    for(int i = 0; i < dimensaoDominio; i++)
    {
        printf("%c ", variaveisDominio[i]);
    }

    printf("\n");
}


/*
==========================================================
Analisa nucleo, imagem, injetividade, sobrejetividade e
bijetividade.
==========================================================
*/

static void analisarTransformacao(const Matriz *matriz,
                                  const char variaveisDominio[],
                                  int dimensaoDominio,
                                  int dimensaoContradominio)
{
    int posto = calcularPosto(matriz);

    int dimImagem = dimensaoImagem(matriz);
    int dimNucleo = dimensaoNucleo(matriz);

    printf("\nTransformacao identificada:\n");
    printf("T: R%d -> R%d\n",
           dimensaoDominio,
           dimensaoContradominio);

    printf("\nPosto da matriz: %d\n", posto);

    printf("\nDimensao da imagem: %d\n", dimImagem);
    printf("Dimensao do nucleo: %d\n", dimNucleo);

    printf("\nClassificacao:\n");

    if(ehTransformacaoInjetiva(matriz))
    {
        printf("A transformacao e injetora.\n");
    }
    else
    {
        printf("A transformacao nao e injetora.\n");
    }

    if(ehTransformacaoSobrejetiva(matriz))
    {
        printf("A transformacao e sobrejetora.\n");
    }
    else
    {
        printf("A transformacao nao e sobrejetora.\n");
    }

    if(ehTransformacaoBijetiva(matriz))
    {
        printf("A transformacao e bijetora.\n");
    }
    else
    {
        printf("A transformacao nao e bijetora.\n");
    }
}


/*
==========================================================
Pausa.
==========================================================
*/

static void pausarTransformacao(void)
{
    char linha[20];

    printf("\nPressione ENTER para continuar...");
    fgets(linha, sizeof(linha), stdin);
}