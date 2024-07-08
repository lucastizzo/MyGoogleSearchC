#include "servidorHTTP.h"
#include "rotas.h"
#include "manipulaRepositorio.h"
#include  "buscaNoticias.h"

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
struct Rota *rotaEncontrada; 
void inicializaRotasArquivo();
void inicializaRotasBusca();

void manipulaRequisicao(char *requisicao, int sock) {
    // Analisar a primeira linha da requisição (método e rota)
    char metodo[20];
    char rota[1024];
    printf("Requisicao, manipula requisicao :  %s\n", requisicao);
    sscanf(requisicao, "%19s %1023s", metodo, rota);
    printf("Metodo %s, Rota %s\n", metodo, rota);
    
    printf("Antes de chamar a struct resposta servidor\n");
    // Responder a solicitação do cliente  
    struct respostaServidor *resposta = malloc(sizeof(struct respostaServidor));
    

    printf("Após chamar a struct resposta servidor\n");
    char status[] = "HTTP/1.1 200 OK\r\n";
    
    printf("Após chamar char status[]\n");

    // Gerar a data e a hora atuais
    time_t now = time(0);
    struct tm *gmt = gmtime(&now);
    char date[35];
    
    strftime(date, sizeof(date), "Date: %a, %d %b %Y %H:%M:%S %Z\r\n", gmt);

   //  printf("Após strftime\n");
    
    // printf("Rota Encontrada: Chave = %s, Valor = %s\n", rotaEncontrada->chave, rotaEncontrada->valor);
    // Buscar a rota na arvore binária definida em rotas.c
   
    printf("Antes de chamar da struct inicializa raiz\n");
    struct Rota *raiz = inicializaRaiz(); 
    // Função que inicializa a raiz da árvore de rotas
    
    printf("Antes de chamar os metodos de rota\n");

    //Inicia todas as rotas
    inicializaRotasArquivo(); 
    printf("Inicializou Rotas arquivos\n");
    inicializaRotasBusca(); 
    printf("Inicializou Rotas bucas\n");


    printf("Depois de inicializar a raiz e inicializar rotas arquivo e rotas busca;\n");

    //Formata a rota 
    char *rotaDividida = strtok(rota, "=");
    // Se a rota for encontrada, chamamos a função correspondente.
    if (strcmp(rota, "/") == 0 || strcmp(rota, "") == 0 || rotaDividida == NULL) {
        // Se a rota for "/" ou vazia, servir o arquivo index.html
        printf("Deve carregar o pagina.html\n");
        FILE *fd = fopen("/home/tizzo/projeto-SD/MygoogleSearchC/src/Servidor-C/pagina.html", "r");
        if (fd == NULL) {
            perror("Erro ao abrir o arquivo html");
            return;
        }
        char html_content[4096];
        size_t bytes_lidos = fread(html_content, 1, sizeof(html_content) - 1, fd);
        html_content[bytes_lidos] = '\0';
        fclose(fd);
        // Enviar cabeçalho HTTP
        int tamanho = snprintf(resposta->resposta, sizeof(resposta->resposta), "%s%s%s%s", status, date,
                                                  "Content-Type: text/html; charset=UTF-8\r\n\r\n", html_content);
        
        if (tamanho >= sizeof(resposta->resposta)) {
                // A resposta é muito grande para caber na matriz resposta->resposta
                perror("Resposta muito grande");
                return;
            }
        
        resposta->tamanhoResposta = tamanho;
        
        write(sock, resposta->resposta, strlen(resposta->resposta)); // Enviar o tamanho real do arquivo
    } else {
        printf("Rota não encontrada\n");
        char not_found_header[] = "HTTP/1.1 404 Not Found\r\n";
        // write(sock, not_found_header, strlen(not_found_header));

        // Se a rota não for encontrada, retornar 404 Not Found
        FILE *fe = fopen("/home/tizzo/projeto-SD/MygoogleSearchC/src/Servidor-C/404.html", "r");
        
        if (fe == NULL) {
            perror("Erro ao abrir o arquivo html\n");
            return;
        } 

        char html_erro[4096];
        size_t bytes_erro = fread(html_erro, 1, sizeof(html_erro) - 1, fe);
        html_erro[bytes_erro] = '\0';
        fclose(fe);
        int tamanho_erro = snprintf(resposta->resposta, sizeof(resposta->resposta), "%s%s%s%s", status, date,
                                                  "Content-Type: text/html; charset=UTF-8\r\n\r\n", html_erro);

        resposta->tamanhoResposta = tamanho_erro;

        write(sock, resposta->resposta, strlen(resposta->resposta)); // Enviar o tamanho real do arquivo
  
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
    // Analisar a primeira linha da requisição (método e rota)
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

