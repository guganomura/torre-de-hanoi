#include <string.h>

#include "../src/jogo.h"
#include "../src/pilha.h"

static int total, falhas;

/* Solucao otima: cada par e origem e destino, de 1 a 3. */
static const char *SOLUCAO = "121323123132121323213123121323";

static void verificar(const char *id, const char *cenario, const char *esperado, int condicao) {
    total++;
    if (!condicao) falhas++;
    printf("[%s] %s %s -> %s\n", condicao ? "OK" : "FALHA", id, cenario, esperado);
}

/* Confere os discos do topo para a base, com discos como "124". */
static int conteudo(const Pilha *p, const char *discos) {
    const No *no = p->topo;
    int n = 0;
    for (; discos[n] != '\0'; n++) {
        if (no == NULL || no->disco != discos[n] - '0') return 0;
        no = no->proximo;
    }
    return no == NULL && p->quantidade == n;
}

static int estado(const Jogo *j, const char *a, const char *b, const char *c, int movimentos) {
    return conteudo(&j->torres[0], a) && conteudo(&j->torres[1], b) &&
           conteudo(&j->torres[2], c) && j->movimentos == movimentos;
}

static int jogar(Jogo *j, const char *passos) {
    int i;
    for (i = 0; passos[i] != '\0'; i += 2)
        if (mover(j, passos[i] - '1', passos[i + 1] - '1') != OK) return 0;
    return 1;
}

static int impresso(const Pilha *p, const char *esperado) {
    char texto[128];
    FILE *f = tmpfile();
    size_t n;
    if (f == NULL) return 0;
    imprimir(p, f);
    rewind(f);
    n = fread(texto, 1, sizeof texto - 1, f);
    texto[n] = '\0';
    fclose(f);
    return strcmp(texto, esperado) == 0;
}

static void testar_inicializar_pilha(void) {
    Pilha p;
    p.topo = (No *)&p;
    p.quantidade = 42;
    inicializar_pilha(&p);
    verificar("INI-01", "inicializar sobre lixo de memoria", "topo = NULL, quantidade = 0",
              p.topo == NULL && p.quantidade == 0);
    empilhar(&p, 2);
    empilhar(&p, 1);
    liberar_pilha(&p);
    verificar("INI-02", "liberar pilha com dois nos", "pilha vazia", conteudo(&p, ""));
    liberar_pilha(&p);
    verificar("INI-03", "liberar duas vezes", "pilha vazia, sem falha", conteudo(&p, ""));
}

static void testar_empilhar(void) {
    Pilha p;
    const No *antigo;
    int i, certo = 1;
    inicializar_pilha(&p);
    verificar("EMP-01", "empilhar 3 em pilha vazia", "[3] -> NULL",
              empilhar(&p, 3) && conteudo(&p, "3"));
    antigo = p.topo;
    verificar("EMP-02", "empilhar 2 sobre 3", "[2] -> [3] -> NULL",
              empilhar(&p, 2) && conteudo(&p, "23"));
    verificar("EMP-03", "referencia do novo no", "aponta para o antigo topo",
              p.topo && p.topo->proximo == antigo);
    liberar_pilha(&p);
    for (i = 0; i < 100000; i++)
        if (!empilhar(&p, i)) certo = 0;
    verificar("EMP-04", "100000 nos sem capacidade previa", "quantidade = 100000",
              certo && p.quantidade == 100000);
    liberar_pilha(&p);
}

static void testar_desempilhar(void) {
    Pilha p;
    const No *segundo;
    int d = 99;
    inicializar_pilha(&p);
    verificar("DES-01", "desempilhar pilha vazia", "falha, destino inalterado",
              !desempilhar(&p, &d) && d == 99 && conteudo(&p, ""));
    empilhar(&p, 3);
    empilhar(&p, 2);
    empilhar(&p, 1);
    segundo = p.topo ? p.topo->proximo : NULL;
    verificar("DES-02", "desempilhar de [1,2,3]", "retorna 1, fica [2,3]",
              desempilhar(&p, &d) && d == 1 && conteudo(&p, "23"));
    verificar("DES-03", "referencia do topo", "passa ao segundo no", p.topo == segundo);
    verificar("DES-04", "ordem LIFO", "depois saem 2 e 3",
              desempilhar(&p, &d) && d == 2 && desempilhar(&p, &d) && d == 3);
    verificar("DES-05", "retirar o unico no", "topo = NULL, quantidade = 0", conteudo(&p, ""));
    empilhar(&p, 4);
    verificar("DES-06", "destino NULL", "retira normalmente",
              desempilhar(&p, NULL) && conteudo(&p, ""));
    liberar_pilha(&p);
}

