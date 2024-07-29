#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <time.h>
#include <math.h>

#define NUM_TESTES 4
#define DURACAO 60 // 1 minuto
#define URL "http://127.0.0.1:8080"

//Algoritmo de teste favor executar o servidor antes de executa-lo
/* Para compilar
    1- Instalar a biblioteca curl, caso no linux o comando é esse

        sudo apt-get install libcurl4-openssl-dev
    2- Compilar o código com o comando
        gcc -o teste teste.c -lcurl -lm
    3- Executar

*/


// Função de comparação para ordenar os tempos de resposta
int comparar(const void *a, const void *b) {
    return (*(double*)a - *(double*)b);
}

// Função para calcular estatísticas (média, mediana e desvio padrão)
void calcular_estatisticas(double *tempos, int contagem, FILE *arquivo) {
    qsort(tempos, contagem, sizeof(double), comparar);

    int descartar = contagem * 0.05;
    int contagem_valida = contagem - 2 * descartar;
    double soma = 0.0, media, mediana, desvio_padrao = 0.0;

    for (int i = descartar; i < contagem - descartar; i++) {
        soma += tempos[i];
    }
    media = soma / contagem_valida;

    if (contagem_valida % 2 == 0) {
        mediana = (tempos[descartar + contagem_valida / 2 - 1] + tempos[descartar + contagem_valida / 2]) / 2.0;
    } else {
        mediana = tempos[descartar + contagem_valida / 2];
    }

    for (int i = descartar; i < contagem - descartar; i++) {
        desvio_padrao += pow(tempos[i] - media, 2);
    }
    desvio_padrao = sqrt(desvio_padrao / contagem_valida);

    fprintf(arquivo, "%.2f,%.2f,%.2f\n", media, mediana, desvio_padrao);
}

// Função para realizar o teste com um número específico de requisições por segundo
void realizar_teste(int requisicoes_por_segundo, FILE *arquivo) {
    CURL *curl;
    CURLcode res;
    struct timespec inicio, fim;
    double *tempos = malloc(requisicoes_por_segundo * DURACAO * sizeof(double));
    int contagem = 0;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        for (int i = 0; i < DURACAO; i++) {
            for (int j = 0; j < requisicoes_por_segundo; j++) {
                clock_gettime(CLOCK_MONOTONIC, &inicio);
                curl_easy_setopt(curl, CURLOPT_URL, URL);
                res = curl_easy_perform(curl);
                clock_gettime(CLOCK_MONOTONIC, &fim);

                if (res == CURLE_OK) {
                    double tempo_decorrido = (fim.tv_sec - inicio.tv_sec) * 1000.0 + (fim.tv_nsec - inicio.tv_nsec) / 1000000.0;
                    tempos[contagem++] = tempo_decorrido;
                }

                struct timespec req = {0, (long)(1e9 / requisicoes_por_segundo)};
                nanosleep(&req, NULL);
            }
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    calcular_estatisticas(tempos, contagem, arquivo);
    free(tempos);
}

int main(void) {
    int requisicoes_por_segundo[NUM_TESTES] = {50, 100, 200, 400};
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    // Formatar o nome do arquivo com a data atual
    char nome_arquivo[100];
    snprintf(nome_arquivo, sizeof(nome_arquivo), "C:\\tmp\\MyGoogleSearchc-Tizzo-%04d-%02d-%02d-%02d-%02d-%02d.csv", 
             tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    // Abrir o arquivo CSV para escrita
    FILE *arquivo = fopen(nome_arquivo, "w");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    // Escrever o cabeçalho do CSV
    fprintf(arquivo, "RequisicoesPorSegundo;Media;Mediana;DesvioPadrao\n");

    for (int i = 0; i < NUM_TESTES; i++) {
        printf("Testando com %d requisições por segundo:\n", requisicoes_por_segundo[i]);
        fprintf(arquivo; "%d;", requisicoes_por_segundo[i]);
        realizar_teste(requisicoes_por_segundo[i], arquivo);
    }

    fclose(arquivo);
    return 0;
}