#include "buscaNoticias.h"
#include "backend.h"
#include "rotas.h"
#include "manipulaRepositorio.h"
#include "frontend.h"

char *limparCaracteresInvalidos(const char *entrada) { // formatar para unicode devido um erro do arquivo
    iconv_t cd = iconv_open("UTF-8//IGNORE", "UTF-8");
    if (cd == (iconv_t)-1)
    {
        perror("iconv_open");
        return NULL;
    }

    size_t inbytesleft = strlen(entrada);
    size_t outbytesleft = inbytesleft * 2;
    char *output = malloc(outbytesleft);
    char *outbuf = output;
    if (iconv(cd, (char **)&entrada, &inbytesleft, &outbuf, &outbytesleft) == (size_t)-1)
    {
        perror("iconv");
        free(output);
        iconv_close(cd);
        return NULL;
    }

    *outbuf = '\0';
    iconv_close(cd);
    return output;
}

void inicializaRotasBusca(struct Rota *raiz)
{
    adicionaRota(raiz, "/resultadoPesquisa", resultadoPesquisahandler);
}

void resultadoPesquisahandler(char *requisicao, int sock, struct respostaServidor *resposta)
{
    char termoPesquisa[1024]; // Aloca espaço para a string

    int numPesquisa;
    printf("antes de ler a requisição no resultaPesquisahandler\n");
    sscanf(requisicao, "%*s %1023s", termoPesquisa); // Lê a string para o array alocado
    printf("termo pesquisa: %s\n", termoPesquisa);

    char *pesquisa = strchr(termoPesquisa + 2, '/');
    if (pesquisa == NULL)
    {
        montaHTML(sock, resposta, "erroPesquisa");
        enviaResposta(sock, resposta);
        return;
    }

    int tamanho = strlen(pesquisa);

    if (pesquisa[tamanho - 1] == '?')
    {
        pesquisa[tamanho - 1] = '\0';
    }

    numPesquisa = atoi(pesquisa + 1);
    printf("numPesquisa: %d\n", numPesquisa);
    char *termoPesquisaPtr = strchr(pesquisa + 1, '/');

    if (termoPesquisaPtr != NULL)
    {
        termoPesquisaPtr++;
        char termoDecodificado[1024];
        url_decode(termoPesquisaPtr, termoDecodificado);
        printf("Termo decodificado: %s\n", termoPesquisaPtr);
        resultadoPesquisa(requisicao, sock, resposta, numPesquisa, termoPesquisaPtr);
    }
    else
    {
        montaHTML(sock, resposta, "erroPesquisa");
        enviaResposta(sock, resposta);
    }
}