static void testar_topo(void) {
    Pilha p;
    int d = 99;
    inicializar_pilha(&p);
    verificar("TOP-01", "topo de pilha vazia", "falha, destino inalterado", !topo(&p, &d) && d == 99);
    empilhar(&p, 4);
    empilhar(&p, 2);
    verificar("TOP-02", "topo de [2,4]", "2", topo(&p, &d) && d == 2);
    verificar("TOP-03", "consultar nao retira", "continua [2,4]", conteudo(&p, "24"));
    desempilhar(&p, NULL);
    verificar("TOP-04", "topo apos desempilhar", "4", topo(&p, &d) && d == 4);
    liberar_pilha(&p);
}

static void testar_imprimir(void) {
    Pilha p;
    inicializar_pilha(&p);
    verificar("IMP-01", "imprimir pilha vazia", "NULL", impresso(&p, "NULL"));
    empilhar(&p, 3);
    verificar("IMP-02", "imprimir um disco", "[3] -> NULL", impresso(&p, "[3] -> NULL"));
    empilhar(&p, 1);
    verificar("IMP-03", "imprimir dois discos", "[1] -> [3] -> NULL",
              impresso(&p, "[1] -> [3] -> NULL"));
    verificar("IMP-04", "imprimir nao retira", "continua [1,3]", conteudo(&p, "13"));
    liberar_pilha(&p);
}

static void testar_partida(void) {
    Jogo j;
    inicializar_jogo(&j);
    verificar("PAR-01", "jogo recem-criado", "torres vazias, 0 movimentos", estado(&j, "", "", "", 0));
    verificar("PAR-02", "nova partida", "[1,2,3,4] na torre 1",
              iniciar_partida(&j) == OK && estado(&j, "1234", "", "", 0));
    jogar(&j, "1213");
    verificar("PAR-03", "reiniciar apos dois movimentos", "estado inicial, 0 movimentos",
              iniciar_partida(&j) == OK && estado(&j, "1234", "", "", 0));
    encerrar_partida(&j);
    verificar("PAR-04", "encerrar a partida", "torres vazias, 0 movimentos", estado(&j, "", "", "", 0));
}

static void testar_movimentos(void) {
    Jogo j;
    inicializar_jogo(&j);
    iniciar_partida(&j);
    verificar("MOV-01", "torre 1 para torre 2", "disco 1 na torre 2",
              mover(&j, 0, 1) == OK && estado(&j, "234", "1", "", 1));
    verificar("MOV-02", "disco maior sobre menor", "erro, nada muda",
              mover(&j, 0, 1) == ERRO_DISCO_MAIOR && estado(&j, "234", "1", "", 1));
    verificar("MOV-03", "origem vazia", "erro, nada muda",
              mover(&j, 2, 0) == ERRO_TORRE_VAZIA && estado(&j, "234", "1", "", 1));
    verificar("MOV-04", "origem igual ao destino", "erro, nada muda",
              mover(&j, 0, 0) == ERRO_MESMA_TORRE && estado(&j, "234", "1", "", 1));
    verificar("MOV-05", "origem inexistente", "erro, nada muda",
              mover(&j, -1, 0) == ERRO_TORRE_INEXISTENTE &&
              mover(&j, 3, 0) == ERRO_TORRE_INEXISTENTE && estado(&j, "234", "1", "", 1));
    verificar("MOV-06", "destino inexistente", "erro, nada muda",
              mover(&j, 0, -1) == ERRO_TORRE_INEXISTENTE &&
              mover(&j, 0, 3) == ERRO_TORRE_INEXISTENTE && estado(&j, "234", "1", "", 1));
    verificar("MOV-07", "segundo movimento valido", "contador = 2",
              mover(&j, 0, 2) == OK && estado(&j, "34", "1", "2", 2));
    verificar("MOV-08", "disco menor sobre maior", "aceita, contador = 3",
              mover(&j, 1, 2) == OK && estado(&j, "34", "", "12", 3));
    encerrar_partida(&j);
}

