#include "manipulaRepositorio.h"
#include "backend.h"
#include "rotas.h"
#include "frontend.h"


arquivosIndexados arquivosIndexadosVet[MAX_ARQUIVOS];
int numArquivosIndexados = 0;

void inicializaRotasArquivo(struct Rota* raiz){
        adicionaRota(raiz, "/removerArquivo", removerArquivoHandler);
        adicionaRota(raiz, "/importarArquivo", importarArquivo);
        adicionaRota(raiz, "/listarArquivos", listarArquivos);
}
//Definir as rotas com base nas funções 



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


void importarArquivo(char *requisicao, int sock, struct respostaServidor *resposta) {
    // Encontrar o boundary no cabeçalho
    const char *boundary = strstr(requisicao, "boundary=");
    if (boundary == NULL) {
        const char *errorPage = "<html><body>Erro: Boundary não encontrado.</body></html>";
        write(sock, errorPage, strlen(errorPage));
        return;
    }

    boundary += 9; // Pular "boundary="
    char boundaryDelimiter[256];
    snprintf(boundaryDelimiter, sizeof(boundaryDelimiter), "--%s", boundary);

    // Encontrar o início do conteúdo após os cabeçalhos
    char *contentStart = strstr(requisicao, "\r\n\r\n");
    if (contentStart == NULL) {
        printf("Não encontrou o início do conteúdo, contentStart == NULL\n");
        montaHTML(sock, resposta, "arquivoNaoEncontrado");
        enviaResposta(sock, resposta);
        return;
    }

    contentStart += 4; // Pular "\r\n\r\n"

    // Encontrar o início do cabeçalho do arquivo
    char *headerStart = strstr(contentStart, "Content-Disposition: form-data;");
    if (headerStart == NULL) {
        printf("Não encontrou o cabeçalho do arquivo, headerStart == NULL\n");
        montaHTML(sock, resposta, "erroAbrirArquivo");
        enviaResposta(sock, resposta);
        return;
    }

    // Extrair o nome do arquivo
    char nomeArquivo[256] = {0};
    sscanf(headerStart, "Content-Disposition: form-data; name=\"file\"; filename=\"%255[^\"]\"", nomeArquivo);

    // Encontrar o início dos dados do arquivo
    char *fileStart = strstr(headerStart, "\r\n\r\n");
    if (fileStart == NULL) {
        printf("Não conseguiu encontrar o início dos dados do arquivo, fileStart == NULL\n");
        montaHTML(sock, resposta, "erroAbrirArquivo");
        enviaResposta(sock, resposta);
        return;
    }

    fileStart += 4; // Pular "\r\n\r\n"

    // Encontrar o fim dos dados do arquivo
    char *fileEnd = strstr(fileStart, boundary);
    if (fileEnd == NULL) {
        // Se não encontrar o boundary, tentar encontrar o final da parte
        fileEnd = strstr(fileStart, "\r\n--");
    }

    if (fileEnd == NULL) {
        printf("Não conseguiu encontrar o fim do arquivo, fileEnd == NULL\n");
        montaHTML(sock, resposta, "erroAbrirArquivo");
        enviaResposta(sock, resposta);
        return;
    }

    // Ajustar o tamanho do arquivo
    size_t fileSize = fileEnd - fileStart;
    if (fileSize == 0) {
        printf("Tamanho do arquivo é zero.\n");
        montaHTML(sock, resposta, "erroAbrirArquivo");
        enviaResposta(sock, resposta);
        return;
    }

    // Salvar o arquivo no diretório de uploads
    char caminhoArquivo[512];
    snprintf(caminhoArquivo, sizeof(caminhoArquivo), "%s/%s", repositorioNoticias, nomeArquivo);

    FILE *fp = fopen(caminhoArquivo, "wb");
    if (fp == NULL) {
        printf("Não conseguiu abrir o arquivo para escrita, fp == NULL\n");
        montaHTML(sock, resposta, "erroAbrirArquivo");
        enviaResposta(sock, resposta);
        return;
    }

    fwrite(fileStart, 1, fileSize, fp);
    fclose(fp);

    // Mostrar sucesso
    mostraArquivoImportado(resposta, sock, nomeArquivo);
}


void mostraArquivoImportado(struct respostaServidor *resposta, int sock,char *nomeArquivo) {
    
    montaHTML(sock, resposta, "mostraArquivoImportado");
    
    char linha[1024];
    snprintf(linha, sizeof(linha), "<h1>Arquivo %s importado com sucesso!</h1>\n", nomeArquivo);
    strcat(resposta->conteudo, linha);
    strcat(resposta->conteudo, "<button type='button' onclick=\"window.location.href='/'\">Voltar</button>");
    strcat(resposta->conteudo, "<button type='button' onclick=\"location.href='/listarArquivos'\">Listar arquivos</button>");
    strcat(resposta->conteudo, "</body></html>");
    
    enviaResposta(sock, resposta);
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