void resultadoPesquisa(char *requisicao, int sock, struct respostaServidor *resposta, int numPesquisa, char *termoChave)
{
    char *htmlFim = "</body></html>";
    char *htmlResultado = malloc(900000 * sizeof(char));
    htmlResultado[0] = '\0';

    buscaNoticias *resultados = malloc(101 * sizeof(buscaNoticias));
    for (int i = 0; i < 101; i++)
    {
        resultados[i].nomeArquivo[0] = '\0';
        resultados[i].descricaoEncontrada[0] = '\0';
        resultados[i].url[0] = '\0';
        resultados[i].titulo[0] = '\0';
    }
    resultados[0].total = 0;

    printf("buscando noticias.\n");
    busca(numPesquisa, termoChave, resultados);

    printf("Criando HTML's: \n");
    strcat(htmlResultado, "<ul>");
    
    char ResultadoPesquisa[200];
    
    if (resultados[0].total > numPesquisa * 100) {
        sprintf(ResultadoPesquisa, "<p>Mostrando resultados de %d a %d de um total de %d</p>", ((numPesquisa * 100) - 100), (numPesquisa * 100), resultados[0].total);
    }else{ 
        sprintf(ResultadoPesquisa, "<p>Mostrando resultados de %d a %d de um total de %d</p>", ((numPesquisa * 100) - 100), resultados[0].total, resultados[0].total);
    }
    strcat(htmlResultado, ResultadoPesquisa);
    
    for (int i = 0; strlen(resultados[i].nomeArquivo) > 0; i++)
    {
        // printf("Iteração %d, montando HTML\n", i);
        char htmlItem[10000];
        sprintf(htmlItem, "<li><a href=\"%s\">%s</a><br>%s<br><small>Arquivo: %s</small></li>", resultados[i].url, resultados[i].titulo, resultados[i].descricaoEncontrada, resultados[i].nomeArquivo);
        strcat(htmlResultado, htmlItem);
    }
    strcat(htmlResultado, "</ul>");

    if (resultados[0].total > 0)
    {
        int numBotoes = (resultados[0].total + 99) / 100;
        int j;

        strcat(htmlResultado, "<div class='pagination'>");

        if (numBotoes > 5) {

            j = numBotoes - 5;

        } else {
            j = 1;
        }

        while (j < (numPesquisa + 6) && j < (numBotoes + 1)){
                
                char botaoHTML[200];
                sprintf(botaoHTML, "<a href='/resultadoPesquisa/%d/%s' class='btn'>%d</a> ", j, termoChave, j);
                strcat(htmlResultado, botaoHTML);
                j++;
        }
        
        if (j < numBotoes) {
            char botaoHTML[200];
            sprintf(botaoHTML, "<a href='/resultadoPesquisa/%d/%s' class='btn'>Próximo</a> ", numBotoes, termoChave);
            strcat(htmlResultado, botaoHTML);
        }


        strcat(htmlResultado, "</div>");
    }

    strcat(htmlResultado, htmlFim);
    printf("Montando HTML's\n");
    montaHTML(sock, resposta, "resultadoPesquisa");
    strcat(resposta->conteudo, htmlResultado);

    enviaResposta(sock, resposta);

    printf("Limpando, noticias, termo chave: %s, numPesquisa: %d\n", termoChave, numPesquisa);
    
    if (htmlResultado != NULL) {
        printf("Limpando HTML resultado: \n");
        free(htmlResultado);
    }

// Verifique se 'resultados' não é nulo antes de liberar a memória
    if (resultados != NULL) {
        printf("Limpando resultado: \n");
        free(resultados);
    }

}

