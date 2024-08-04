#include "backend.h"
#include "rotas.h"
#include "manipulaRepositorio.h"
#include "buscaNoticias.h"
#include "frontend.h"



void manipulaRequisicao(char *requisicao, int sock) {
    char metodo[20];
    char rotaStr[1024];
    sscanf(requisicao, "%19s %1023s", metodo, rotaStr);
    printf("Metodo: %s\n", metodo);
    printf("rotaStr: %s\n", rotaStr);

    struct respostaServidor *resposta = malloc(sizeof(struct respostaServidor));
    
    if (resposta == NULL) {
        perror("Falha ao alocar memória para resposta");
        close(sock);
        return;
    }

    struct Rota *raiz = inicializaRaiz(); 
    
    printf("Inicializando rotas:\n");
    
    inicializaRotasArquivo(raiz);
    printf("Após inicializar rotas arquivo: \n");
    
    inicializaRotasBusca(raiz);
    printf("Após inicializar rotas busca: \n");

    printf("Lista de rotas: \n");
    listaTodasRotas(raiz);

    struct Rota* rotaEncontrada = buscaRota(raiz, rotaStr); 

    if (rotaEncontrada != NULL) {
        printf("Rota encontrada %s\n", rotaEncontrada->chave);
        
        rotaEncontrada->funcao(requisicao, sock, resposta);

        printf("Após chamar a função atrelada a rota: \n");
    } else {
        
        printf("Não encontrou a rota: %s\n", rotaStr);
        pagina404(requisicao, sock, resposta);
    }

    if (resposta != NULL) {
        printf("Limpando resposta?\n");
        free(resposta);
        resposta = NULL;
    }
}

void *manipulaConexao(void *cliente_socket){
    int sock = *(int*)cliente_socket;
    free(cliente_socket);
    char requisicao[4096] = {0}; //Incializar com zero para evitar lixo de memória
    
    //Ler a solicitação HTTP do Cliente
    ssize_t n = read(sock, requisicao, sizeof(requisicao)-1);
    printf("Leitura na manipula Conexão :  %s, %ld\n", requisicao, n);
    if (n <= 0) {
        // Erro de leitura ou conexão fechada pelo cliente
        close(sock);
        return NULL;
    } else if (n == 0) {
        // Conexão fechada pelo cliente
        printf("Conexão fechada pelo cliente\n");
        close(sock);
        return NULL;
    }
    requisicao[n] = '\0';  // Adicionar terminador de string

    printf("Requisicao, manipula conexão :  %s, %ld\n", requisicao, n);

    printf("Chamando a manipulaRequisição \n");
   
    manipulaRequisicao(requisicao,  sock);
    
    printf("Após a chamada de manipulaRequisição \n");

    printf("Fechando o socket\n");
    
    close(sock);
    printf("Socket fechado\n");
    return NULL;
}


int criaSocketServidor(int porta, int maximoConexoes){
    int servidorSocket;
    struct sockaddr_in servidorEndereco;

    // Cria o socket do servidor
    if ((servidorSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Falha ao criar o socket");
        exit(EXIT_FAILURE);
    }
   
    int opt = 1;
    if (setsockopt(servidorSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("Falha ao configurar o socket para reutilizar o endereço");
        close(servidorSocket);
        exit(EXIT_FAILURE);
    }

    // Configura o socket do servidor como não bloqueante
    int flags = fcntl(servidorSocket, F_GETFL, 0);
    fcntl(servidorSocket, F_SETFL, flags | O_NONBLOCK);

    // Define o endereço do servidor
    servidorEndereco.sin_family = AF_INET;
    servidorEndereco.sin_addr.s_addr = INADDR_ANY;
    servidorEndereco.sin_port = htons(porta);

    // Associa o socket ao endereço e porta
    if (bind(servidorSocket, (struct sockaddr *)&servidorEndereco, sizeof(servidorEndereco)) < 0) {
        perror("Falha ao associar o socket");
        exit(EXIT_FAILURE);
    }

    // Coloca o socket em modo de escuta
    if (listen(servidorSocket, maximoConexoes) < 0) {
        perror("Falha ao colocar o socket em modo de escuta");
        exit(EXIT_FAILURE);
    }

    printf("Servidor em execução na porta %d\n", porta);
    return servidorSocket;
}

int verificaErroSocket(int ver, const char *msg) {
    if (ver < 0) {
        perror(msg);
        exit(1);
    }
    return ver;
}

void sigpipe_handler(int signum) {
    // Estava tendo muito erro de SIGPIPE quando a pagina demorava carregar.
    printf("Capturado SIGPIPE: cliente fechou a conexão.\n");
}

