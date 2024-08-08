#include "main.h"
#include "backend.h"
#include "buscaNoticias.h"
#include "manipulaRepositorio.h"
#include "rotas.h"
#include <locale.h>
#include <pthread.h>

#define PORTA_BACKEND 8081 // Ajuste a porta conforme necessário
#define TAMANHO_BUFFER 4096
#define MAXIMO_CONEXOES 10000000

int servidorAtual = 0;

// Definindo o nome do servidor e o repositório de notícias
char *nomeServidor = "MyGoogleSearchC";
char *repositorioNoticias = "/home/tizzo/projeto-SD/repositorio-noticias/";

void iniciaThreadConexao(int servidorSocket) {
    fd_set readfds;
    int max_sd, sd, activity;

    printf("Iniciando thread de conexão do socket %d\n", servidorSocket);

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(servidorSocket, &readfds);
        max_sd = servidorSocket;

        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            printf("Erro no select");
        }

        if (FD_ISSET(servidorSocket, &readfds)) {
            int client_socket = accept(servidorSocket, NULL, NULL);

            if (client_socket < 0) {
                perror("Erro ao aceitar conexão");
                continue;
            }

            printf("Nova conexão aceita do load balancer...\n");
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
            } else {
                pthread_detach(thread_id);
            }
        }
    }
}

int main() {
    int servidorSocket = criaSocketServidor(PORTA_BACKEND, MAXIMO_CONEXOES);
    printf("Backend em execução na porta %d\n", PORTA_BACKEND);
    iniciaThreadConexao(servidorSocket);
    return 0;
}