void busca(int numPesquisa, const char *termoChave, buscaNoticias resultados[])
{
    printf("Na chamada\n");
    DIR *dir;
    struct dirent *ent;
    FILE *arquivo;

    int numResultados = 0;
    int resultadoTotal = 0;
    int vetorPesquisa = 0;

    if (termoChave != NULL)
    {
        printf("Antes de abrir repositorioNoticias, %s\n", repositorioNoticias);
        if ((dir = opendir(repositorioNoticias)) != NULL)
        {
            printf("Abriu o repositorioNoticias\n");

            while ((ent = readdir(dir)) != NULL)
            {

                if (ent->d_type == DT_REG)
                { // Verifica se é um arquivo regular
                    char caminhoArquivo[300];
                    sprintf(caminhoArquivo, "%s/%s", repositorioNoticias, ent->d_name);

                    char *extensao = strrchr(ent->d_name, '.');

                    if (extensao != NULL && strcmp(extensao, ".jsonl") == 0)
                    {

                        arquivo = fopen(caminhoArquivo, "r");

                        if (arquivo == NULL)
                        {
                            printf("Não foi possível abrir o arquivo %s\n", caminhoArquivo);
                            continue;
                        }

                        char *linha = NULL;
                        size_t buffer_size = 0;
                        ssize_t linha_length;
                        struct json_tokener *tok = json_tokener_new();
                        
                        while ((linha_length = getline(&linha, &buffer_size, arquivo)) != -1)
                        {

                            char *linha_corrigida = NULL;

                            enum json_tokener_error jerr;
                            struct json_object *parsed_json = json_tokener_parse_ex(tok, linha, strlen(linha));
                            jerr = json_tokener_get_error(tok);

                            if (jerr != json_tokener_success){

                                printf("Fazendo correção de caracter JSON: %s\n", json_tokener_error_desc(jerr));
                                linha_corrigida = limparCaracteresInvalidos(linha);
                                if (linha_corrigida != NULL){

                                    parsed_json = json_tokener_parse_ex(tok, linha_corrigida, strlen(linha_corrigida));
                                    jerr = json_tokener_get_error(tok);
                                    
                                    printf("Feita correção de json, validando:\n");
                                    free(linha_corrigida);

                                    if (jerr != json_tokener_success)
                                        {
                                        printf("Erro ao fazer parse do JSON após conversão, verifique! \n%s\n", json_tokener_error_desc(jerr));
                                        json_tokener_reset(tok); // Reset tokener para a próxima linha
                                        continue;
                                    } else {
                                        printf("Linha JSON convertido com sucesso\n");
                                    }
                                }
                                else{
                                    printf("Erro ao fazer correção de caracteres inválidos\n");
                                    json_tokener_reset(tok);
                                    continue;
                                }
                            }

                            // Processar JSON caso OK
                            struct json_object *titulo, *url, *maintext, *filename;

                            if (!json_object_object_get_ex(parsed_json, "title", &titulo) ||
                                !json_object_object_get_ex(parsed_json, "url", &url) ||
                                !json_object_object_get_ex(parsed_json, "maintext", &maintext) ||
                                !json_object_object_get_ex(parsed_json, "filename", &filename))
                            {

                                printf("Campos necessários não encontrados no JSON\n");

                                json_object_put(parsed_json);
                                continue;
                            }

                            const char *maintext_str = json_object_get_string(maintext);
                            if (maintext_str != NULL && termoChave != NULL && strstr(maintext_str, termoChave) != NULL)
                            {
                                 numResultados++;
                                //printf("Num Pesquisa: %d\n", ((numPesquisa * 100) - 101));
                                if (numResultados > ((numPesquisa * 100) - 101) && numResultados < ((numPesquisa  * 100) + 1))
                                {
                                    strcpy(resultados[vetorPesquisa].nomeArquivo, json_object_get_string(filename));
                                    strcpy(resultados[vetorPesquisa].titulo, json_object_get_string(titulo));
                                    strcpy(resultados[vetorPesquisa].url, json_object_get_string(url));
                                    strncpy(resultados[vetorPesquisa].descricaoEncontrada, maintext_str, MAX_DESCRICAO_NOT_TAM);
                                    vetorPesquisa++;
                                    
                                  //  printf("vetorPesquisa: %d, numResulados: %d\n" , vetorPesquisa, numResultados);
                                   
                                    
                                }
                            }

                            json_object_put(parsed_json);
                        }
                        
                        free(linha);
                        fclose(arquivo);
                        json_tokener_free(tok);
                    }
                }
            }
            closedir(dir);
        }
        else
        {
            strcpy(resultados[0].nomeArquivo, "Não foi possível abrir o diretório");
        }
    }
    else
    {
        strcpy(resultados[0].nomeArquivo, "Não é possível pesquisar por termo nulo!");
    }

    printf("Num Resultados: %d\n", numResultados);

    if (numResultados == 0)
    {
        strcpy(resultados[0].nomeArquivo, "Nenhum resultado encontrado.");
        strcpy(resultados[0].descricaoEncontrada, "");
        resultados[0].total = 0;
    }
    else
    {
        resultados[0].total = numResultados;
        if (resultadoTotal > 0)
        {
            sprintf(resultados[resultadoTotal].nomeArquivo, "Total de ocorrências encontradas: %d", numResultados);
            strcpy(resultados[resultadoTotal].descricaoEncontrada, termoChave);
        }
    }
}