#include "manipulaRepositorio.h"
#include "servidorHTTP.h"
#include "rotas.h"

// Inicializar a raiz da árvore de rotas


// Bibliotecas definidas 
/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Variaveis definidas 

#define MAX_ARQUIVOS 1000
#define MAX_RESULTADOS 1000000
#define MAX_TERMO_CHAVE_TAM 100


// Estrutura do resultado de uma pesquisa
typedef struct {
    char nomeArquivo[100];
    char descricao[MAX_DESCRICAO_TAM];
} buscaNoticia;
*/
//Vetor arquivos indexados
arquivosIndexados arquivosIndexadosVet[MAX_ARQUIVOS];
int numArquivosIndexados = 0;

void inicializaRotasArquivo(){
struct Rota *raiz = NULL;
adicionaRota(raiz, "/removerArquivo", "removerArquivo");
adicionaRota(raiz, "/mostrarArquivoIndexado", "mostrarArquivoIndexado");
adicionaRota(raiz, "/importarArquivo", "importarArquivo");
adicionaRota(raiz, "/listarArquivos", "listarArquivos");
}
//Definir as rotas com base nas funções 

//Mostrar o HTML das funções 
void mostraArquivoImportado(struct respostaServidor *resposta){
    char *html = "<html><head><title>Arquivo Importado</title></head><body><h1>Arquivo Importado com sucesso!</h1></body></html>";
    strcpy(resposta->conteudoResposta, html);
    resposta->tamanhoResposta = strlen(html);

}

// Função para indexar um novo arquivo. 
void indexarArquivo(const char *nomeArquivo, const char *descricaoIndex){

    
    if(numArquivosIndexados < MAX_ARQUIVOS){
        strcpy(arquivosIndexadosVet[numArquivosIndexados].nomeArquivo, nomeArquivo);
        strcpy(arquivosIndexadosVet[numArquivosIndexados].descricaoIndex, descricaoIndex);
        numArquivosIndexados++;
    }
    if (numArquivosIndexados  > 0) {
        printf("Arquivo indexado com sucesso!");
    }
}

// Função para remover algum arquivo 
void removerArquivo(const char *nomeArquivo){
    char caminhoCompleto[300]; // Controla o tamanho do buffer para evitar inserções muito grande 
    strcpy(caminhoCompleto, repositorioNoticias); // Copia o diretorio de repositorio para o buffer 
    strcat(caminhoCompleto, nomeArquivo); // Pega o nome do arquivo a partir do parametro e concatena com o diretorio padrão 

    remove(caminhoCompleto);
    printf("Arquivo: %s removido com sucesso!\n", nomeArquivo);
}         

// Função que retorna todos os arquivos indexados.
arquivosIndexados* mostrarArquivoIndexado() {
    printf("Arquivos Indexados: \n");
    for(int i = 0; i < numArquivosIndexados; i++){
        printf("- %s\n", arquivosIndexadosVet[i].nomeArquivo);
    }
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
    // Indexar o arquivo ao final da importação 
    indexarArquivo(arquivoDestino, indexacao);
}

void listarArquivos(){
    DIR *dp;
    struct dirent *ep;

    dp = opendir(repositorioNoticias);

    if (dp != NULL){
        int index = 0;
        printf("Arquivos no repositorio de noticias em %s: \n", repositorioNoticias);
        while((ep = readdir(dp)) != NULL){
            if(ep->d_type == DT_REG){
                arquivosIndexados arquivosIndexados;
                arquivosIndexados.descricaoIndex[sizeof(arquivosIndexados.descricaoIndex) - 1] = '\0'; // Adiciona o terminador nulo
                arquivosIndexados.nomeArquivo[sizeof(arquivosIndexados.nomeArquivo) - 1] = '\0'; // Adiciona o terminador nulo

                strncpy(arquivosIndexados.nomeArquivo, ep->d_name, sizeof(arquivosIndexados.nomeArquivo) -1);
                snprintf(arquivosIndexados.descricaoIndex, sizeof(arquivosIndexados.descricaoIndex) - 1, "Descrição do arquivo %s", ep->d_name);

                printf("%d. Nome: %s, Descrição: %s\n", index + 1, arquivosIndexados.nomeArquivo, arquivosIndexados.descricaoIndex);
            }
        }
        (void) closedir(dp);
    } else {
        perror("Erro ao abrir o diretório!");
        exit(EXIT_FAILURE);
    }

}
