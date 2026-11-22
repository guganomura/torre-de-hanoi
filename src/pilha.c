#include "pilha.h"

#include <stdlib.h>

void inicializar_pilha(Pilha *pilha) {
    pilha->topo = NULL;
    pilha->quantidade = 0;
}

int empilhar(Pilha *pilha, int disco) {
    No *novo = malloc(sizeof *novo);
    if (novo == NULL) return 0;

    novo->disco = disco;
    novo->proximo = pilha->topo; /* 1. o novo aponta para o antigo topo */
    pilha->topo = novo;          /* 2. o novo passa a ser o topo */
    pilha->quantidade++;
    return 1;
}

int desempilhar(Pilha *pilha, int *disco) {
    No *retirado = pilha->topo; /* 1. guarda o no do topo */
    if (retirado == NULL) return 0;

    if (disco != NULL) *disco = retirado->disco;
    pilha->topo = retirado->proximo; /* 2. o topo desce para o no de baixo */
    free(retirado);                  /* 3. libera o no que saiu */
    pilha->quantidade--;
    return 1;
}

int topo(const Pilha *pilha, int *disco) {
    if (pilha->topo == NULL) return 0;
    *disco = pilha->topo->disco;
    return 1;
}

void imprimir(const Pilha *pilha, FILE *saida) {
    const No *atual;

    for (atual = pilha->topo; atual != NULL; atual = atual->proximo) {
        fprintf(saida, "[%d] -> ", atual->disco);
    }
    fprintf(saida, "NULL");
}

void liberar_pilha(Pilha *pilha) {
    while (desempilhar(pilha, NULL)) {
        /* cada chamada libera um no */
    }
}