/*
 * Com 4 discos existem 3^4 = 81 estados. Em cada um sao tentados os 9 pares de
 * origem e destino, num total de 729 jogadas, comparadas com as regras.
 */
static void testar_todos_os_estados(void) {
    Jogo j;
    int e, o, d, i, resto, torre, aceitas = 0, certo = 1;
    inicializar_jogo(&j);
    for (e = 0; e < 81; e++) {
        for (o = 0; o < 3; o++) {
            for (d = 0; d < 3; d++) {
                int onde[5], menor[3] = {9, 9, 9};
                Status esperado, obtido;
                encerrar_partida(&j);
                for (i = 4, resto = e; i >= 1; i--, resto /= 3) {
                    torre = resto % 3;
                    onde[i] = torre;
                    empilhar(&j.torres[torre], i);
                    menor[torre] = i;
                }
                if (onde[1] == 2 && onde[2] == 2 && onde[3] == 2 && onde[4] == 2)
                    esperado = ERRO_PARTIDA_VENCIDA;
                else if (o == d) esperado = ERRO_MESMA_TORRE;
                else if (menor[o] == 9) esperado = ERRO_TORRE_VAZIA;
                else if (menor[o] > menor[d]) esperado = ERRO_DISCO_MAIOR;
                else esperado = OK;

                obtido = mover(&j, o, d);
                if (obtido != esperado) certo = 0;
                if (obtido == OK) {
                    onde[menor[o]] = d;
                    aceitas++;
                }
                for (torre = 0; torre < 3; torre++) {
                    char discos[5];
                    int n = 0;
                    for (i = 1; i <= 4; i++)
                        if (onde[i] == torre) discos[n++] = (char)('0' + i);
                    discos[n] = '\0';
                    if (!conteudo(&j.torres[torre], discos)) certo = 0;
                }
                if (j.movimentos != (obtido == OK)) certo = 0;
            }
        }
    }
    verificar("REG-01", "729 jogadas possiveis nos 81 estados", "todas conforme as regras", certo);
    /* O grafo tem 120 arestas (240 sentidos); os 2 que saem da vitoria nao valem. */
    verificar("REG-02", "total de jogadas aceitas", "238", aceitas == 238);
    encerrar_partida(&j);
}

static void testar_vitoria(void) {
    Jogo j;
    inicializar_jogo(&j);
    verificar("VIT-01", "jogo sem partida", "nao venceu", !venceu(&j));
    iniciar_partida(&j);
    verificar("VIT-02", "estado inicial", "nao venceu", !venceu(&j));
    jogar(&j, "131232132123131232312132131232"); /* leva tudo para a torre 2 */
    verificar("VIT-03", "todos os discos na torre 2", "nao venceu",
              !venceu(&j) && estado(&j, "", "1234", "", 15));
    iniciar_partida(&j);
    verificar("VIT-04", "solucao otima", "venceu com 15 movimentos",
              jogar(&j, SOLUCAO) && venceu(&j) && estado(&j, "", "", "1234", 15));
    verificar("VIT-05", "mover apos vencer", "erro, nada muda",
              mover(&j, 2, 0) == ERRO_PARTIDA_VENCIDA && estado(&j, "", "", "1234", 15));
    verificar("VIT-06", "nova partida apos vencer", "estado inicial",
              iniciar_partida(&j) == OK && !venceu(&j) && estado(&j, "1234", "", "", 0));
    verificar("VIT-07", "vencer sem ser no minimo", "venceu com 17 movimentos",
              jogar(&j, "1221") && jogar(&j, SOLUCAO) && venceu(&j) && j.movimentos == 17);
    encerrar_partida(&j);
}

int main(void) {
    testar_inicializar_pilha();
    testar_empilhar();
    testar_desempilhar();
    testar_topo();
    testar_imprimir();
    testar_partida();
    testar_movimentos();
    testar_todos_os_estados();
    testar_vitoria();
    printf("\nUnitarios: %d testes, %d falhas\n", total, falhas);
    return falhas != 0;
}
