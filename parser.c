#include <stdio.h>
#include <string.h>

#include "parser.h"

/*
==========================================================
Funcoes privadas do parser.c

Como sao static, elas so podem ser usadas dentro deste
arquivo.
==========================================================
*/

static void removerQuebraLinha(char texto[]);
static void removerEspacos(char texto[]);

static int ehDigito(char c);
static int ehLetra(char c);
static int ehSinal(char c);

static char paraMinusculo(char c);

static void limparVariaveis(Variaveis *vars);
static void limparEquacao(Equacao *eq);

static int indiceVariavel(const Variaveis *vars, char letra);
static int adicionarVariavel(Variaveis *vars, char letra);

static void ordenarVariaveis(Variaveis *vars);
static void atualizarColunas(Variaveis *vars);

static int encontrarIgual(const char texto[]);

static int lerNumero(const char texto[],int *indice,double *numero);

static int interpretarLadoEsquerdo(const char texto[],int fim,const Variaveis *vars,Equacao *eq);

static int interpretarLadoDireito(const char texto[],int inicio,Equacao *eq);

static int interpretarEquacao(const char texto[],const Variaveis *vars, Equacao *eq);

static void preencherLinha(const Equacao *eq, Matriz *matriz,int linha,int quantidadeVariaveis);


/*
==========================================================
Funcao publica:
    lerSistema

Objetivo:
    Ler as equacoes digitadas pelo usuario.

Regra:
    O usuario NAO informa a quantidade de equacoes.
    O programa conta automaticamente.
==========================================================
*/

void lerSistema(SistemaEntrada *entrada)
{
    char linha[MAX_TAMANHO];

    entrada->quantidade = 0;

    for(int i = 0; i < MAX_EQUACOES; i++)
    {
        entrada->texto[i][0] = '\0';
    }

    printf("\nDigite as equacoes do sistema.\n");
    printf("Pressione ENTER em branco para finalizar.\n");
    printf("Limite maximo: %d equacoes.\n\n", MAX_EQUACOES);

    for(int i = 0; i < MAX_EQUACOES; i++)
    {
        printf("Equacao %d: ", i + 1);

        if(fgets(linha, MAX_TAMANHO, stdin) == NULL)
        {
            break;
        }

        removerQuebraLinha(linha);
        removerEspacos(linha);

        /*
            Se o usuario apertar ENTER sem digitar nada,
            a leitura termina.
        */
        if(strlen(linha) == 0)
        {
            break;
        }

        strcpy(entrada->texto[entrada->quantidade], linha);
        entrada->quantidade++;
    }

    if(entrada->quantidade == 0)
    {
        printf("\nNenhuma equacao foi informada.\n");
    }
}


/*
==========================================================
Funcao publica:
    identificarVariaveis

Objetivo:
    Descobrir automaticamente quais variaveis aparecem
    no sistema.

Exemplo:
    2y+x+5z=10

Variaveis encontradas:
    x, y, z
==========================================================
*/

int identificarVariaveis(const SistemaEntrada *entrada,
                         Variaveis *vars)
{
    limparVariaveis(vars);

    if(entrada->quantidade <= 0)
    {
        printf("\nErro: nao ha equacoes para analisar.\n");
        return 0;
    }

    for(int i = 0; i < entrada->quantidade; i++)
    {
        int posIgual = encontrarIgual(entrada->texto[i]);

        if(posIgual == -1)
        {
            printf("\nErro: equacao %d deve possuir exatamente um sinal de igual.\n", i + 1);
            return 0;
        }

        for(int j = 0; entrada->texto[i][j] != '\0'; j++)
        {
            if(ehLetra(entrada->texto[i][j]))
            {
                char letra = paraMinusculo(entrada->texto[i][j]);

                /*
                    Bloqueamos i e j para evitar interpretacao
                    como numero complexo.
                */
                if(letra == 'i' || letra == 'j')
                {
                    printf("\nErro: a equacao %d possui possivel valor complexo.\n", i + 1);
                    printf("O programa trabalha apenas com numeros reais.\n");
                    return 0;
                }

                /*
                    As variaveis devem aparecer antes do sinal de igual.
                    O lado direito deve ser apenas um numero.
                */
                if(j > posIgual)
                {
                    printf("\nErro: o lado direito da equacao %d deve ser numerico.\n", i + 1);
                    return 0;
                }

                if(!adicionarVariavel(vars, letra))
                {
                    printf("\nErro: o sistema possui mais de %d variaveis.\n", MAX_VARIAVEIS);
                    return 0;
                }
            }
        }
    }

    if(vars->quantidade == 0)
    {
        printf("\nErro: nenhuma variavel foi encontrada.\n");
        return 0;
    }

    ordenarVariaveis(vars);
    atualizarColunas(vars);

    return 1;
}


