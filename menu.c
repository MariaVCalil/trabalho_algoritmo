#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "menu.h"
#include "parser.h"
#include "matriz.h"
#include "sistemas.h"
#include "transf_l.h"
#include "base.h"
#include "autovalores.h"
#include "diagonal.h"

#define LARGURA_LINHA 70

/*
==========================================================
Funcoes privadas do menu.c
==========================================================
*/

static void escreverLinha(void);
static void escreveTitulo(const char titulo[], const char subtitulo[]);
static void centralizador(int quantidade);
static void limparTela(void);
static void pausar(void);
static int lerOpcao(void);

static void escreverMenuPrincipal(void);
static void menuSistemas(void);
static void executarSistemaLinear(void);

static void naoImplementado(void);
static void encerramento(void);
static void invalida(int inicio, int fim);


/*
==========================================================
Menu principal
==========================================================
*/

void menuInicial(void)
{
    int opcao;

    do
    {
        limparTela();

        escreveTitulo("SISTEMA ALGEBRA LINEAR", "");

        escreverMenuPrincipal();

        opcao = lerOpcao();

        limparTela();

        switch(opcao)
        {
            case 1:
                escreveTitulo("SISTEMA ALGEBRA LINEAR", " - SISTEMAS LINEARES");
                menuSistemas();
                break;

            case 2:
                escreveTitulo("SISTEMA ALGEBRA LINEAR", " - TRANSFORMACAO LINEAR");
                executarTransformacaoLinear();
                break;

             case 3:
                escreveTitulo("SISTEMA ALGEBRA LINEAR", " - BASES");
                executarBases();
                break;

            case 4:
                escreveTitulo("SISTEMA ALGEBRA LINEAR", " - AUTOVALORES E AUTOVETORES");
                executarAutovalores();
                break;

            case 5:
                escreveTitulo("SISTEMA ALGEBRA LINEAR", " - DIAGONALIZACAO");
                executarDiagonalizacao();
            break;

            case 6:
                escreveTitulo("SISTEMA ALGEBRA LINEAR", " - ARQUIVOS");
                naoImplementado();
                break;

            case 7:
                encerramento();
                break;

            default:
                invalida(1, 7);
                pausar();
                break;
        }

    } while(opcao != 7);
}


/*
==========================================================
Escreve as opcoes do menu principal
==========================================================
*/

static void escreverMenuPrincipal(void)
{
    printf("\t[1] Resolver um sistema linear\n");
    printf("\t[2] Verificar transformacao linear\n");
    printf("\t[3] Determinar bases\n");
    printf("\t[4] Calcular autovalores e autovetores\n");
    printf("\t[5] Diagonalizar uma matriz\n");
    printf("\t[6] Arquivos\n");
    printf("\t[7] Sair\n");

    escreverLinha();
}


/*
==========================================================
Submenu de sistemas lineares
==========================================================
*/

static void menuSistemas(void)
{
    int opcao;

    do
    {
        printf("\t[1] Escrever sistema linear\n");
        printf("\t[2] Ler sistema linear de arquivo\n");
        printf("\t[3] Voltar\n");

        escreverLinha();

        opcao = lerOpcao();

        limparTela();

        switch(opcao)
        {
            case 1:
                escreveTitulo("SISTEMA ALGEBRA LINEAR", " - LEITURA DE SISTEMA");
                executarSistemaLinear();
                break;

            case 2:
                escreveTitulo("SISTEMA ALGEBRA LINEAR", " - LEITURA DE ARQUIVO");
                naoImplementado();
                break;

            case 3:
                return;

            default:
                invalida(1, 3);
                pausar();
                break;
        }

        if(opcao != 3)
        {
            limparTela();
            escreveTitulo("SISTEMA ALGEBRA LINEAR", " - SISTEMAS LINEARES");
        }

    } while(opcao != 3);
}


/*
==========================================================
Executa a parte que ja fizemos:
    - ler equacoes
    - identificar variaveis
    - montar matriz
    - classificar SI / SPI / SPD
    - resolver
==========================================================
*/

static void executarSistemaLinear(void)
{
    SistemaEntrada entrada;
    Variaveis variaveis;
    Matriz matriz;

    lerSistema(&entrada);

    if(entrada.quantidade == 0)
    {
        printf("\nNenhuma equacao foi digitada.\n");
        pausar();
        return;
    }

    if(!identificarVariaveis(&entrada, &variaveis))
    {
        printf("\nNao foi possivel identificar as variaveis.\n");
        pausar();
        return;
    }

    imprimirVariaveis(&variaveis);

    if(!montarSistema(&entrada, &variaveis, &matriz))
    {
        printf("\nNao foi possivel montar a matriz aumentada.\n");
        pausar();
        return;
    }

    printf("\nMatriz aumentada montada:\n");
    imprimirMatriz(&matriz);

    resolverSistema(&matriz, &variaveis);

    pausar();
}


/*
==========================================================
Funcionalidade ainda nao implementada
==========================================================
*/

static void naoImplementado(void)
{
    printf("\n\tFuncionalidade nao implementada ainda.\n");
    pausar();
}


/*
==========================================================
Mensagem de encerramento
==========================================================
*/

static void encerramento(void)
{
    printf("\nAte logo!\n");
}


/*
==========================================================
Mensagem de opcao invalida
==========================================================
*/

static void invalida(int inicio, int fim)
{
    printf("\nOpcao invalida! Escolha uma opcao entre %d e %d.\n",
           inicio,
           fim);
}


/*
==========================================================
Limpa a tela.

Funciona em Linux e Windows.
==========================================================
*/

static void limparTela(void)
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}


/*
==========================================================
Pausa para o usuario conseguir ler a resposta
==========================================================
*/

static void pausar(void)
{
    printf("\nPressione ENTER para continuar...");
    getchar();
}


/*
==========================================================
Le uma opcao usando fgets.

Isso evita problema entre scanf e fgets.
==========================================================
*/

static int lerOpcao(void)
{
    char linha[20];
    int valor;

    printf("  -> ");

    if(fgets(linha, sizeof(linha), stdin) == NULL)
    {
        return -1;
    }

    if(sscanf(linha, "%d", &valor) != 1)
    {
        return -1;
    }

    return valor;
}


/*
==========================================================
Escreve uma linha divisoria simples.
==========================================================
*/

static void escreverLinha(void)
{
    for(int i = 0; i < LARGURA_LINHA; i++)
    {
        printf("=");
    }

    printf("\n");
}


/*
==========================================================
Escreve o titulo centralizado
==========================================================
*/

static void escreveTitulo(const char titulo[], const char subtitulo[])
{
    char texto[150];

    strcpy(texto, titulo);
    strcat(texto, subtitulo);

    int tamanho = strlen(texto);
    int espacos = (LARGURA_LINHA - tamanho) / 2;

    escreverLinha();

    centralizador(espacos);
    printf("%s\n", texto);

    escreverLinha();
}


/*
==========================================================
Imprime espacos para centralizar
==========================================================
*/

static void centralizador(int quantidade)
{
    for(int i = 0; i < quantidade; i++)
    {
        printf(" ");
    }
}