#include "frontend.h"
#include "buscaNoticias.h"
#include "backend.h"
#include "rotas.h"
#include "manipulaRepositorio.h"


void montaHTML(int sock, struct respostaServidor *resposta, const char* nomeArquivo) {
    //Inicializacao de header e conteudo
    printf("Inicio montaHTML");

    memset(resposta->header, 0, sizeof(resposta->header));
    printf("memset header, montando HTML\n");
    memset(resposta->conteudo, 0, sizeof(resposta->conteudo));
    printf("memset conteudo, montando HTML\n");
        
    // Cabeçalho HTTP
    time_t now = time(0);
    struct tm *gmt = gmtime(&now);
    char date[35];
    strftime(date, sizeof(date), "Date: %a, %d %b %Y %H:%M:%S %Z\r\n", gmt);
    snprintf(resposta->header, sizeof(resposta->header), "HTTP/1.1 200 OK\r\n%sContent-Type: text/html; charset=UTF-8\r\n\r\n", date);
    printf("snprintf header\n");
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
    printf("fread resposta\n");
    resposta->conteudo[bytes_lidos] = '\0';
    fclose(fd);

}


void paginaInicial(char *requisicao,int sock, struct respostaServidor *resposta) {
    
    montaHTML(sock, resposta, "paginaInicial");
    enviaResposta(sock, resposta);
}

void pagina404(int sock, struct respostaServidor *resposta) {
     printf("chamada de pagina404\n");
     montaHTML(sock, resposta, "404");
     enviaResposta(sock, resposta);
     printf("resposta enviada\n");
}

void enviaResposta(int sock, struct respostaServidor *resposta){
    
    printf("Enviando resposta: \n");

    ssize_t bytes_written = write(sock, resposta->header, strlen(resposta->header));
    if (bytes_written == -1) {
        perror("Erro ao enviar header");
        return;
    }

    printf("Enviando resposta, header: \n");

    bytes_written = write(sock, resposta->conteudo, strlen(resposta->conteudo));
    if (bytes_written == -1) {
        perror("Erro ao enviar conteúdo");
        return;
    }

    printf("Enviando resposta, conteudo: \n");
}


void url_decode(char *src, char *dest){
    char a, b;
    while (*src) {
        if ((*src == '%') && ((a = src[1]) && (b = src[2])) && (isxdigit(a) && isxdigit(b))) {
            if (a >= 'a') a -= 'a' - 'A';
            if (a >= 'A') a -= ('A' - 10);
            else a -= '0';
            if (b >= 'a') b -= 'a' - 'A';
            if (b >= 'A') b -= ('A' - 10);
            else b -= '0';
            *dest++ = 16 * a + b;
            src += 3;
        } else if (*src == '+') {
            *dest++ = ' ';
            src++;
        } else {
            *dest++ = *src++;
        }
    }
    *dest = '\0';
}