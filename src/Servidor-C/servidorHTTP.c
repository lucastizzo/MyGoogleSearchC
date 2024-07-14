#include "servidorHTTP.h"
#include "rotas.h"
#include "manipulaRepositorio.h"
#include "buscaNoticias.h"
#include "paginasPadrao.h"

// struct Rota* inicializaRaiz();
// struct Rota *rotaEncontrada; 
// void inicializaRotasPadrao();
// void inicializaRotasArquivo();
// void inicializaRotasBusca();

void manipulaRequisicao(char *requisicao, int sock) {
    char metodo[20];
    char rotaStr[1024];
    sscanf(requisicao, "%19s %1023s", metodo, rotaStr);
    printf("Metodo: %s\n", metodo);
    printf("rotaStr: %s\n", rotaStr);

    struct respostaServidor *resposta = malloc(sizeof(struct respostaServidor));
    struct Rota *raiz = inicializaRaiz(); 
    
    printf("Inicializando rotas:\n");
    
    inicializaRaiz();
    printf("Inicializou a raiz ,%s\n", raiz->chave);
    
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

    free(resposta);
}

void *manipulaConexao(void *cliente_socket){
    int sock = *(int*)cliente_socket;
    free(cliente_socket);
    char requisicao[4096];
    
    //Ler a solicitação HTTP do Cliente
    ssize_t n = read(sock, requisicao, sizeof(requisicao)-1);
    printf("Leitura na manipula Conexão :  %s, %ld\n", requisicao, n);
    if (n <= 0) {
        // Erro de leitura ou conexão fechada pelo cliente
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

int iniciaServidor(servidorHTTP *servidor, int porta, int maximoConexoes) {
    servidor->porta = porta;
    int opt = 1;
    
    int servidorSocket = socket(AF_INET, SOCK_STREAM, 0);
    verificaErroSocket(servidorSocket != -1, "Houve uma falha ao criar o socket");

    struct sockaddr_in servidorEndereco;
    memset(&servidorEndereco, 0, sizeof(servidorEndereco));
    servidorEndereco.sin_family = AF_INET;
    servidorEndereco.sin_port = htons(porta);
    servidorEndereco.sin_addr.s_addr = htonl(INADDR_ANY);

    if (setsockopt(servidorSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    verificaErroSocket(bind(servidorSocket, (struct sockaddr *)&servidorEndereco, sizeof(servidorEndereco)), "Falha na vinculação socket-servidor");
    verificaErroSocket(listen(servidorSocket, maximoConexoes), "Houve uma falha no Listen do servidor");

    servidor->socket = servidorSocket;
    printf("Servidor HTTP inicializado\nPorta: %d\n", servidor->porta);
    printf("Servidor socket: %d\n", servidorSocket);

    return servidorSocket;
}

int verificaErroSocket(int ver, const char *msg) {
    if (ver < 0) {
        perror(msg);
        exit(1);
    }
    return ver;
}

