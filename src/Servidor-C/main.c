#include "main.h"
#include "backend.h"
#include "buscaNoticias.h"
#include "manipulaRepositorio.h"
#include "rotas.h"
#include <locale.h>
#include <pthread.h>

#define porta 8080
#define tamanhoBuffer 4096
#define maximoConexoes 10000000
#define NUM_SERVERS 3 


const char *servidoresBackend[NUM_SERVERS][2] = {
    {"127.0.0.1", "8081"},
    {"127.0.0.1", "8082"},
    {"127.0.0.1", "8083"}
};

int servidorAtual = 0;

// Definindo o nome do servidor e o repositório de notícias
char *nomeServidor = "MyGoogleSearchC";
char *repositorioNoticias = "/home/tizzo/projeto-SD/repositorio-noticias/";

void iniciaThreadConexao(int servidorSocket) {
    fd_set readfds;
    int max_sd, sd, activity;

    while (1) {
        // Limpa o conjunto de descritores de arquivo
        FD_ZERO(&readfds);

        // Adiciona o socket do servidor ao conjunto
        FD_SET(servidorSocket, &readfds);
        max_sd = servidorSocket;

        // Espera por atividade em um dos sockets
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            printf("Erro no select");
        }

        // Se algo aconteceu no socket do servidor, é uma nova conexão
        if (FD_ISSET(servidorSocket, &readfds)) {
            int client_socket = accept(servidorSocket, NULL, NULL);

            if (client_socket < 0) {
                perror("Erro ao aceitar conexão");
                continue;
            }

            int *pclient = malloc(sizeof(int));
            if (pclient == NULL) {
                perror("Erro ao alocar memória para pclient");
                close(client_socket);
                continue;
            }
            *pclient = client_socket;

            pthread_t thread_id;
            printf("Chamando a thread para manipulaConexao\n");
            if (pthread_create(&thread_id, NULL, manipulaConexao, pclient) != 0) {
                perror("Erro ao criar thread");
                free(pclient);
                close(client_socket);
            } else{
                pthread_detach(thread_id);
            }
        }
    }
}

int main(){

    int servidorSocket = criaSocketServidor(porta, maximoConexoes);
    iniciaThreadConexao(servidorSocket);
    
    return 0;

}
