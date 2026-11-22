/* Interface de terminal. As mensagens nao usam acentos para aparecerem
 * corretamente tambem no terminal do Windows. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jogo.h"

#define TAM_LINHA 64

/* Respostas possiveis alem dos numeros das torres (0 a 2). */
#define REINICIAR -1
#define VOLTAR_AO_MENU -2
#define SAIR -3

static Jogo jogo;

static void liberar_jogo(void) {
    encerrar_partida(&jogo);
}

static void sair(void) {
    printf("\nJogo encerrado.\n");
    exit(0); /* liberar_jogo e chamada pelo atexit */
}

/* Le uma linha, sem espacos nas pontas. O fim da entrada encerra o jogo. */
static void ler_linha(const char *pergunta, char linha[TAM_LINHA]) {
    size_t inicio, fim;
    int c;

    for (;;) {
        printf("%s", pergunta);
        fflush(stdout);
        if (fgets(linha, TAM_LINHA, stdin) == NULL) {
            printf("\nFim da entrada.");
            sair();
        }
        if (strchr(linha, '\n') != NULL || feof(stdin)) break;

        while ((c = getchar()) != '\n' && c != EOF) {
            /* descarta o restante da linha */
        }
        printf("Entrada muito longa.\n");
    }
    inicio = strspn(linha, " \t\r\n");
    fim = strlen(linha);
    while (fim > inicio && strchr(" \t\r\n", linha[fim - 1]) != NULL) fim--;
    memmove(linha, linha + inicio, fim - inicio);
    linha[fim - inicio] = '\0';
}

static int ler_jogada(const char *pergunta) {
    char linha[TAM_LINHA];

    for (;;) {
        ler_linha(pergunta, linha);
        if (strlen(linha) == 1) {
            char c = linha[0];
            if (c >= '1' && c < '1' + NUM_TORRES) return c - '1';
            if (c == 'r' || c == 'R') return REINICIAR;
            if (c == 'm' || c == 'M') return VOLTAR_AO_MENU;
            if (c == 's' || c == 'S') return SAIR;
        }
        printf("Entrada invalida. Digite 1, 2, 3, R, M ou S.\n");
    }
}

/* Joga ate a vitoria ou ate o jogador encerrar a partida. */
static void jogar_partida(void) {
    Status status = iniciar_partida(&jogo);
    int origem, destino, disco, resposta;

    if (status != OK) {
        printf("Erro: %s.\n", mensagem_status(status));
        return;
    }
    printf("\nNova partida iniciada.\n");
    printf("Comandos: R reinicia | M encerra a partida | S sai do jogo\n");
    exibir_jogo(&jogo, stdout);

    while (!venceu(&jogo)) {
        destino = 0;
        origem = ler_jogada("\nTorre de origem (1-3): ");
        if (origem >= 0) {
            /* Avisa logo, sem pedir um destino a toa. */
            if (!topo(&jogo.torres[origem], &disco)) {
                printf("Movimento invalido: %s.\n", mensagem_status(ERRO_TORRE_VAZIA));
                continue;
            }
            destino = ler_jogada("Torre de destino (1-3): ");
        }
        resposta = origem < 0 ? origem : destino;

        if (resposta == SAIR) sair();
        if (resposta == VOLTAR_AO_MENU) {
            printf("Partida encerrada. Movimentos feitos: %d\n", jogo.movimentos);
            encerrar_partida(&jogo);
            return;
        }
        if (resposta == REINICIAR) {
            status = iniciar_partida(&jogo);
            if (status != OK) {
                printf("Erro: %s.\n", mensagem_status(status));
                return;
            }
            printf("Partida reiniciada.\n");
            exibir_jogo(&jogo, stdout);
            continue;
        }

        status = mover(&jogo, origem, destino);
        if (status != OK) {
            printf("Movimento invalido: %s.\n", mensagem_status(status));
            continue;
        }
        printf("Disco %d movido da torre %d para a torre %d.\n", disco, origem + 1, destino + 1);
        exibir_jogo(&jogo, stdout);
    }

    printf("\nParabens, voce venceu em %d movimentos!\n", jogo.movimentos);
    if (jogo.movimentos == MINIMO_MOVIMENTOS) printf("Esse e o minimo possivel.\n");
    else printf("O minimo possivel e %d.\n", MINIMO_MOVIMENTOS);
}

int main(void) {
    char linha[TAM_LINHA];

    inicializar_jogo(&jogo);
    atexit(liberar_jogo);

    for (;;) {
        printf("\n=== TORRE DE HANOI ===\n"
               "Leve os %d discos da torre 1 para a torre 3.\n"
               "Mova um disco por vez e nunca ponha um maior sobre um menor.\n"
               " 1. Nova partida\n"
               " 0. Encerrar o jogo\n",
               NUM_DISCOS);
        ler_linha("Opcao: ", linha);

        if (strcmp(linha, "1") == 0) jogar_partida();
        else if (strcmp(linha, "0") == 0) sair();
        else printf("Opcao invalida.\n");
    }
}
