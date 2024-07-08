#include "rotas.h"

// Bibliotecas importados do arquivo Rotas.h
//#include <stdlib.h>
//#include <string.h>
//#include <stdio.h>

struct Rota * iniciaRota(char* chave, char* valor) {
	struct Rota * temp = (struct Rota *) malloc(sizeof(struct Rota));

	temp->chave = chave;
	temp->valor = valor;

	temp->esquerda = temp->direita = NULL;
	return temp;
}

void ordena(struct Rota* raiz)
{

    if (raiz != NULL) {
        ordena(raiz->esquerda);
        printf("%s -> %s \n", raiz->chave, raiz->valor); // Corrigido para imprimir o valor ao invés da chave duplicada
        ordena(raiz->direita);
    }
}

struct Rota * adicionaRota(struct Rota * raiz, char* chave, char* valor) {
    if (raiz == NULL) {
        raiz = iniciaRota(chave, valor);
        return raiz;
    }

    if (strcmp(chave, raiz->chave) == 0) {
        printf("Já existe uma rota para %s\n", chave);
    } else if (strcmp(chave, raiz->chave) > 0) {
        raiz->direita = adicionaRota(raiz->direita, chave, valor);
    } else {
        raiz->esquerda = adicionaRota(raiz->esquerda, chave, valor);
    }
    return raiz;
}

struct Rota * buscaRota(struct Rota * raiz, char* chave) {
	if (raiz == NULL) {
		return NULL;
	} 

	if (strcmp(chave, raiz->chave) == 0){
		return raiz;
	}else if (strcmp(chave, raiz->chave) > 0) {
		return buscaRota(raiz->direita, chave);
	}else{
		return buscaRota(raiz->esquerda, chave);
	}   

}

struct Rota* inicializaRaiz() {
    struct Rota* raiz = malloc(sizeof(struct Rota));
    raiz->chave = strdup("/");  // A chave da rota raiz é "/"
    raiz->valor = strdup("raiz");  
    raiz->esquerda = NULL;
    raiz->direita = NULL;
    return raiz;
}