#ifndef ROTAS_H
#define ROTAS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "frontend.h"

struct respostaServidor;
typedef void (*RotaFunc)(char *, int, struct respostaServidor *);

struct Rota {
    char* chave; 
    char* valor; 
    RotaFunc funcao; 
    struct Rota *esquerda, *direita; 
};

struct Rota* inicializaRaiz();
struct Rota* adicionaRota(struct Rota *raiz, char *chave, RotaFunc func);
struct Rota* buscaRota(struct Rota *raiz, char* chave);
void listaTodasRotas(struct Rota *raiz);


#endif
