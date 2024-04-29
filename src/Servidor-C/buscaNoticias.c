#include "buscaNoticias.h"

// Bibliotecas incluidas :  #include <netinet/in.h>
//                          #include <stdio.h>
//                          #include <sys/socket.h>
//                            #include <arpa/inet.h> 

// Variaveis definidas: #define MAX_LINHA = 1024
// 

// Estruturas definidas
// Estrutura do resultado de uma pesquisa
/*typedef struct {
    char nomeArquivo[100];
    char descricaoEncontrada[MAX_DESCRICAO_TAM];
} arquivosIndexados; */


//Função para fazer a busca 
buscaNoticias* busca(int numPesquisa,const char *termoChave, const char *repositorioNoticias){
    DIR* dir;
    struct dirent *ent;
    FILE* arquivo;  //Declarações para manipular arquivos/diretorios

    //Aloca a memória para 100 resultados de busca
    buscaNoticias *resultados = malloc(100 * sizeof(buscaNoticias));
    int numResultados = 0;
    numPesquisa = numPesquisa * 100;

    if((dir = opendir(repositorioNoticias)) != NULL){
        
        while((ent = readdir(dir)) != NULL){
            char caminhoArquivo[300];
            sprintf(caminhoArquivo, "%s%s", repositorioNoticias, ent->d_name); //Armazena strings em um buffer no nosso caso caminhoArquivo

            arquivo = fopen(caminhoArquivo, "r");
            if(arquivo == NULL){
                printf("Não foi possível abrir o diretório %s\n", repositorioNoticias);
                continue;
            }

            char linha[MAX_LINHA];
            while (fgets(linha, sizeof(linha), arquivo)){
                if (strstr(linha, termoChave) != NULL){ //strstr le 
                    printf("Busca encontrada encontrado em %s: %s\n", caminhoArquivo, linha); // Adiciona o resultado a lista de resultados
                    numResultados++;
                    if(numResultados < numPesquisa  && numResultados > numPesquisa - 100){
                        strcpy(resultados[numResultados].nomeArquivo, caminhoArquivo); // Copia o nome do arquivo para a spring de resultado
                        strcpy(resultados[numResultados].descricaoEncontrada, linha);     // Copia a string resultante em resultados                 
                    }
                }
            }

            fclose(arquivo);
        }
        closedir(dir);
    } else {
        printf("Não foi possível abrir o diretório %s\n", repositorioNoticias);
    }

    printf("O total de ocorrencias encontradas para o termo: %s foi de %d\n", termoChave, numResultados);

    return resultados;
}
