#include "loadbalancer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/statvfs.h>
#include <sys/sysinfo.h>
#include <stdbool.h>
#include <time.h>

#define PORTA 8080
#define TAMANHO_BUFFER 4096
#define MAXIMO_CONEXOES 10
#define NUM_SERVIDORES 3
#define CSV_FILE "performance_metrics.csv"

const char *servidoresBackend[NUM_SERVIDORES][2] = {
    {"127.0.0.1", "8081"},
    {"127.0.0.1", "8082"},
    {"127.0.0.1", "8083"}
};

bool servidoresDisponiveis[NUM_SERVIDORES] = { true, true, true };
int servidorAtual = 0;
pthread_mutex_t mutexServidor;


int main() {
    int socketServidor, socketCliente;
    struct sockaddr_in enderecoServidor, enderecoCliente;
    socklen_t tamanhoCliente = sizeof(enderecoCliente);
    pthread_t idThread;

    pthread_mutex_init(&mutexServidor, NULL);

    socketServidor = criaSocketLoadBalancer(PORTA, MAXIMO_CONEXOES);

    printf("Load Balancer em execução na porta %d\n", PORTA);

    // Inicializa o monitoramento de recursos
    pthread_t threadMonitoramento;
    if (pthread_create(&threadMonitoramento, NULL, (void *(*)(void *))monitorarRecursos, NULL) != 0) {
        perror("Falha ao criar thread de monitoramento");
    }

    // Cria o arquivo CSV e escreve o cabeçalho
    inicializaCSV();

    while (1) {
        printf("Aguardando nova conexão...\n");
        if ((socketCliente = accept(socketServidor, (struct sockaddr *)&enderecoCliente, &tamanhoCliente)) < 0) {
            perror("Falha ao aceitar conexão");
            continue;
        }

        printf("Nova conexão aceita. Criando thread para manipulação do cliente..., socket cliente:%d\n", socketCliente);
        int *pCliente = malloc(sizeof(int));
        *pCliente = socketCliente;

        if (pthread_create(&idThread, NULL, manipulaCliente, pCliente) != 0) {
            perror("Falha ao criar thread");
            free(pCliente);
            close(socketCliente);
        }
    }

    pthread_mutex_destroy(&mutexServidor);
    close(socketServidor);
    return 0;
}

void *manipulaCliente(void *arg) {
    int socketCliente = *(int *)arg;
    free(arg);

    struct timeval start, end;
    gettimeofday(&start, NULL);

    printf("Thread para manipulação do cliente iniciada, socket cliente: %d\n", socketCliente);

    char buffer[TAMANHO_BUFFER];
    ssize_t bytesLidos;

    // Lendo dados do cliente
    printf("Lendo dados do cliente no socket %d...\n", socketCliente);
    bytesLidos = read(socketCliente, buffer, TAMANHO_BUFFER);
    if (bytesLidos < 0) {
        perror("Erro ao ler dados do cliente");
        close(socketCliente);
        return NULL;
    } else if (bytesLidos == 0) {
        printf("Conexão fechada pelo cliente.\n");
        close(socketCliente);
        return NULL;
    }
    printf("Dados lidos do cliente (%zd bytes): %s\n", bytesLidos, buffer);

    redistribuirRequisicoes(buffer, bytesLidos, socketCliente);

    gettimeofday(&end, NULL);
    long tempoExecucao = calculaTempoExecucao(start, end);
    printf("Tempo de execução da requisição: %ld ms\n", tempoExecucao);
    registrarMetricaCSV(tempoExecucao);

    return NULL;
}

