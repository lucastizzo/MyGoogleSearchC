#include "manipulaRepositorio.h"
#include "servidorHTTP.h"
#include "rotas.h"
#include "paginasPadrao.h"


arquivosIndexados arquivosIndexadosVet[MAX_ARQUIVOS];
int numArquivosIndexados = 0;

void inicializaRotasArquivo(struct Rota* raiz){
        adicionaRota(raiz, "/removerArquivo", removerArquivoHandler);
    //    adicionaRota(raiz, "/mostrarArquivoIndexado", mostrarArquivoIndexado);
    //    adicionaRota(raiz, "/importarArquivo", importarArquivo);
        adicionaRota(raiz, "/listarArquivos", listarArquivos);
}
//Definir as rotas com base nas funções 

//Mostrar o HTML das funções 
void mostraArquivoImportado(struct respostaServidor *resposta){
    char *html = "<html><head><title>Arquivo Importado</title></head><body><h1>Arquivo Importado com sucesso!</h1></body></html>";
    strcpy(resposta->conteudo, html);
   // resposta->tamanhoResposta = strlen(html);
}

// Função para remover algum arquivo 
void removerArquivo(char *requisicao, int sock, struct respostaServidor *resposta,  char *nomeArquivo){
    
    char linha[1024]; 
    char caminhoCompleto[300]; // Controla o tamanho do buffer para evitar inserções muito grande 
    strcpy(caminhoCompleto, repositorioNoticias); // Copia o diretorio de repositorio para o buffer 
    strcat(caminhoCompleto, nomeArquivo); // Pega o nome do arquivo a partir do parametro e concatena com o diretorio padrão 

    if (remove(caminhoCompleto) == 0){ 
        montaHTML(sock, resposta, "arquivoRemovido");
        printf("Arquivo %s removido com sucesso\n", nomeArquivo);
        snprintf(linha, sizeof(linha), "<br>Arquivo: %s removido com sucesso!</br>", nomeArquivo);
        strcat(resposta->conteudo, linha);
        strcat(resposta->conteudo, "<button type='button' onclick='location.href=\"/listarArquivos\"'>Listar arquivos</button></body></html>");
    
    }else{
        montaHTML(sock, resposta, "erroRemoverArquivo");
        printf("Houve um erro ao remover o arquivo %s\n", nomeArquivo);
    }

    enviaResposta(sock, resposta);
}         


void removerArquivoHandler(char *requisicao, int sock, struct respostaServidor *resposta){
    char *arquivoRemover;
    sscanf(requisicao, "%*s %1023s", arquivoRemover);
    printf("Arquivo a ser removido: chamada para remover %s\n", arquivoRemover);
    
    arquivoRemover = strchr(arquivoRemover + 2, '/');

    printf("Arquivo a ser removido: %s\n", arquivoRemover);
    
    removerArquivo(requisicao, sock, resposta, arquivoRemover);    
}    




void importarArquivo(int sockfd, const char *arquivoImporta, char arquivoDestino[MAX_BUFFER_TAM], const char *descricaoIndex, const char *indexacao){
// Abre o arquivo para leitura
    FILE *arquivo = fopen(arquivoImporta, "r");
    snprintf(arquivoDestino, MAX_BUFFER_TAM, "%s_recebido", arquivoImporta);
    FILE *arquivoDestinoPtr = fopen(arquivoDestino, "wb");

    if (arquivo == NULL){
        printf("Erro ao abrir o arquivo %s\n", arquivoImporta);
        fclose(arquivo);
        return;
    }

    ssize_t bytesRecebidos;
    char buffer[MAX_BUFFER_TAM];


    while ((bytesRecebidos = recv(sockfd, buffer, MAX_BUFFER_TAM, 0)) > 0){
       if (fwrite(buffer, 1, bytesRecebidos, arquivoDestinoPtr) != bytesRecebidos){
            printf("Erro ao escrever no arquivo %s\n", arquivoDestino);
            fclose(arquivoDestinoPtr);
            return;
       }
    }

    if (bytesRecebidos == 0){
        printf("Conexão encerrada pelo cliente\n");
    } else if (bytesRecebidos == -1){
        perror("Erro ao receber dados do cliente");
    }

    fclose(arquivoDestinoPtr);

}

void listarArquivos(char *requisicao, int sock, struct respostaServidor *resposta){
    DIR *dp;
    struct dirent *ep;

    montaHTML(sock, resposta, "listarArquivos");
    printf("%s\n", repositorioNoticias);
    dp = opendir(repositorioNoticias);

    if (dp != NULL) {
        
        strcat(resposta->conteudo, "<ul>\n");
        
        while ((ep = readdir(dp)) != NULL) {
            if (ep->d_type == DT_REG) {
                char linha[1024]; 
                snprintf(linha, sizeof(linha), "<li>%s - Descrição do arquivo %s <a href='/removerArquivo/%s'>Remover</a></li>\n", ep->d_name, ep->d_name, ep->d_name);
                strcat(resposta->conteudo, linha);
            }
        }

        strcat(resposta->conteudo, "</ul>\n</body>\n</html>");
        closedir(dp);
        enviaResposta(sock, resposta);
    } else {
        if (dp == NULL) perror("Erro ao abrir o diretório!");
    }
}
