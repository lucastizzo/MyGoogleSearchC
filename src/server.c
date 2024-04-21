#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <dirent.h>


#define MAX_ARQUIVOS 1000
#define MAX_RESULTADOS 1000000
#define MAX_TERMO_CHAVE_TAM 100
#define MAX_DESCRICAO_TAM 200

char repositorioNoticias = "mnt/c/home/tizzo/projeto-SD/repositorio-noticias/"

// Estrutura de um arquivo indexado 
typedef struct {
    char nomeArquivo[100];
    char descricao[MAX_DESCRICAO_TAM];
} arquivosIndexados;

// Estrutura do resultado de uma pesquisa
typedef struct {
    char nomeArquivo[100];
    char descricao[MAX_DESCRICAO_TAM];
} buscaNoticia;

//Vetor arquivos indexados
arquivosIndexados arquivosIndexados[MAX_ARQUIVOS];
int numArquivosIndexados = 0;

// Função para indexar um novo arquivo. 
void indexarArquivo(const char *nomeArquivo, const char *descricao){
    if(numArquivosIndexados < MAX_ARQUIVOS){
        strcpy(arquivosIndexados[numArquivosIndexados].nomeArquivo, nomeArquivo);
        strcpy(arquivosIndexados[numArquivosIndexados].nomeArquivo, descricao);
        numArquivos_indexados++;
    }
}

// Função para remover algum arquivo 
void removerArquivo(const char *nomeArquivo){
    char caminhoCompleto[300]; // Controla o tamanho do buffer para evitar inserções muito grande 
    strcpy(caminhoCompleto, repositorioNoticias); // Copia o diretorio de repositorio para o buffer 
    strcat(caminhoCompleto, nomeArquivo); // Pega o nome do arquivo a partir do parametro e concatena com o diretorio padrão 

    remove(caminhoCompleto);
}         

// Função que retorna todos os arquivos indexados.
void mostrarArquivoIndexado() {
    printf("Arquivos Indexados: \n");
    for(int i = 0; i < numArquivosIndexados; i++){
        printf("- %s\n" arquivosIndexados[i].nomeArquivo);
    }
}

//Função para fazer a busca 
buscaNoticia* busca(int numPesquisa,const char *termoChave){
    DIR* dir;
    struct dirent *ent;
    FILE* arquivo;  //Declarações para manipular arquivos/diretorios

    //Aloca a memória para 100 resultados de busca
    buscaNoticias *resultados = malloc(100 * sizeof(buscaNoticia));
    int numResultados = 0;
    numPesquisa = numPesquisa * 100;

    if((dir = opendir(repositorioNoticias)) != NULL){
        
        while((ent = readdir(dir))) != NULL{
            char caminhoArquivo[300];
            sprintf(caminhoArquivo, "%s%s", repositorioNoticias, ent->d_name); //Armazena strings em um buffer no nosso caso caminhoArquivo

            arquivo = fopen(caminho_arquivo, "r");
            if(arquivo == NULL){
                printf("Não foi possível abrir o diretório %s\n", repositorioNoticias);
                continue;
            }

            char linha[MAX_LINHA];
            while (fgets(linha, sizeof(linha), arquivo)){
                if (strstr(linha, termoChave) != NULL){ //strstr le 
                    printf("Busca encontrada encontrado em %s: %s\n", caminhoArquivo, linha) // Adiciona o resultado a lista de resultados
                    numResultados++;
                    if(numResultados < numPesquisa  && numResultados > numPesquisa - 100){
                        strcpy(resultados[numResultados].nomeArquivo, caminhoArquivo); // Copia o nome do arquivo para a spring de resultado
                        strcpy(resultados[numResultados].linhaEncontrada, linha);     // Copia a string resultante no                     
                    }
                }
            }

            fclose(arquivo);
        }
        closedir(dir);
    } else {
        printf("Não foi possível abrir o diretório %s\n", repositorioNoticias);
    }

    printf("O total de ocorrencias encontradas para o termo: %s",  termoChave "  foi de %d\n", numResultados);

    return resultados;
}
