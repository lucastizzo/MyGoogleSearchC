#ifndef FRONTEND_H
#define FRONTEND_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/socket.h>
#include <ctype.h>
#include "backend.h"
#include "mongoose.h"
#include "rotas.h"


struct respostaServidor;
struct Rota; // Forward declaration of struct Rota

void adicionarRotaPadrao(struct Rota **raiz);
void url_decode(char* src, char* dest);
void montaHTML(int sock, struct respostaServidor *resposta, const char* nomeArquivo);
void enviaResposta(int sock, struct respostaServidor *resposta);
void paginaInicial(char *requisicao,int sock, struct respostaServidor *resposta);
void servir404(int sock, struct respostaServidor *resposta);


#endif