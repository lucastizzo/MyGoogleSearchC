#ifndef PAGINAS_PADRAO_H
#define PAGINAS_PADRAO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/socket.h>
#include "servidorHTTP.h"
#include "mongoose.h"
#include "rotas.h"


struct respostaServidor;
struct Rota; // Forward declaration of struct Rota

void adicionarRotaPadrao(struct Rota **raiz);
void montaHTML(int sock, struct respostaServidor *resposta, const char* nomeArquivo);
void enviaResposta(int sock, struct respostaServidor *resposta);
void paginaInicial(char *requisicao, int sock, struct respostaServidor *resposta);
void servir404(char *requisicao, int sock, struct respostaServidor *resposta);


#endif