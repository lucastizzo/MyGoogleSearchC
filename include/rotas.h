#ifndef ROTAS_H
#define ROTAS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct Rota { // Estrutura para busca binaria
	char* chave;
	char* valor;

	struct Rota *esquerda, *direita;
};

struct Rota * iniciaRota(char* chave, char* valor);

struct Rota * adicionaRota(struct Rota * raiz, char* chave, char* valor);

struct Rota * buscaRota(struct Rota * raiz, char * chave);

struct Rota* inicializaRaiz(); // Função que inicializa a raiz da árvore de rotas

void ordena(struct Rota * raiz );
void listaArquivo();

#endif
