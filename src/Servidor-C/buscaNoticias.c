#include "buscaNoticias.h"
#include "servidorHTTP.h"
#include "rotas.h"
#include "manipulaRepositorio.h"
#include "paginasPadrao.h"

void inicializaRotasBusca(struct Rota* raiz) {
    adicionaRota(raiz, "/resultadoPesquisa", resultadoPesquisahandler);
}

void resultadoPesquisahandler(char *requisicao, int sock, struct respostaServidor *resposta) {
    char termoPesquisa[1024]; // Aloca espaço para a string
    
    int numPesquisa;
    printf("antes de ler a requisição no resultaPesquisahandler\n");
    sscanf(requisicao, "%*s %1023s", termoPesquisa); // Lê a string para o array alocado
    printf("termo pesquisa: %s\n", termoPesquisa);

    char *pesquisa = strchr(termoPesquisa + 2, '/');
    if (pesquisa == NULL) {
        montaHTML(sock, resposta, "erroPesquisa");
        enviaResposta(sock, resposta);
        return;
    }

    int tamanho = strlen(pesquisa);
    
    if (pesquisa[tamanho - 1] == '?') {
        pesquisa[tamanho - 1] = '\0';
    }

    numPesquisa = atoi(pesquisa + 1);
    printf("numPesquisa: %d\n", numPesquisa);
    char *termoPesquisaPtr = strchr(pesquisa + 1, '/');
    
    if (termoPesquisaPtr != NULL) {
        printf("termoPesquisa: %s\n", termoPesquisaPtr); 
        resultadoPesquisa(requisicao, sock, resposta, numPesquisa, termoPesquisaPtr);
    } else {
        montaHTML(sock, resposta, "erroPesquisa");
        enviaResposta(sock, resposta);
    }

}

void resultadoPesquisa(char *requisicao, int sock, struct respostaServidor *resposta, int numPesquisa, char *termoChave) {
    char *htmlFim = "</body></html>";
    char *htmlResultado = malloc(900000 * sizeof(char));
    htmlResultado[0] = '\0';

    buscaNoticias *resultados = malloc(101 * sizeof(buscaNoticias)); 
    for (int i = 0; i < 101; i++) {
        resultados[i].nomeArquivo[0] = '\0';
        resultados[i].descricaoEncontrada[0] = '\0';
    }
    resultados[0].total = 0;

    printf("buscando noticias.\n");
    busca(numPesquisa, termoChave, resultados);
    
    printf("Criando HTML's: \n");
    strcat(htmlResultado, "<ul>");
    for (int i = 0; strlen(resultados[i].nomeArquivo) > 0; i++) {
        char htmlItem[10000];
        sprintf(htmlItem, "<li><a href=\"%s\">%s</a> - %s</li>", resultados[i].nomeArquivo, resultados[i].nomeArquivo, resultados[i].descricaoEncontrada);
        strcat(htmlResultado, htmlItem);
    }
    strcat(htmlResultado,"</ul>");

    if (resultados[0].total > 0){
        int numBotoes = (resultados[0].total + 99) / 100;
        
        strcat(htmlResultado, "<div class='pagination'>");
        
        for (int j = 0; j < numBotoes; j++){
            char botaoHTML[200];
            sprintf(botaoHTML, "<a href='/buscaNoticia/%d/%s' class='btn'>%d</a> ", j, termoChave, j);
            strcat(htmlResultado, botaoHTML);
        }
        strcat(htmlResultado, "</div>");
    }

    strcat(htmlResultado, htmlFim);
    printf("Montando HTML's\n");
    montaHTML(sock, resposta, "resultadoPesquisa");
    strcat(resposta->conteudo, htmlResultado);
    
    enviaResposta(sock, resposta);
    
    printf("Limpando: \n");
    free(htmlResultado);
    free(resultados);
    
}

void busca(int numPesquisa, const char *termoChave, buscaNoticias resultados[]) {
    printf("Na chamada\n");
    DIR* dir;
    struct dirent *ent;
    FILE* arquivo;

    int numResultados = 0;
    int resultadoTotal = 0;

    if (termoChave != NULL){
        printf("Antes de abrir repositorioNoticias, %s\n", repositorioNoticias);
        if ((dir = opendir(repositorioNoticias)) != NULL){
            printf("Abriu o repositorioNoticias\n");

            while ((ent = readdir(dir)) != NULL){
                if (ent->d_type == DT_REG) { // Verifica se é um arquivo regular
                    char caminhoArquivo[300];
                    sprintf(caminhoArquivo, "%s/%s", repositorioNoticias, ent->d_name);

                    arquivo = fopen(caminhoArquivo, "r");
                    if (arquivo == NULL){
                        printf("Não foi possível abrir o arquivo %s\n", caminhoArquivo);
                        continue;
                    }

                    char linha[1024];
                    while (fgets(linha, sizeof(linha), arquivo)){
                        
                        if (strstr(linha, termoChave) != NULL){
                            numResultados++;
                            printf("Resultado encontrado:  %s\n", linha);
                            if (numResultados > numPesquisa && numResultados <= numPesquisa + 100){
                                strcpy(resultados[numResultados - numPesquisa - 1].nomeArquivo, caminhoArquivo);
                                strcpy(resultados[numResultados - numPesquisa - 1].descricaoEncontrada, linha);
                                resultadoTotal++;
                                printf("resultado total:%d\n", resultadoTotal);
                            }
                        }
                    }
                    fclose(arquivo);
                }
            }
            closedir(dir);
        } else {
            strcpy(resultados[0].nomeArquivo, "Não foi possível abrir o diretório");
        }
    } else {
        strcpy(resultados[0].nomeArquivo, "Não é possível pesquisar por termo nulo!");
    }

    if (numResultados == 0) {
        strcpy(resultados[0].nomeArquivo, "Nenhum resultado encontrado.");
        strcpy(resultados[0].descricaoEncontrada, "");
        resultados[0].total = 0;
    } else {
        resultados[0].total = numResultados;
        if (resultadoTotal > 0) {
            sprintf(resultados[resultadoTotal].nomeArquivo, "Total de ocorrências encontradas: %d", numResultados);
            strcpy(resultados[resultadoTotal].descricaoEncontrada, "%s", termoChave);
        }
    }
}
