#include "buscaNoticias.h"
#include "servidorHTTP.h"
#include "rotas.h"
#include "manipulaRepositorio.h"

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




// Resultado HTML da pesquisa 
void resultadoPesquisa(struct respostaServidor *resposta, int numPesquisa,const char *termoChave){
    char *html = "<html><head><title> MyGoogleSearch Resultado da pesquisa</title></head><body><h1>Resultado da pesquisa</h1><ul>";
    char *htmlFim = "</ul></body></html>";
    int i;


    char *htmlResultado = malloc(1000000 * sizeof(char));
    strcpy(htmlResultado, html);

    buscaNoticias *resultados = busca(numPesquisa, termoChave);// Adicionei esta linha para definir resultados

    while(strlen(resultados[i].nomeArquivo) > 0){
        char *htmlItem = malloc(1000 * sizeof(char));
        sprintf(htmlItem, "<li><a href=\"%s\">%s</a> - %s</li>", resultados[i].nomeArquivo, resultados[i].nomeArquivo, resultados[i].descricaoEncontrada);
        strcat(htmlResultado, htmlItem);
        free(htmlItem);
        
        i++;
    }

    strcat(htmlResultado, htmlFim);

    strcpy(resposta->conteudoResposta, htmlResultado);
    resposta->tamanhoResposta = strlen(htmlResultado);
    free(resultados);
}

//Função para fazer a busca 
buscaNoticias* busca(int numPesquisa,const char *termoChave){
    DIR* dir;
    struct dirent *ent;
    FILE* arquivo;  //Declarações para manipular arquivos/diretorios

    //Aloca a memória para 100 resultados de busca
    buscaNoticias *resultados = malloc(101 * sizeof(buscaNoticias));
    int numResultados = 0;
    int resultadoTotal = 0;
    numPesquisa = numPesquisa * 101;
    
    if (termoChave != NULL){
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
                                resultadoTotal++;    
                            }
                        }
                    }

                    fclose(arquivo);
                }
                closedir(dir);
            } else {
                sprintf(resultados[resultadoTotal].nomeArquivo,  "Não foi possível abrir o diretório %s\n", repositorioNoticias);
            }
    }else {
        sprintf(resultados->nomeArquivo, "Não é possivel pesquisar por nenhum termo!");
    }

    // Caso os resultados sejam maior que um, adiciona o total de resultados encontrados no final da lista criando um novo elemento no vetor, caso contrário apenas grava na posição 0.
    sprintf(resultados[resultadoTotal > 0 ? resultadoTotal +  1 : resultadoTotal].nomeArquivo,"O total de ocorrencias encontradas para o termo: %s foi de %d\n", termoChave, numResultados);
        //Definir rotas
    struct Rota *raiz = NULL;
    adicionaRota(raiz, "/resultadoPesquisa", "resultadoPesquisa");
    return resultados;
    
}
