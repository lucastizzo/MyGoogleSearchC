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

#define MAX_DESCRICAO_NOT_TAM 1024 // Definir o tamanho máximo da descrição encontrada
#define MAX_NOME_ARQUIVO 100   // Definir o tamanho máximo do nome do arquivo
#define MAX_LINHA 10000000 // Numero maximo de linhas que podem ser contadas

// Estrutura do resultado de uma pesquisa
typedef struct {
    char nomeArquivo[MAX_NOME_ARQUIVO];
    char descricaoEncontrada[MAX_DESCRICAO_NOT_TAM];
} buscaNoticias;

// Protótipo da função para fazer a busca
buscaNoticias* busca(int numPesquisa,const char *termoChave);
void inicializaRotasBusca();
//Função chamada pelo socket para 
void resultadoPesquisa(struct respostaServidor *resposta, int numPesquisa,const char *termoChave);

#endif /* BUSCA_NOTICIAS_H */
