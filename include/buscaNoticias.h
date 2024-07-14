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

#define MAX_DESCRICAO_NOT_TAM 1024 // Definir o tamanho máximo da descrição encontrada
#define MAX_NOME_ARQUIVO 100   // Definir o tamanho máximo do nome do arquivo
#define MAX_LINHA 1024 // Numero maximo de linhas que podem ser contadas

// Estrutura do resultado de uma pesquisa
typedef struct {
    char nomeArquivo[MAX_NOME_ARQUIVO];
    char descricaoEncontrada[MAX_DESCRICAO_NOT_TAM];
    int total;
} buscaNoticias;

// Protótipo da função para fazer a busca
void busca(int numPesquisa,const char *termoChave,  buscaNoticias resultados[]);
void resultadoPesquisahandler(char *requisicao, int sock, struct respostaServidor *resposta);
void inicializaRotasBusca(struct Rota* raiz);
void resultadoPesquisa(char *requisicao, int sock, struct respostaServidor *resposta, int numPesquisa, char *termoChave);

#endif /* BUSCA_NOTICIAS_H */