void redistribuirRequisicoes(char *buffer, ssize_t bytesLidos, int socketCliente) {
    ssize_t bytesEnviados;
    int socketBackend;
    struct sockaddr_in enderecoBackend;

    int tentativas = 0;
    while (tentativas < NUM_SERVIDORES) {
        int indiceServidor = obterProximoServidor();
        if (indiceServidor == -1) {
            printf("Nenhum servidor disponível. Encerrando conexão.\n");
            close(socketCliente);
            return;
        }

        printf("Conectando ao backend %s:%s\n", servidoresBackend[indiceServidor][0], servidoresBackend[indiceServidor][1]);

        if ((socketBackend = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("Falha ao criar socket backend");
            tentativas++;
            continue;
        }

        enderecoBackend.sin_family = AF_INET;
        enderecoBackend.sin_port = htons(atoi(servidoresBackend[indiceServidor][1]));

        if (inet_pton(AF_INET, servidoresBackend[indiceServidor][0], &enderecoBackend.sin_addr) <= 0) {
            perror("Endereço inválido ou não suportado");
            close(socketBackend);
            tentativas++;
            continue;
        }

        printf("Tentando conectar ao backend...\n");
        if (connect(socketBackend, (struct sockaddr *)&enderecoBackend, sizeof(enderecoBackend)) < 0) {
            perror("Falha ao conectar ao backend");
            close(socketBackend);
            tentativas++;
            continue;
        }

        // Enviando dados para o backend
        printf("Conectado ao backend. Enviando dados...\n");
        bytesEnviados = send(socketBackend, buffer, bytesLidos, 0);
        if (bytesEnviados < 0) {
            perror("Erro ao enviar dados para o backend");
            close(socketBackend);
            tentativas++;
            continue;
        }
        printf("Dados enviados ao backend (%zd bytes)\n", bytesEnviados);

        // Limpando buffer e aguardando resposta do backend
        memset(buffer, 0, TAMANHO_BUFFER);
        printf("Aguardando resposta do backend...\n");

        // Lendo dados do backend e enviando de volta ao cliente iterativamente
        ssize_t bytesRecebidos;
        while ((bytesRecebidos = read(socketBackend, buffer, TAMANHO_BUFFER)) > 0) {
            printf("Dados recebidos do backend (%zd bytes)\n", bytesRecebidos);

            // Enviando dados de volta ao cliente
            bytesEnviados = send(socketCliente, buffer, bytesRecebidos, 0);
            if (bytesEnviados < 0) {
                perror("Erro ao enviar dados ao cliente");
                close(socketBackend);
                close(socketCliente);
                return;
            }
            printf("Dados enviados ao cliente (%zd bytes)\n", bytesEnviados);
        }

        if (bytesRecebidos < 0) {
            perror("Erro ao ler dados do backend");
        } else {
            printf("Backend fechou a conexão.\n");
        }

        close(socketBackend);
        break;
    }

    if (tentativas == NUM_SERVIDORES) {
        printf("Não foi possível conectar a nenhum backend. Encerrando conexão.\n");
        close(socketCliente);
    }
}

void registrarMetricaCSV(long tempoExecucao) {
    FILE *csvFile = fopen(CSV_FILE, "a");
    if (csvFile != NULL) {
        fprintf(csvFile, "%ld\n", tempoExecucao);
        fclose(csvFile);
    } else {
        perror("Erro ao abrir o arquivo CSV");
    }
}

int obterProximoServidor() {
    pthread_mutex_lock(&mutexServidor);
    int servidorInicial = servidorAtual;
    do {
        if (servidoresDisponiveis[servidorAtual]) {
            int servidor = servidorAtual;
            servidorAtual = (servidorAtual + 1) % NUM_SERVIDORES;
            pthread_mutex_unlock(&mutexServidor);
            return servidor;
        }
        servidorAtual = (servidorAtual + 1) % NUM_SERVIDORES;
    } while (servidorAtual != servidorInicial);

    pthread_mutex_unlock(&mutexServidor);
    return -1; // Nenhum servidor disponível
}

int criaSocketLoadBalancer(int porta, int maximoConexoes) {
    int socketServidor;
    struct sockaddr_in enderecoServidor;

    if ((socketServidor = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Falha ao criar socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(socketServidor, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("Falha ao configurar opções do socket");
        close(socketServidor);
        exit(EXIT_FAILURE);
    }

    enderecoServidor.sin_family = AF_INET;
    enderecoServidor.sin_addr.s_addr = INADDR_ANY;
    enderecoServidor.sin_port = htons(porta);

    if (bind(socketServidor, (struct sockaddr *)&enderecoServidor, sizeof(enderecoServidor)) < 0) {
        perror("Falha ao associar o socket");
        close(socketServidor);
        exit(EXIT_FAILURE);
    }

    if (listen(socketServidor, maximoConexoes) < 0) {
        perror("Falha ao colocar o socket em modo de escuta");
        close(socketServidor);
        exit(EXIT_FAILURE);
    }

    return socketServidor;
}

long calculaTempoExecucao(struct timeval start, struct timeval end) {
    return ((end.tv_sec - start.tv_sec) * 1000L + (end.tv_usec - start.tv_usec) / 1000L);
}

bool verificarDisponibilidade(const char *servidorIP) {
    char comando[100];
    snprintf(comando, sizeof(comando), "ping -c 1 %s", servidorIP);
    int resultado = system(comando);
    return resultado == 0; // 0 indica sucesso
}

void monitorarRecursos() {
    while (1) {
        for (int i = 0; i < NUM_SERVIDORES; i++) {
            if (!verificarDisponibilidade(servidoresBackend[i][0])) {
                printf("Servidor %s:%s está offline\n", servidoresBackend[i][0], servidoresBackend[i][1]);
                servidoresDisponiveis[i] = false;
            } else {
                printf("Servidor %s:%s está online\n", servidoresBackend[i][0], servidoresBackend[i][1]);
                servidoresDisponiveis[i] = true;
            }
        }
        sleep(10); // Aguardar 10 segundos antes da próxima verificação
    }
}

void inicializaCSV() {
    FILE *csvFile = fopen(CSV_FILE, "w");
    if (csvFile != NULL) {
        fprintf(csvFile, "TempoExecucao(ms)\n");
        fclose(csvFile);
    } else {
        perror("Erro ao criar o arquivo CSV");
    }
}
