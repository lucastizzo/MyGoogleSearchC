#ifndef MANIPULA_REPOSITORIO_H
#define MANIPULA_REPOSITORIO_H

#define MAX_ARQUIVOS 1000
#define MAX_DESCRICAO_TAM 1024
#define MAX_INDEX_TAM 80
#define MAX_BUFFER_TAM 1024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/socket.h>
// Estrutura do resultado de uma pesquisa
typedef struct {
    char nomeArquivo[100];
    char descricao[MAX_DESCRICAO_TAM];
} buscaNoticia;

typedef struct {
    char nomeArquivo[50]; // Nome do arquivo
    char descricaoIndex[MAX_INDEX_TAM];  // Descrição do arquivo
} arquivosIndexados; // Similar a noticia buscado porém não vamos nos ater a isso

// Como a usaremos como um vetor precisaremos  declarar a estrutura como um vetor por aqui


// Função para indexar um novo arquivo
void indexarArquivo(const char *nomeArquivo, const char *descricao);

// Função para remover um arquivo
void removerArquivo(const char *nomeArquivo, const char *repositorioNoticias);

// Função que retorna todos os arquivos indexados
void mostrarArquivoIndexado();

void importarArquivo(int sockfd, const char *arquivoImporta,char arquivoDestino[]);

void listarArquivos(const char *repositorioNoticias);

#endif /* MANIPULA_REPOSITORIO_H */
