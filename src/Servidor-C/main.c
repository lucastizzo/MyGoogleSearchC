#include "main.h"
#include "servidorHTTP.h"
#include "buscaNoticias.h"
#include "manipulaRepositorio.h"
#include "rotas.h"
#include <locale.h>
#include <pthread.h>

#define porta 8080
#define tamanhoBuffer 4096
#define maximoConexoes 10000000
 
// Definindo o nome do servidor e o repositório de notícias
char *nomeServidor = "MyGoogleSearchC";
char *repositorioNoticias = "/home/tizzo/projeto-SD/repositorio-noticias/";

void iniciaThreadConexao(int servidorSocket) {
    
    while (1) {
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
            continue;
        }
        printf("Após a chamada da thread para manipulaConexao \n");
        pthread_detach(thread_id);
    }
}


int main(){

    // limpar a tela
    system("clear");
    setlocale(LC_ALL, "pt_BR.UTF-8");
    
    printf("\n Inicialializando o servidor %s...\n", nomeServidor);
    
    // Iniciar servidor HTTP
    servidorHTTP meuServidor;
    int servidorSocket, novoSocket, c, *socketNovo;
    struct sockaddr_in servidor, cliente;

    //Inicia Rotas
    servidorSocket = iniciaServidor(&meuServidor, porta, maximoConexoes);
    
    if (servidorSocket < 0) {
        perror("Erro ao iniciar servidor");
        return 1;
    }

    iniciaThreadConexao(servidorSocket);

    return 0;
}