/*
==========================================================
Funcao publica:
    montarSistema

Objetivo:
    Interpretar todas as equacoes e montar a matriz
    aumentada.

Importante:
    A matriz so sera preenchida se TODAS as equacoes
    forem validas.
==========================================================
*/

int montarSistema(const SistemaEntrada *entrada,
                  const Variaveis *vars,
                  Matriz *matriz)
{
    Equacao equacoes[MAX_EQUACOES];

    if(entrada->quantidade <= 0)
    {
        printf("\nErro: nao ha equacoes para montar o sistema.\n");
        return 0;
    }

    if(vars->quantidade <= 0)
    {
        printf("\nErro: nao ha variaveis identificadas.\n");
        return 0;
    }

    for(int i = 0; i < MAX_EQUACOES; i++)
    {
        limparEquacao(&equacoes[i]);
    }

    /*
        Primeiro interpretamos todas as equacoes.
        Ainda NAO colocamos nada na matriz.
    */
    for(int i = 0; i < entrada->quantidade; i++)
    {
        if(!interpretarEquacao(entrada->texto[i],vars,&equacoes[i]))
        {
            printf("\nErro: a equacao %d possui sintaxe invalida.\n", i + 1);
            return 0;
        }
    }

    /*
        Se chegou aqui, todas as equacoes sao validas.
        Agora sim podemos criar a matriz aumentada.
    */
    inicializarMatriz(matriz,entrada->quantidade,vars->quantidade + 1);

    for(int i = 0; i < entrada->quantidade; i++)
    {
        preencherLinha(&equacoes[i],matriz,i,vars->quantidade);
    }

    return 1;
}


/*
==========================================================
Funcao publica:
    imprimirVariaveis

Objetivo:
    Mostrar a ordem das variaveis encontradas.
==========================================================
*/

