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
        printf("%s -> %s \n", raiz->chave, raiz->chave);
        ordena(raiz->direita);
    }
}

struct Rota * adicionaRota(struct Rota * raiz, char* chave, char* valor) {
	if (raiz == NULL) {
		return iniciaRota(chave, valor);
	}

	if (strcmp(chave, raiz->chave) == 0) {
		printf("Já existe uma rota para %s", chave);
	}else if (strcmp(chave, raiz->chave) > 0) {
		raiz->direita = adicionaRota(raiz->direita, chave, valor);
	}else {
		raiz->esquerda = adicionaRota(raiz->esquerda, chave, valor);
	}
}

struct Rota * buscaRota(struct Rota * raiz, char* chave) {
	if (raiz == NULL) {
		return NULL;
	} 

	if (strcmp(chave, raiz->chave) == 0){
		return raiz;
	}else if (strcmp(chave, raiz->chave) > 0) {
		return buscaRota(raiz->direita, chave);
	}else if (strcmp(chave, raiz->chave) < 0) {
		return buscaRota(raiz->esquerda, chave);
	}  

}
