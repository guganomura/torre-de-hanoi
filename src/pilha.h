#ifndef PILHA_H
#define PILHA_H

#include <stdio.h>

typedef struct No {
    int disco;
    struct No *proximo; /* o no logo abaixo; NULL na base */
} No;

typedef struct {
    No *topo; /* NULL se a pilha estiver vazia */
    int quantidade;
} Pilha;

/* Deixa a pilha vazia. Se ela ja tiver nos, chame liberar_pilha antes. */
void inicializar_pilha(Pilha *pilha);

/* Coloca um disco no topo. Retorna 1, ou 0 se faltar memoria. */
int empilhar(Pilha *pilha, int disco);

/* Retira o disco do topo. Retorna 1, ou 0 se a pilha estiver vazia.
 * Se disco nao for NULL, recebe o valor retirado. */
int desempilhar(Pilha *pilha, int *disco);

/* Consulta o topo sem retirar. Retorna 1, ou 0 se a pilha estiver vazia. */
int topo(const Pilha *pilha, int *disco);

/* Escreve do topo para a base: [1] -> [2] -> NULL */
void imprimir(const Pilha *pilha, FILE *saida);

/* Auxiliar exigida pelo C: devolve a memoria de todos os nos. */
void liberar_pilha(Pilha *pilha);

#endif
