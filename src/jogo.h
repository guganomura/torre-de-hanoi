#ifndef JOGO_H
#define JOGO_H

#include "pilha.h"

#define NUM_DISCOS 4
#define NUM_TORRES 3
#define MINIMO_MOVIMENTOS ((1 << NUM_DISCOS) - 1) /* 2^n - 1 */

typedef struct {
    Pilha torres[NUM_TORRES];
    int movimentos;
} Jogo;

typedef enum {
    OK,
    ERRO_TORRE_INEXISTENTE,
    ERRO_MESMA_TORRE,
    ERRO_TORRE_VAZIA,
    ERRO_DISCO_MAIOR,
    ERRO_PARTIDA_VENCIDA,
    ERRO_MEMORIA
} Status;

/* Deixa as torres vazias. Deve ser chamada uma vez, antes das demais. */
void inicializar_jogo(Jogo *jogo);

/* Serve tanto para iniciar quanto para reiniciar. */
Status iniciar_partida(Jogo *jogo);

/* origem e destino vao de 0 a 2. Um movimento invalido nao altera o jogo. */
Status mover(Jogo *jogo, int origem, int destino);

int venceu(const Jogo *jogo);
const char *mensagem_status(Status status);

/* Desenha as torres, mostra as pilhas e a quantidade de movimentos. */
void exibir_jogo(const Jogo *jogo, FILE *saida);

void encerrar_partida(Jogo *jogo);

#endif
