#include "jogo.h"

#define LARGURA (2 * NUM_DISCOS + 1) /* largura do maior disco no desenho */

void inicializar_jogo(Jogo *jogo) {
    int i;
    for (i = 0; i < NUM_TORRES; i++) inicializar_pilha(&jogo->torres[i]);
    jogo->movimentos = 0;
}

void encerrar_partida(Jogo *jogo) {
    int i;
    for (i = 0; i < NUM_TORRES; i++) liberar_pilha(&jogo->torres[i]);
    jogo->movimentos = 0;
}

Status iniciar_partida(Jogo *jogo) {
    int disco;

    encerrar_partida(jogo);
    /* O maior entra primeiro, entao o menor termina no topo (LIFO). */
    for (disco = NUM_DISCOS; disco >= 1; disco--) {
        if (!empilhar(&jogo->torres[0], disco)) {
            encerrar_partida(jogo);
            return ERRO_MEMORIA;
        }
    }
    return OK;
}

Status mover(Jogo *jogo, int origem, int destino) {
    int disco, disco_destino;

    if (origem < 0 || origem >= NUM_TORRES || destino < 0 || destino >= NUM_TORRES)
        return ERRO_TORRE_INEXISTENTE;
    if (venceu(jogo)) return ERRO_PARTIDA_VENCIDA;
    if (origem == destino) return ERRO_MESMA_TORRE;
    if (!topo(&jogo->torres[origem], &disco)) return ERRO_TORRE_VAZIA;
    if (topo(&jogo->torres[destino], &disco_destino) && disco > disco_destino)
        return ERRO_DISCO_MAIOR;

    /* Empilha antes de desempilhar: se faltar memoria, o disco nao se perde. */
    if (!empilhar(&jogo->torres[destino], disco)) return ERRO_MEMORIA;
    desempilhar(&jogo->torres[origem], NULL);
    jogo->movimentos++;
    return OK;
}

/* As regras garantem a ordem, entao basta contar os discos da torre 3. */
int venceu(const Jogo *jogo) {
    return jogo->torres[NUM_TORRES - 1].quantidade == NUM_DISCOS;
}

const char *mensagem_status(Status status) {
    switch (status) {
        case OK: return "sucesso";
        case ERRO_TORRE_INEXISTENTE: return "essa torre nao existe";
        case ERRO_MESMA_TORRE: return "origem e destino sao a mesma torre";
        case ERRO_TORRE_VAZIA: return "a torre de origem esta vazia";
        case ERRO_DISCO_MAIOR: return "um disco maior nao pode ficar sobre um menor";
        case ERRO_PARTIDA_VENCIDA: return "a partida ja terminou";
        case ERRO_MEMORIA: return "memoria insuficiente";
    }
    return "erro desconhecido";
}

static void repetir(char c, int vezes, FILE *saida) {
    while (vezes-- > 0) fputc(c, saida);
}

/* O disco d ocupa 2d + 1 colunas: =1=, ==2==, ===3===, ====4==== */
static void desenhar_disco(int disco, FILE *saida) {
    int margem = NUM_DISCOS - disco;

    repetir(' ', margem, saida);
    repetir('=', disco, saida);
    fprintf(saida, "%d", disco);
    repetir('=', disco, saida);
    repetir(' ', margem, saida);
}

void exibir_jogo(const Jogo *jogo, FILE *saida) {
    const No *atual[NUM_TORRES]; /* so percorre os nos, nao guarda discos */
    int linha, i;

    for (i = 0; i < NUM_TORRES; i++) atual[i] = jogo->torres[i].topo;

    fprintf(saida, "\n");
    /* De cima para baixo, sem desempilhar. A linha mais alta mostra so as hastes. */
    for (linha = NUM_DISCOS + 1; linha >= 1; linha--) {
        for (i = 0; i < NUM_TORRES; i++) {
            fprintf(saida, "  ");
            if (atual[i] != NULL && jogo->torres[i].quantidade >= linha) {
                desenhar_disco(atual[i]->disco, saida);
                atual[i] = atual[i]->proximo;
            } else {
                repetir(' ', NUM_DISCOS, saida);
                fputc('|', saida);
                repetir(' ', NUM_DISCOS, saida);
            }
        }
        fprintf(saida, "\n");
    }
    fprintf(saida, "  ");
    repetir('-', NUM_TORRES * (LARGURA + 2) - 2, saida);
    fprintf(saida, "\n");
    for (i = 0; i < NUM_TORRES; i++) fprintf(saida, "   Torre %d ", i + 1);
    fprintf(saida, "\n\n");

    for (i = 0; i < NUM_TORRES; i++) {
        fprintf(saida, "Torre %d: ", i + 1);
        imprimir(&jogo->torres[i], saida);
        fprintf(saida, "\n");
    }
    fprintf(saida, "Movimentos: %d\n", jogo->movimentos);
}
