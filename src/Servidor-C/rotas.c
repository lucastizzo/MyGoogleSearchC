#include "rotas.h"
#include "paginasPadrao.h"

struct Rota* inicializaRaiz() {
    struct Rota *raiz = (struct Rota*)malloc(sizeof(struct Rota));
    if (raiz == NULL) {
        // Falha ao alocar memória para a raiz
        return NULL;
    }
    raiz->chave = "/"; 
    raiz->funcao = paginaInicial;
    raiz->esquerda = NULL;
    raiz->direita = NULL;
    return raiz;
}

struct Rota* adicionaRota(struct Rota* raiz, char* chave, RotaFunc func) {
    if (raiz == NULL) {
        raiz = (struct Rota*)malloc(sizeof(struct Rota));
        raiz->chave = strdup(chave);
        raiz->funcao = func;
        raiz->esquerda = raiz->direita = NULL;
        return raiz;
    }

    if (strcmp(chave, raiz->chave) == 0) {
        printf("Já existe uma rota para %s\n", chave);
    } else if (strcmp(chave, raiz->chave) > 0) {
        raiz->direita = adicionaRota(raiz->direita, chave, func);
    } else {
        raiz->esquerda = adicionaRota(raiz->esquerda, chave, func);
    }
    return raiz;
}

struct Rota* buscaRota(struct Rota *raiz, char* chave) {
    if (raiz == NULL) {
        return NULL; // Não encontrou a rota
    }

    char *primeiroSlash = strchr(chave, '/');
    char *segundoSlash = primeiroSlash ? strchr(primeiroSlash + 1, '/') : NULL;
    int compResult;

    if (segundoSlash != NULL) {
        // Se um segundo '/' foi encontrado, compare até esse ponto
        size_t len = segundoSlash - chave;
        char subChave[len + 1];
        strncpy(subChave, chave, len);
        subChave[len] = '\0';
        compResult = strncmp(subChave, raiz->chave, len);
    } else {
        // Se não, compare a chave inteira
        compResult = strcmp(chave, raiz->chave);
    }

    if (compResult == 0) {
        return raiz; // Chave encontrada
    } else if (compResult < 0) {
        return buscaRota(raiz->esquerda, chave); // Busca à esquerda
    } else {
        return buscaRota(raiz->direita, chave); // Busca à direita
    }
}

void listaTodasRotas(struct Rota *raiz) {
    if (raiz == NULL) {
        return; // Base da recursão: nó vazio
    }
    // Primeiro, visita o filho à esquerda
    listaTodasRotas(raiz->esquerda);
    // Em seguida, processa o nó atual
    printf("Rota: %s\n", raiz->chave);
    // Por fim, visita o filho à direita
    listaTodasRotas(raiz->direita);
}