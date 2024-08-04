#ifndef BACKEND_H
#define BACKEND_H

#include <stdlib.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <unistd.h>    
#include <pthread.h>   
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>
#include "mongoose.h"
#include "signal.h"
#include <sys/sendfile.h>
#include "manipulaRepositorio.h"
#include <time.h>
#include "frontend.h"

#define socketErro -1

struct respostaServidor {
    char header[8000];
    char conteudo[100000];
};

struct servidorEndereco { 
    short sin_family; // Família de Endereços 
    unsigned short sin_port; // Numero da porta - convertido para ordem de bytes da rede
    struct in_addr sin_addr; // Endereço IP do servidor
    char sin_zero[8]; // Preenchimento para manter o mesmo tamanho da estrutura sockaddr_in que queremos copiar
}; // Essa é a estrutura sockaddr_in

// Definindo a estrutura do servidor HTTP 
typedef struct {
    int porta;
    int socket;
} servidorHTTP;

// Define a estrutura do socket cliente
typedef struct {
    int porta;
    int socket;
} socketClient;

int criaSocketServidor(int porta, int maximoConexoes);
int iniciaServidor(servidorHTTP* servidorHTTP, int porta, int maximoConexoes);
int verificaErroSocket(int ver, const char *msg);
void *manipulaConexao(void *cliente_socket);
void manipulaRequisicao(char *requisicao, int sock);
void sigpipe_handler(int signum);

#endif