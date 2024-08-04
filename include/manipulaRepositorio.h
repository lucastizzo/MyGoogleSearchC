#ifndef MANIPULA_REPOSITORIO_H
#define MANIPULA_REPOSITORIO_H

#define MAX_ARQUIVOS 1000
#define MAX_DESCRICAO_TAM 1024
#define MAX_INDEX_TAM 280
#define MAX_BUFFER_TAM 1024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include "backend.h"
#include "mongoose.h"
#include "rotas.h"
#include "frontend.h"


extern char* repositorioNoticias;


struct respostaServidor;

// Estrutura do resultado de uma pesquisa
typedef struct {
    char nomeArquivo[100];
    char descricao[MAX_DESCRICAO_TAM];
} buscaNoticia;

typedef struct {
    char nomeArquivo[50]; // Nome do arquivo
    char descricaoIndex[MAX_INDEX_TAM];  // Descrição do arquivo
} arquivosIndexados; // Similar a noticia buscado porém não vamos nos ater a isso


struct Rota; // Forward declaration of struct Rota

void inicializaRotasArquivo(struct Rota* raiz);

// Função para remover um arquivo
void removerArquivoHandler(char *requisicao, int sock, struct respostaServidor *resposta);
void removerArquivo(char *requisicao, int sock, struct respostaServidor *resposta,  char *nomeArquivo);    


void importarArquivo(char *requisicao, int sock, struct respostaServidor *resposta);
void mostraArquivoImportado(struct respostaServidor *resposta, int sock, char *nomeArquivo);

void listarArquivos(char *requisicao, int sock, struct respostaServidor *resposta);

#endif /* MANIPULA_REPOSITORIO_H */
