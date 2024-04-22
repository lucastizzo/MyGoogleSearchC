#include "servidorHTTP.h"
#include "buscaNoticias.h"
#include "manipulaRepositorio.h"
#include "rotas.h"
#include <pthread.h>

#define porta 8080
#define tamanhoBuffer 4096
#define maximoConexoes 10000000
 
char *repositorioNoticias = "mnt/c/home/tizzo/projeto-SD/repositorio-noticias";


int main(){
    // Iniciar servidor HTTP
    servidorHTTP meuServidor;
    int servidorSocket, novoSocket, c, *socketNovo;
    struct sockaddr_in servidor, cliente;
    
    servidorSocket = iniciaServidor(&meuServidor, porta, maximoConexoes);
    // Registro de Rotas
     while(1){
        printf("Aguardando por conexões...\n");
        char mensagemCliente[4096] = "";
        printf("%s\n", mensagemCliente);
     }

    return 0;
}
