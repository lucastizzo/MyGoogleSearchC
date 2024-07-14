#include "paginasPadrao.h"
#include "buscaNoticias.h"
#include "servidorHTTP.h"
#include "rotas.h"
#include "manipulaRepositorio.h"


void montaHTML(int sock, struct respostaServidor *resposta, const char* nomeArquivo) {
    //Inicializacao de header e conteudo
    memset(resposta->header, 0, sizeof(resposta->header));
    memset(resposta->conteudo, 0, sizeof(resposta->conteudo));
        
    // Cabeçalho HTTP
    time_t now = time(0);
    struct tm *gmt = gmtime(&now);
    char date[35];
    strftime(date, sizeof(date), "Date: %a, %d %b %Y %H:%M:%S %Z\r\n", gmt);
    snprintf(resposta->header, sizeof(resposta->header), "HTTP/1.1 200 OK\r\n%sContent-Type: text/html; charset=UTF-8\r\n\r\n", date);
    
    // Abrir o arquivo HTML especificado e montar o conteudo 
    char caminhoCompleto[1024];
    snprintf(caminhoCompleto, sizeof(caminhoCompleto), "/home/tizzo/projeto-SD/MygoogleSearchC/src/Servidor-C/html/%s.html", nomeArquivo);
    

    FILE *fd = fopen(caminhoCompleto, "r");
    if (fd == NULL) {
        snprintf(resposta->header, sizeof(resposta->header), "HTTP/1.1 500 Internal Server Error\r\n%sContent-Type: text/plain\r\n\r\n", date);
        snprintf(resposta->conteudo, sizeof(resposta->conteudo), "Erro ao abrir o arquivo HTML.\n");
        enviaResposta(sock,resposta);
        return;
    }
    
    size_t bytes_lidos = fread(resposta->conteudo, 1, sizeof(resposta->conteudo) - 1, fd);     
    resposta->conteudo[bytes_lidos] = '\0';
    fclose(fd);

}


void paginaInicial(char *requisicao, int sock, struct respostaServidor *resposta) {
    
    montaHTML(sock, resposta, "paginaInicial");
    enviaResposta(sock, resposta);
}

void pagina404(char *requisicao, int sock, struct respostaServidor *resposta) {
     
     montaHTML(sock, resposta, "404");
     enviaResposta(sock, resposta);
}

void enviaResposta(int sock, struct respostaServidor *resposta){
    write(sock, resposta->header, strlen(resposta->header));
    write(sock, resposta->conteudo, strlen(resposta->conteudo));
}