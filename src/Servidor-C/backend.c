#include "backend.h"
#include "rotas.h"
#include "manipulaRepositorio.h"
#include "buscaNoticias.h"
#include "frontend.h"

#define TAMANHO_BUFFER 100000

//pagina404(char *requisicao, int sock, struct respostaServidor *resposta);

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
        montaHTML(sock, resposta, "404");
        enviaResposta(sock, resposta);
        printf("Após a função pagina 404\n");
    }

    if (resposta != NULL) {
        printf("Limpando resposta...\n");
        free(resposta);
        resposta = NULL;
    }
}


void *manipulaConexao(void *cliente_socket) {
    int sock = *(int*)cliente_socket;
    free(cliente_socket);
    char requisicao[TAMANHO_BUFFER] = {0};

    printf("Lendo dados do load balancer...\n");
    ssize_t n = read(sock, requisicao, sizeof(requisicao) - 1);
    if (n <= 0) {
        perror("Erro ao ler dados do load balancer");
        close(sock);
        return NULL;
    }
    requisicao[n] = '\0';

    //printf("Requisição recebida (%zd bytes): %s\n", n, requisicao);

    manipulaRequisicao(requisicao, sock);

    printf("Fechando o socket\n");
    close(sock);
    return NULL;
}



int criaSocketServidor(int porta, int maximoConexoes) {
    int servidorSocket;
    struct sockaddr_in servidorEndereco;

    if ((servidorSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Falha ao criar socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(servidorSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("Falha ao configurar opções do socket");
        close(servidorSocket);
        exit(EXIT_FAILURE);
    }

    servidorEndereco.sin_family = AF_INET;
    servidorEndereco.sin_addr.s_addr = INADDR_ANY;
    servidorEndereco.sin_port = htons(porta);

    if (bind(servidorSocket, (struct sockaddr *)&servidorEndereco, sizeof(servidorEndereco)) < 0) {
        perror("Falha ao associar o socket");
        close(servidorSocket);
        exit(EXIT_FAILURE);
    }

    if (listen(servidorSocket, maximoConexoes) < 0) {
        perror("Falha ao colocar o socket em modo de escuta");
        close(servidorSocket);
        exit(EXIT_FAILURE);
    }

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

