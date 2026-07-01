#ifndef SISTEMAS_H
#define SISTEMAS_H

#include "matriz.h"
#include "parser.h"

/*
==========================================================
TipoSistema

SI  -> Sistema Impossivel
SPI -> Sistema Possivel e Indeterminado
SPD -> Sistema Possivel e Determinado
==========================================================
*/

typedef enum
{
    SISTEMA_SI,
    SISTEMA_SPI,
    SISTEMA_SPD

} TipoSistema;


/*
==========================================================
Calcula o posto de uma matriz.
==========================================================
*/

int calcularPosto(const Matriz *m);


/*
==========================================================
Escalona uma matriz.

Essa funcao sera usada para calcular posto.
==========================================================
*/

void escalonarMatriz(Matriz *m);


/*
==========================================================
Classifica o sistema usando posto(A) e posto(A|b).

Retorno:
    SISTEMA_SI
    SISTEMA_SPI
    SISTEMA_SPD
==========================================================
*/

TipoSistema classificarSistema(const Matriz *aumentada);


/*
==========================================================
Resolve e imprime o resultado do sistema.
==========================================================
*/

void resolverSistema(const Matriz *aumentada, const Variaveis *vars);


/*
==========================================================
Imprime o tipo do sistema.
==========================================================
*/

void imprimirTipoSistema(TipoSistema tipo);

#endif