#include "servidorHTTP.h"
#include "rotas.h"


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
struct Rota* inicializaRaiz();

void *manipulaConexao(void *cliente_socket){
    int sock = *(int*)cliente_socket;
    char requisicao[4096];

        //Ler a solicitação HTTP do Cliente
    ssize_t n = read(sock, requisicao, sizeof(requisicao)-1);
    if (n <= 0) {
        // Erro de leitura ou conexão fechada pelo cliente
        close(sock);
        free(cliente_socket);
        return NULL;
    }
    requisicao[n] = '\0';  // Adicionar terminador de string

    // Analisar a primeira linha da requisição (método e rota)
    char metodo[20];
    char rota[1024];
    sscanf(requisicao, "%s %s", metodo, rota);


    //Responder a solicitação do cliente  
    struct respostaServidor resposta;
    // Enviar a resposta HTTP de volta ao cliente
    write(sock, resposta.resposta, sizeof(resposta.resposta));
    
    //Buscar a rota na arvore binária definida em rotas.c
    struct Rota *raiz = inicializaRaiz(); // Função que inicializa a raiz da árvore de rotas
    struct Rota *rotaEncontrada = buscaRota(raiz, requisicao);

    //Buscar a rota na arvore binária definida em rotas.c

    //Se a rota for encontrada, chamamos a função correspondente.
    if (strcmp(requisicao, "/") == 0 || strcmp(requisicao, "") == 0) {
        // Se a rota for "/" ou vazia, servir o arquivo index.html,
        printf("Deve carregar o pagina.html\n");
        int fd = open("pagina.html", O_RDONLY);
        struct stat st;
        fstat(fd, &st);
        sendfile(sock, fd, NULL, st.st_size);  // Enviar o tamanho real do arquivo
        close(fd);

    }else if (rota != NULL){

            printf("Deve carregar o pagina.html\n");
            int fd = open("pagina.html", O_RDONLY);

            
            if (fd == -1) {
                perror("Erro ao abrir o arquivo html");
            }

            struct stat st;
            fstat(fd, &st);

            // Enviar cabeçalho HTTP
            char header[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
            write(sock, header, sizeof(header) - 1);

            sendfile(sock, fd, NULL, st.st_size);  // Enviar o tamanho real do arquivo
            close(fd);

        } else {
                printf("Caiu em rota = a nulo\n");
        }

    //Fechar a conexão com o cliente
    close(sock);
    free(cliente_socket);

    return NULL;

}

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
	servidorEndereco.sin_addr.s_addr = htonl(INADDR_ANY); // Definir endereço IP do servidor - qualquer disponivel no nosso caso localhost

    int opt = 1;
    setsockopt(servidorSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
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


