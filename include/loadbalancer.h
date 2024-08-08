#ifndef LOADBALANCER_H
#define LOADBALANCER_H

#include "backend.h"

struct respostaServidor;


void inicializaCSV();
void registrarMetricaCSV(long tempoExecucao);
void *manipulaCliente(void *arg);
int obterProximoServidor();
int criaSocketLoadBalancer(int porta, int maximoConexoes);
void redistribuirRequisicoes(char *buffer, ssize_t bytesLidos, int socketCliente);
long calculaTempoExecucao(struct timeval start, struct timeval end);
bool verificarDisponibilidade(const char *servidorIP);
void monitorarRecursos();

#endif