#include "servidorHTTP.h"


// Bibliotecas incluidas: 
//#include <netinet/in.h>
 //include <stdio.h>
// #include <sys/socket.h>
// //#include <unistd.h>    
//#include <pthread.h>   
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>
//#include <semaphore.h>


// Structs definidas
/*typedef struct servidorEndereco { 
    short sin_family; // Família de Endereços 
    unsigned short sin_port; // Numero da porta  - convertido para ordem de bytes da rede
    struct in_addr sin_addr;  // Endereço IP do servidor
    char sin_zero[8]; // Prenchimento para manter o mesmo tamanho da estrutura sckaddr_in que queremos copiar
};

//Definindo a estrutura do servidor HTTP 
typedef struct{
    int porta;
    int socket;
} servidorHTTP;
*/

// Função de inicialização de servido HTTP, recebendo como parametro o servidor HTTP, a porta e o numero maximo de conexões
int iniciaServidor(servidorHTTP * servidorHTTP, int porta, int maximoConexoes) {
	// Definir a porta do servidor
    servidorHTTP->porta = porta;

    // Criar um socket para o servidor, pasando como parametro a familia de endereços  e o tipo de socket no caso STREAM de protocolo TCP,
    // O ultimo parametro 0 é passado para definir o tipo de protocolo a ser usado pelo socket, o 0 nesse caso indica o mais adequado com base nos dois parametros anteriores no caso TCP/IP
	int servidorSocket = socket(AF_INET, SOCK_STREAM, 0);

    // Verifica se o socket iniciou corretamente
    verificaErroSocket(servidorSocket != -1, "Houve uma falha ao criar o socket");

    // Configurar o endereço do servidor 
	struct sockaddr_in  servidorEndereco;  
    memset(&servidorEndereco, 0, sizeof(servidorEndereco));
	servidorEndereco.sin_family = AF_INET; // Definir como familia de endereços o IPv4
	servidorEndereco.sin_port = htons(porta);  //  Definir a porta do servidor (convertida em ordem de bytes da rede)
	servidorEndereco.sin_addr.s_addr = INADDR_ANY; // Definir endereço IP do servidor - qualquer disponivel no nosso caso localhost

	// Associa o socket criado ao endereço e porta do servidorm, usamos a função check para dar mensagem de retorno caso falhe
    verificaErroSocket(bind(servidorSocket, (struct sockaddr  *) &servidorEndereco , sizeof(servidorEndereco )), "Falha na vinculação socket-servidor");
    
    // Colocar o socket em modo de escuta para conexões de entrada, com fila de tamanho 1000 (nosso máximo para testes)
	verificaErroSocket(listen(servidorSocket, maximoConexoes), "Houve uma falha no Listen do servidor");

	servidorHTTP->socket = servidorSocket;
    // Exibe uma mensagem para cada inicilialização indicando a a porta a qual foi conectada 
	printf("Servidor HTTP inicializado\nPorta: %d\n", servidorHTTP->porta);

    return servidorSocket;

}

int verificaErroSocket(int ver, const char *msg){ // Como a maioria dos erros relacionados as funções de socket são muito parecidos podemos usar uma função de verificação
    if(ver == socketErro){
        perror(msg);
        exit(1);
    }
    return ver;
}


