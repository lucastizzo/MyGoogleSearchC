#ifndef BUSCA_NOTICIAS_H
#define BUSCA_NOTICIAS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "servidorHTTP.h"
#include "manipulaRepositorio.h"
#include "rotas.h"
#include "mongoose.h"
#include "paginasPadrao.h"
#include <json-c/json.h>
#include <iconv.h>

#define MAX_DESCRICAO_NOT_TAM 400 // Definir o tamanho máximo da descrição encontrada
#define MAX_NOME_ARQUIVO 100   // Definir o tamanho máximo do nome do arquivo
#define MAX_LINHA 1024 // Numero maximo de linhas que podem ser contadas
#define MAX_TITULO_TAM 600
#define MAX_URL_TAM 400

// Estrutura do resultado de uma pesquisa
typedef struct {
    char nomeArquivo[MAX_NOME_ARQUIVO];
    char titulo[MAX_TITULO_TAM];
    char url [MAX_URL_TAM]; 
    char descricaoEncontrada[MAX_DESCRICAO_NOT_TAM];
    int total;
} buscaNoticias;

// Protótipo da função para fazer a busca
char *limparCaracteresInvalidos(const char *entrada);
void busca(int numPesquisa,const char *termoChave,  buscaNoticias resultados[]);
void resultadoPesquisahandler(char *requisicao, int sock, struct respostaServidor *resposta);
void inicializaRotasBusca(struct Rota* raiz);
void resultadoPesquisa(char *requisicao, int sock, struct respostaServidor *resposta, int numPesquisa, char *termoChave);

#endif /* BUSCA_NOTICIAS_H */