void imprimirVariaveis(const Variaveis *vars)
{
    printf("\nVariaveis detectadas:\n");

    for(int i = 0; i < vars->quantidade; i++)
    {
        printf("%c ", vars->lista[i].nome);
    }

    printf("\n");
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
Remove espacos em branco da equacao.

Exemplo:
    2y + x = 5

Vira:
    2y+x=5
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
Verifica se um caractere e digito.
==========================================================
*/

static int ehDigito(char c)
{
    return c >= '0' && c <= '9';
}


/*
==========================================================
Verifica se um caractere e letra.
==========================================================
*/

static int ehLetra(char c)
{
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z');
}


/*
==========================================================
Verifica se um caractere e sinal + ou -.
==========================================================
*/

static int ehSinal(char c)
{
    return c == '+' || c == '-';
}


/*
==========================================================
Converte letra maiuscula para minuscula.

Exemplo:
    X vira x
==========================================================
*/

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
Limpa a lista de variaveis.
==========================================================
*/

static void limparVariaveis(Variaveis *vars)
{
    vars->quantidade = 0;

    for(int i = 0; i < MAX_VARIAVEIS; i++)
    {
        vars->lista[i].nome = '\0';
        vars->lista[i].coluna = -1;
    }
}


/*
==========================================================
Limpa uma equacao interpretada.
==========================================================
*/

static void limparEquacao(Equacao *eq)
{
    for(int i = 0; i < MAX_VARIAVEIS; i++)
    {
        eq->coeficientes[i] = 0.0;
    }

    eq->resultado = 0.0;
    eq->valida = 0;
}


/*
==========================================================
Procura uma variavel na lista.

Retorna:
    coluna da variavel, se encontrou
    -1, se nao encontrou
==========================================================
*/

static int indiceVariavel(const Variaveis *vars, char letra)
{
    letra = paraMinusculo(letra);

    for(int i = 0; i < vars->quantidade; i++)
    {
        if(vars->lista[i].nome == letra)
        {
            return vars->lista[i].coluna;
        }
    }

    return -1;
}


/*
==========================================================
Adiciona uma variavel na lista, sem repetir.

Retorna:
    1 -> sucesso
    0 -> erro, passou do limite
==========================================================
*/

static int adicionarVariavel(Variaveis *vars, char letra)
{
    letra = paraMinusculo(letra);

    for(int i = 0; i < vars->quantidade; i++)
    {
        if(vars->lista[i].nome == letra)
        {
            return 1;
        }
    }

    if(vars->quantidade >= MAX_VARIAVEIS)
    {
        return 0;
    }

    vars->lista[vars->quantidade].nome = letra;
    vars->lista[vars->quantidade].coluna = vars->quantidade;
    vars->quantidade++;

    return 1;
}


/*
==========================================================
Ordena as variaveis em ordem alfabetica.

Exemplo:
    y x z

Vira:
    x y z
==========================================================
*/

static void ordenarVariaveis(Variaveis *vars)
{
    Variavel auxiliar;

    for(int i = 0; i < vars->quantidade - 1; i++)
    {
        for(int j = i + 1; j < vars->quantidade; j++)
        {
            if(vars->lista[j].nome < vars->lista[i].nome)
            {
                auxiliar = vars->lista[i];
                vars->lista[i] = vars->lista[j];
                vars->lista[j] = auxiliar;
            }
        }
    }
}


/*
==========================================================
Atualiza a coluna de cada variavel depois da ordenacao.
==========================================================
*/

static void atualizarColunas(Variaveis *vars)
{
    for(int i = 0; i < vars->quantidade; i++)
    {
        vars->lista[i].coluna = i;
    }
}


/*
==========================================================
Verifica se existe exatamente um sinal de igual.

Retorna:
    posicao do =
    -1 se nao houver ou se houver mais de um
==========================================================
*/

static int encontrarIgual(const char texto[])
{
    int posicao = -1;
    int quantidade = 0;

    for(int i = 0; texto[i] != '\0'; i++)
    {
        if(texto[i] == '=')
        {
            posicao = i;
            quantidade++;
        }
    }

    if(quantidade != 1)
    {
        return -1;
    }

    return posicao;
}


/*
==========================================================
Le um numero a partir de uma posicao da string.

Exemplo:
    texto = "2.5x"
    indice comeca em 0

Resultado:
    numero = 2.5
    indice passa a apontar para x

Observacao:
    Esta funcao NAO le sinal.
    O sinal e tratado antes.
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
Interpreta o lado esquerdo da equacao.

Exemplo:
    2y+x-5z

Com variaveis:
    x y z

Gera:
    x = 1
    y = 2
    z = -5
==========================================================
*/

static int interpretarLadoEsquerdo(const char texto[],int fim, const Variaveis *vars, Equacao *eq)
{
    int i = 0;

    if(fim <= 0)
    {
        return 0;
    }

    while(i < fim)
    {
        int sinal = 1;
        double numero = 1.0;
        int temNumero = 0;

        if(ehSinal(texto[i]))
        {
            if(texto[i] == '-')
            {
                sinal = -1;
            }

            i++;

            if(i >= fim)
            {
                return 0;
            }
        }

        /*
            Tenta ler um numero.

            Exemplos:
                2x  -> numero 2
                5   -> constante 5
                x   -> nao tem numero, entao coeficiente 1
        */
        if(lerNumero(texto, &i, &numero))
        {
            temNumero = 1;
        }
        else
        {
            numero = 1.0;
        }

        /*
            Caso 1:
            Depois do numero veio uma letra.

            Exemplo:
                2x
                -5y
                x
        */
        if(i < fim && ehLetra(texto[i]))
        {
            char letra = paraMinusculo(texto[i]);

            if(letra == 'i' || letra == 'j')
            {
                return 0;
            }

            int coluna = indiceVariavel(vars, letra);

            if(coluna == -1)
            {
                return 0;
            }

            eq->coeficientes[coluna] += sinal * numero;

            i++;

            if(i < fim && !ehSinal(texto[i]))
            {
                return 0;
            }
        }
        /*
            Caso 2:
            Era apenas um numero no lado esquerdo.

            Exemplo:
                x + 5 = 2

            O 5 precisa ir para o outro lado:
                x = 2 - 5
        */
        else if(temNumero)
        {
            eq->resultado -= sinal * numero;

            if(i < fim && !ehSinal(texto[i]))
            {
                return 0;
            }
        }
        /*
            Caso 3:
            Nao tem numero e nao tem letra.
            Entao a sintaxe esta errada.
        */
        else
        {
            return 0;
        }
    }

    return 1;
}

/*
==========================================================
Interpreta o lado direito da equacao.

Exemplos:
    =10
    =-5
    =2.5

O lado direito deve ser apenas um numero real.
==========================================================
*/

static int interpretarLadoDireito(const char texto[],
                                  int inicio,
                                  Equacao *eq)
{
    int i = inicio;
    int sinal = 1;
    double numero = 0.0;

    if(texto[i] == '\0')
    {
        return 0;
    }

    if(ehSinal(texto[i]))
    {
        if(texto[i] == '-')
        {
            sinal = -1;
        }

        i++;

        if(texto[i] == '\0')
        {
            return 0;
        }
    }

    if(!lerNumero(texto, &i, &numero))
    {
        return 0;
    }

    /*
        Se sobrou qualquer caractere depois do numero,
        entao a sintaxe esta incorreta.
    */
    if(texto[i] != '\0')
    {
        return 0;
    }

    /*
        Aqui atribuimos o resultado do lado direito.

        Exemplo:
            x + 5 = 2

        Primeiro:
            resultado = 2

        Depois, o lado esquerdo ajusta:
            resultado = 2 - 5
    */
    eq->resultado = sinal * numero;

    return 1;
}

/*
==========================================================
Interpreta uma equacao completa.

Exemplo:
    2y+x-5z=10
==========================================================
*/

static int interpretarEquacao(const char texto[],
                              const Variaveis *vars,
                              Equacao *eq)
{
    char copia[MAX_TAMANHO];

    limparEquacao(eq);

    strcpy(copia, texto);
    removerEspacos(copia);

    int posIgual = encontrarIgual(copia);

    if(posIgual == -1)
    {
        return 0;
    }

    if(posIgual == 0)
    {
        return 0;
    }

    if(copia[posIgual + 1] == '\0')
    {
        return 0;
    }

    if(!interpretarLadoDireito(copia,
                           posIgual + 1,
                           eq))
{
    return 0;
}

if(!interpretarLadoEsquerdo(copia,posIgual,vars, eq))
{
    return 0;
}
    eq->valida = 1;

    return 1;
}


/*
==========================================================
Preenche uma linha da matriz aumentada.

Exemplo:
    coeficientes: 1 2 -5
    resultado: 10

Matriz:
    1  2  -5 | 10
==========================================================
*/

static void preencherLinha(const Equacao *eq,Matriz *matriz,int linha,int quantidadeVariaveis)
{
    for(int coluna = 0; coluna < quantidadeVariaveis; coluna++)
    {
        matriz->dados[linha][coluna] = eq->coeficientes[coluna];
    }

    matriz->dados[linha][quantidadeVariaveis] = eq->resultado;
}