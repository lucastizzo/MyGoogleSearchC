#include "servidorHTTP.h"
#include "buscaNoticias.h"
#include "manipulaRepositorio.h"
#include "rotas.h"
#include <pthread.h>

#define porta 8080
#define tamanhoBuffer 4096
#define maximoConexoes 10000000
 
char *nomeServidor = "MyGoogleSearchC";
char *repositorioNoticias = "/mnt/c/home/tizzo/projeto-SD/repositorio-noticias";


int main(){
    // limpar a tela
    system("clear");

    printf("\n Inicialializando o servidor %s...\n", nomeServidor);
    
    // Iniciar servidor HTTP
    servidorHTTP meuServidor;
    int servidorSocket, novoSocket, c, *socketNovo;
    struct sockaddr_in servidor, cliente;
    
    servidorSocket = iniciaServidor(&meuServidor, porta, maximoConexoes);
    // Registro de Rotas
     while(1){
       int client_socket = accept(servidorSocket, NULL, NULL);
       
       int* pclient  = malloc(sizeof(int));
           *pclient = client_socket;

        pthread_t thread_id;
        printf("chamando a thread para manipulaConexao\n");
        pthread_create(&thread_id, NULL, manipulaConexao, pclient);
        printf("Após a chamada da thread para manipulaConexao \n");
        pthread_detach(thread_id);
     }

    return 0;
}
