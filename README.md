# Torre de Hanói com pilhas dinâmicas

Jogo da Torre de Hanói em C, com 4 discos e três torres. Cada torre é uma pilha dinâmica implementada
manualmente com nós e ponteiros. Usa apenas a biblioteca padrão do C.

- [Compilação e execução](#compilação-e-execução)
- [Como jogar](#como-jogar)
- [A pilha e suas operações](#a-pilha-e-suas-operações)
- [Funcionalidades](#funcionalidades)
- [Exemplos de execução](#exemplos-de-execução)
- [Testes realizados](#testes-realizados)
- [Resultados dos testes](#resultados-dos-testes)

## Compilação e execução

É necessário um compilador C99 (GCC ou Clang).

Com Make (macOS e Linux):

```bash
make        # compila
make run    # executa
make test   # roda todos os testes
make clean  # apaga os binários
```

Sem Make:

```bash
mkdir -p build
cc -std=c99 src/main.c src/pilha.c src/jogo.c -o build/hanoi
./build/hanoi

cc -std=c99 tests/test_unitarios.c src/pilha.c src/jogo.c -o build/testes
./build/testes
sh tests/test_sistema.sh
```

No Windows, com MinGW:

```bat
gcc -std=c99 src/main.c src/pilha.c src/jogo.c -o hanoi.exe
hanoi.exe
```

## Como jogar

O objetivo é levar os 4 discos da torre 1 para a torre 3:

- somente o disco do topo pode ser movido;
- somente um disco é movido por vez;
- um disco maior não pode ficar sobre um menor.

Digite o número da torre de origem e depois o da torre de destino. Nas duas perguntas também valem:

| Comando | Efeito |
|:-:|---|
| `R` | reinicia a partida |
| `M` | encerra a partida e volta ao menu |
| `S` | encerra o jogo |

Com 4 discos, o mínimo é de 2⁴ − 1 = 15 movimentos.
Regras de referência: [Clubes de Matemática da OBMEP](https://clubes.obmep.org.br/blog/torre-de-hanoi/).

## Organização

```text
src/pilha.h, pilha.c      pilha dinâmica
src/jogo.h, jogo.c        regras e desenho das torres
src/main.c                menu e leitura das jogadas
tests/test_unitarios.c    testes da pilha e das regras
tests/test_sistema.sh     testes do jogo completo
```

## A pilha e suas operações

```c
typedef struct No {
    int disco;
    struct No *proximo;
} No;

typedef struct {
    No *topo;
    int quantidade;
} Pilha;
```

O jogo tem três pilhas. O número do disco é o seu tamanho (1 é o menor). No início da partida:

```text
Torre 1                           Torre 2           Torre 3

topo -> [1 | *]                   topo -> NULL      topo -> NULL
             |
             v
        [2 | *]
             |
             v
        [3 | *]
             |
             v
        [4 | *] -> NULL
```

Os discos são empilhados na ordem 4, 3, 2, 1. Como o último a entrar é o primeiro a sair (LIFO), o
disco 1 fica no topo.

| Operação | Retorno | Tempo |
|---|---|:-:|
| `inicializar_pilha(pilha)` | nenhum | O(1) |
| `empilhar(pilha, disco)` | 1, ou 0 se faltar memória | O(1) |
| `desempilhar(pilha, &disco)` | 1, ou 0 se estiver vazia | O(1) |
| `topo(pilha, &disco)` | 1, ou 0 se estiver vazia | O(1) |
| `imprimir(pilha, saida)` | nenhum | O(n) |

### `inicializar_pilha`

Cria o estado de pilha vazia: `topo = NULL` e `quantidade = 0`.

### `empilhar`

Cria um nó com `malloc` e o coloca no topo.

```text
antes:    topo -> [2] -> [3] -> NULL

1. novo->proximo = topo       [1] -> [2]
2. topo = novo
3. quantidade++

depois:   topo -> [1] -> [2] -> [3] -> NULL
```

Se os passos 1 e 2 fossem invertidos, o novo nó apontaria para si mesmo e o resto da pilha seria
perdido.

### `desempilhar`

Retira o nó do topo e devolve sua memória com `free`.

```text
antes:    topo -> [1] -> [2] -> [3] -> NULL

1. retirado = topo
2. topo = retirado->proximo
3. free(retirado)
4. quantidade--

depois:   topo -> [2] -> [3] -> NULL
```

O nó é guardado em `retirado` porque, depois do passo 2, o topo já é outro.

### `topo`

Informa o disco do topo sem retirá-lo. É o que permite validar uma jogada antes de alterar as pilhas.

### `imprimir`

Percorre os nós do topo até `NULL`, sem desempilhar, e escreve `[1] -> [2] -> NULL`.

### Como um movimento usa a pilha

1. `topo(origem)`: se não houver disco, o movimento é inválido.
2. `topo(destino)`: se o disco de lá for menor, o movimento é inválido.
3. `empilhar(destino, disco)`.
4. `desempilhar(origem)`.
5. `movimentos++`.

O passo 3 vem antes do 4 para que o disco não se perca se faltar memória ao empilhar.

`liberar_pilha` é uma função auxiliar, necessária em C, que desempilha até esvaziar.

## Funcionalidades

| Requisito | Como usar | Função | Testes |
|---|---|---|---|
| Iniciar nova partida | `1` no menu | `iniciar_partida` | PAR-02, SIS-01, SIS-02 |
| Visualizar as três torres | automático | `exibir_jogo`, `imprimir` | SIS-02 a SIS-05 |
| Escolher a origem | `1`, `2` ou `3` | `ler_jogada` | SIS-07, SIS-17 |
| Escolher o destino | `1`, `2` ou `3` | `ler_jogada` | SIS-08, SIS-20 |
| Realizar movimentos válidos | origem e destino | `mover` | MOV-01, MOV-08, SIS-09 |
| Mensagem de movimento inválido | automático | `mensagem_status` | SIS-13 a SIS-15 |
| Impedir retirada de torre vazia | automático | `topo`, `mover` | MOV-03, SIS-13 |
| Impedir maior sobre menor | automático | `topo`, `mover` | MOV-02, SIS-14 |
| Contar os movimentos válidos | automático | `mover` | MOV-07, SIS-11, SIS-16 |
| Saber quando venceu | automático | `venceu` | VIT-04, SIS-21 |
| Reiniciar a partida | `R` | `iniciar_partida` | PAR-03, SIS-26 a SIS-28 |
| Encerrar a partida | `M` | `encerrar_partida` | PAR-04, SIS-29 |
| Encerrar o jogo | `S`, ou `0` no menu | `sair` | SIS-31 a SIS-33 |

### Decisões de projeto

- A vitória só vale com todos os discos na **torre 3**, como pede o enunciado.
- Origem igual ao destino é um movimento inválido.
- Movimentos inválidos não alteram as torres nem o contador.
- Torre de origem vazia é avisada antes de perguntar o destino.
- Reiniciar zera o contador.

## Exemplos de execução

### Exemplo 1: movimentos válidos, inválidos, reinício e saída

```text
=== TORRE DE HANOI ===
Leve os 4 discos da torre 1 para a torre 3.
Mova um disco por vez e nunca ponha um maior sobre um menor.
 1. Nova partida
 0. Encerrar o jogo
Opcao: 1

Nova partida iniciada.
Comandos: R reinicia | M encerra a partida | S sai do jogo

      |          |          |
     =1=         |          |
    ==2==        |          |
   ===3===       |          |
  ====4====      |          |
  -------------------------------
   Torre 1    Torre 2    Torre 3

Torre 1: [1] -> [2] -> [3] -> [4] -> NULL
Torre 2: NULL
Torre 3: NULL
Movimentos: 0

Torre de origem (1-3): 1
Torre de destino (1-3): 2
Disco 1 movido da torre 1 para a torre 2.

      |          |          |
      |          |          |
    ==2==        |          |
   ===3===       |          |
  ====4====     =1=         |
  -------------------------------
   Torre 1    Torre 2    Torre 3

Torre 1: [2] -> [3] -> [4] -> NULL
Torre 2: [1] -> NULL
Torre 3: NULL
Movimentos: 1

Torre de origem (1-3): 2
Torre de destino (1-3): 2
Movimento invalido: origem e destino sao a mesma torre.

Torre de origem (1-3): 3
Movimento invalido: a torre de origem esta vazia.

Torre de origem (1-3): 1
Torre de destino (1-3): 2
Movimento invalido: um disco maior nao pode ficar sobre um menor.

Torre de origem (1-3): r
Partida reiniciada.

      |          |          |
     =1=         |          |
    ==2==        |          |
   ===3===       |          |
  ====4====      |          |
  -------------------------------
   Torre 1    Torre 2    Torre 3

Torre 1: [1] -> [2] -> [3] -> [4] -> NULL
Torre 2: NULL
Torre 3: NULL
Movimentos: 0

Torre de origem (1-3): s

Jogo encerrado.
```

### Exemplo 2: vitória em 15 movimentos

Os desenhos intermediários foram omitidos. As jogadas e o estado final são os da execução real.

```text
=== TORRE DE HANOI ===
Leve os 4 discos da torre 1 para a torre 3.
Mova um disco por vez e nunca ponha um maior sobre um menor.
 1. Nova partida
 0. Encerrar o jogo
Opcao: 1

Nova partida iniciada.
Comandos: R reinicia | M encerra a partida | S sai do jogo

      |          |          |
     =1=         |          |
    ==2==        |          |
   ===3===       |          |
  ====4====      |          |
  -------------------------------
   Torre 1    Torre 2    Torre 3

Torre 1: [1] -> [2] -> [3] -> [4] -> NULL
Torre 2: NULL
Torre 3: NULL
Movimentos: 0

Torre de origem (1-3): 1
Torre de destino (1-3): 2
Disco 1 movido da torre 1 para a torre 2.

Torre de origem (1-3): 1
Torre de destino (1-3): 3
Disco 2 movido da torre 1 para a torre 3.

Torre de origem (1-3): 2
Torre de destino (1-3): 3
Disco 1 movido da torre 2 para a torre 3.

Torre de origem (1-3): 1
Torre de destino (1-3): 2
Disco 3 movido da torre 1 para a torre 2.

Torre de origem (1-3): 3
Torre de destino (1-3): 1
Disco 1 movido da torre 3 para a torre 1.

Torre de origem (1-3): 3
Torre de destino (1-3): 2
Disco 2 movido da torre 3 para a torre 2.

Torre de origem (1-3): 1
Torre de destino (1-3): 2
Disco 1 movido da torre 1 para a torre 2.

Torre de origem (1-3): 1
Torre de destino (1-3): 3
Disco 4 movido da torre 1 para a torre 3.

Torre de origem (1-3): 2
Torre de destino (1-3): 3
Disco 1 movido da torre 2 para a torre 3.

Torre de origem (1-3): 2
Torre de destino (1-3): 1
Disco 2 movido da torre 2 para a torre 1.

Torre de origem (1-3): 3
Torre de destino (1-3): 1
Disco 1 movido da torre 3 para a torre 1.

Torre de origem (1-3): 2
Torre de destino (1-3): 3
Disco 3 movido da torre 2 para a torre 3.

Torre de origem (1-3): 1
Torre de destino (1-3): 2
Disco 1 movido da torre 1 para a torre 2.

Torre de origem (1-3): 1
Torre de destino (1-3): 3
Disco 2 movido da torre 1 para a torre 3.

Torre de origem (1-3): 2
Torre de destino (1-3): 3
Disco 1 movido da torre 2 para a torre 3.

      |          |          |
      |          |         =1=
      |          |        ==2==
      |          |       ===3===
      |          |      ====4====
  -------------------------------
   Torre 1    Torre 2    Torre 3

Torre 1: NULL
Torre 2: NULL
Torre 3: [1] -> [2] -> [3] -> [4] -> NULL
Movimentos: 15

Parabens, voce venceu em 15 movimentos!
Esse e o minimo possivel.
```

## Testes realizados

São **77 testes automatizados**: 42 unitários e 35 de sistema. Para repetir:

```bash
make test
```

O teste REG-01 é exaustivo: com 4 discos existem 3⁴ = 81 estados, e em cada um são tentados os
9 pares de origem e destino. São 729 jogadas, todas conferidas contra as regras.

### Pilha

| ID | Cenário | Resultado esperado | Resultado |
|---|---|---|:-:|
| INI-01 | inicializar sobre lixo de memoria | `topo = NULL, quantidade = 0` | Passou |
| INI-02 | liberar pilha com dois nos | `pilha vazia` | Passou |
| INI-03 | liberar duas vezes | `pilha vazia, sem falha` | Passou |
| EMP-01 | empilhar 3 em pilha vazia | `[3] -> NULL` | Passou |
| EMP-02 | empilhar 2 sobre 3 | `[2] -> [3] -> NULL` | Passou |
| EMP-03 | referencia do novo no | `aponta para o antigo topo` | Passou |
| EMP-04 | 100000 nos sem capacidade previa | `quantidade = 100000` | Passou |
| DES-01 | desempilhar pilha vazia | `falha, destino inalterado` | Passou |
| DES-02 | desempilhar de [1,2,3] | `retorna 1, fica [2,3]` | Passou |
| DES-03 | referencia do topo | `passa ao segundo no` | Passou |
| DES-04 | ordem LIFO | `depois saem 2 e 3` | Passou |
| DES-05 | retirar o unico no | `topo = NULL, quantidade = 0` | Passou |
| DES-06 | destino NULL | `retira normalmente` | Passou |
| TOP-01 | topo de pilha vazia | `falha, destino inalterado` | Passou |
| TOP-02 | topo de [2,4] | `2` | Passou |
| TOP-03 | consultar nao retira | `continua [2,4]` | Passou |
| TOP-04 | topo apos desempilhar | `4` | Passou |
| IMP-01 | imprimir pilha vazia | `NULL` | Passou |
| IMP-02 | imprimir um disco | `[3] -> NULL` | Passou |
| IMP-03 | imprimir dois discos | `[1] -> [3] -> NULL` | Passou |
| IMP-04 | imprimir nao retira | `continua [1,3]` | Passou |

### Regras do jogo

| ID | Cenário | Resultado esperado | Resultado |
|---|---|---|:-:|
| PAR-01 | jogo recem-criado | `torres vazias, 0 movimentos` | Passou |
| PAR-02 | nova partida | `[1,2,3,4] na torre 1` | Passou |
| PAR-03 | reiniciar apos dois movimentos | `estado inicial, 0 movimentos` | Passou |
| PAR-04 | encerrar a partida | `torres vazias, 0 movimentos` | Passou |
| MOV-01 | torre 1 para torre 2 | `disco 1 na torre 2` | Passou |
| MOV-02 | disco maior sobre menor | `erro, nada muda` | Passou |
| MOV-03 | origem vazia | `erro, nada muda` | Passou |
| MOV-04 | origem igual ao destino | `erro, nada muda` | Passou |
| MOV-05 | origem inexistente | `erro, nada muda` | Passou |
| MOV-06 | destino inexistente | `erro, nada muda` | Passou |
| MOV-07 | segundo movimento valido | `contador = 2` | Passou |
| MOV-08 | disco menor sobre maior | `aceita, contador = 3` | Passou |
| REG-01 | 729 jogadas possiveis nos 81 estados | `todas conforme as regras` | Passou |
| REG-02 | total de jogadas aceitas | `238` | Passou |
| VIT-01 | jogo sem partida | `nao venceu` | Passou |
| VIT-02 | estado inicial | `nao venceu` | Passou |
| VIT-03 | todos os discos na torre 2 | `nao venceu` | Passou |
| VIT-04 | solucao otima | `venceu com 15 movimentos` | Passou |
| VIT-05 | mover apos vencer | `erro, nada muda` | Passou |
| VIT-06 | nova partida apos vencer | `estado inicial` | Passou |
| VIT-07 | vencer sem ser no minimo | `venceu com 17 movimentos` | Passou |

### Sistema

Enviam entradas ao executável, como um jogador faria, e conferem a saída.

| ID | Cenário | Resultado esperado | Resultado |
|---|---|---|:-:|
| SIS-01 | iniciar nova partida | `partida iniciada` | Passou |
| SIS-02 | quantidade de discos | `4 discos na torre 1` | Passou |
| SIS-03 | torre 2 no inicio | `vazia` | Passou |
| SIS-04 | torre 3 no inicio | `vazia` | Passou |
| SIS-05 | desenho das torres | `disco 4 na base` | Passou |
| SIS-06 | contador no inicio | `0` | Passou |
| SIS-07 | escolher origem | `pede a origem` | Passou |
| SIS-08 | escolher destino | `pede o destino` | Passou |
| SIS-09 | movimento valido | `disco 1 na torre 2` | Passou |
| SIS-10 | origem apos mover | `[2,3,4]` | Passou |
| SIS-11 | contador apos mover | `1` | Passou |
| SIS-12 | empilhar menor sobre maior | `[1,2] na torre 3` | Passou |
| SIS-13 | retirar de torre vazia | `mensagem de erro` | Passou |
| SIS-14 | maior sobre menor | `mensagem de erro` | Passou |
| SIS-15 | mesma torre | `mensagem de erro` | Passou |
| SIS-16 | invalido nao conta | `contador = 2` | Passou |
| SIS-17 | torre inexistente | `pede de novo` | Passou |
| SIS-18 | texto como torre | `pede de novo` | Passou |
| SIS-19 | entrada vazia | `pede de novo` | Passou |
| SIS-20 | destino inexistente | `pede de novo` | Passou |
| SIS-21 | vencer a partida | `mensagem de vitoria` | Passou |
| SIS-22 | vitoria no minimo | `avisa que e o minimo` | Passou |
| SIS-23 | estado na vitoria | `4 discos na torre 3` | Passou |
| SIS-24 | vencer com mais movimentos | `informa o minimo` | Passou |
| SIS-25 | jogar de novo apos vencer | `nova contagem` | Passou |
| SIS-26 | reiniciar | `partida reiniciada` | Passou |
| SIS-27 | contador apos reiniciar | `zera e volta a contar` | Passou |
| SIS-28 | reiniciar ao pedir destino | `partida reiniciada` | Passou |
| SIS-29 | encerrar partida | `volta ao menu` | Passou |
| SIS-30 | nova partida apos encerrar | `nova contagem` | Passou |
| SIS-31 | encerrar jogo na partida | `programa termina` | Passou |
| SIS-32 | encerrar jogo ao pedir destino | `programa termina` | Passou |
| SIS-33 | encerrar jogo no menu | `programa termina` | Passou |
| SIS-34 | opcao invalida no menu | `pede de novo` | Passou |
| SIS-35 | fim inesperado da entrada | `encerra sem travar` | Passou |

## Resultados dos testes

| Grupo | Executados | Passaram | Falharam |
|---|:-:|:-:|:-:|
| Pilha | 21 | 21 | 0 |
| Regras do jogo | 21 | 21 | 0 |
| Sistema | 35 | 35 | 0 |
| **Total** | 77 | 77 | 0 |

Ambiente da execução: macOS 26.5.1 (arm64), Apple clang version 21.0.0 (clang-2100.1.1.101).

<details>
<summary>Saída completa de <code>make test</code></summary>

```text
[OK] INI-01 inicializar sobre lixo de memoria -> topo = NULL, quantidade = 0
[OK] INI-02 liberar pilha com dois nos -> pilha vazia
[OK] INI-03 liberar duas vezes -> pilha vazia, sem falha
[OK] EMP-01 empilhar 3 em pilha vazia -> [3] -> NULL
[OK] EMP-02 empilhar 2 sobre 3 -> [2] -> [3] -> NULL
[OK] EMP-03 referencia do novo no -> aponta para o antigo topo
[OK] EMP-04 100000 nos sem capacidade previa -> quantidade = 100000
[OK] DES-01 desempilhar pilha vazia -> falha, destino inalterado
[OK] DES-02 desempilhar de [1,2,3] -> retorna 1, fica [2,3]
[OK] DES-03 referencia do topo -> passa ao segundo no
[OK] DES-04 ordem LIFO -> depois saem 2 e 3
[OK] DES-05 retirar o unico no -> topo = NULL, quantidade = 0
[OK] DES-06 destino NULL -> retira normalmente
[OK] TOP-01 topo de pilha vazia -> falha, destino inalterado
[OK] TOP-02 topo de [2,4] -> 2
[OK] TOP-03 consultar nao retira -> continua [2,4]
[OK] TOP-04 topo apos desempilhar -> 4
[OK] IMP-01 imprimir pilha vazia -> NULL
[OK] IMP-02 imprimir um disco -> [3] -> NULL
[OK] IMP-03 imprimir dois discos -> [1] -> [3] -> NULL
[OK] IMP-04 imprimir nao retira -> continua [1,3]
[OK] PAR-01 jogo recem-criado -> torres vazias, 0 movimentos
[OK] PAR-02 nova partida -> [1,2,3,4] na torre 1
[OK] PAR-03 reiniciar apos dois movimentos -> estado inicial, 0 movimentos
[OK] PAR-04 encerrar a partida -> torres vazias, 0 movimentos
[OK] MOV-01 torre 1 para torre 2 -> disco 1 na torre 2
[OK] MOV-02 disco maior sobre menor -> erro, nada muda
[OK] MOV-03 origem vazia -> erro, nada muda
[OK] MOV-04 origem igual ao destino -> erro, nada muda
[OK] MOV-05 origem inexistente -> erro, nada muda
[OK] MOV-06 destino inexistente -> erro, nada muda
[OK] MOV-07 segundo movimento valido -> contador = 2
[OK] MOV-08 disco menor sobre maior -> aceita, contador = 3
[OK] REG-01 729 jogadas possiveis nos 81 estados -> todas conforme as regras
[OK] REG-02 total de jogadas aceitas -> 238
[OK] VIT-01 jogo sem partida -> nao venceu
[OK] VIT-02 estado inicial -> nao venceu
[OK] VIT-03 todos os discos na torre 2 -> nao venceu
[OK] VIT-04 solucao otima -> venceu com 15 movimentos
[OK] VIT-05 mover apos vencer -> erro, nada muda
[OK] VIT-06 nova partida apos vencer -> estado inicial
[OK] VIT-07 vencer sem ser no minimo -> venceu com 17 movimentos

Unitarios: 42 testes, 0 falhas

[OK] SIS-01 iniciar nova partida -> partida iniciada
[OK] SIS-02 quantidade de discos -> 4 discos na torre 1
[OK] SIS-03 torre 2 no inicio -> vazia
[OK] SIS-04 torre 3 no inicio -> vazia
[OK] SIS-05 desenho das torres -> disco 4 na base
[OK] SIS-06 contador no inicio -> 0
[OK] SIS-07 escolher origem -> pede a origem
[OK] SIS-08 escolher destino -> pede o destino
[OK] SIS-09 movimento valido -> disco 1 na torre 2
[OK] SIS-10 origem apos mover -> [2,3,4]
[OK] SIS-11 contador apos mover -> 1
[OK] SIS-12 empilhar menor sobre maior -> [1,2] na torre 3
[OK] SIS-13 retirar de torre vazia -> mensagem de erro
[OK] SIS-14 maior sobre menor -> mensagem de erro
[OK] SIS-15 mesma torre -> mensagem de erro
[OK] SIS-16 invalido nao conta -> contador = 2
[OK] SIS-17 torre inexistente -> pede de novo
[OK] SIS-18 texto como torre -> pede de novo
[OK] SIS-19 entrada vazia -> pede de novo
[OK] SIS-20 destino inexistente -> pede de novo
[OK] SIS-21 vencer a partida -> mensagem de vitoria
[OK] SIS-22 vitoria no minimo -> avisa que e o minimo
[OK] SIS-23 estado na vitoria -> 4 discos na torre 3
[OK] SIS-24 vencer com mais movimentos -> informa o minimo
[OK] SIS-25 jogar de novo apos vencer -> nova contagem
[OK] SIS-26 reiniciar -> partida reiniciada
[OK] SIS-27 contador apos reiniciar -> zera e volta a contar
[OK] SIS-28 reiniciar ao pedir destino -> partida reiniciada
[OK] SIS-29 encerrar partida -> volta ao menu
[OK] SIS-30 nova partida apos encerrar -> nova contagem
[OK] SIS-31 encerrar jogo na partida -> programa termina
[OK] SIS-32 encerrar jogo ao pedir destino -> programa termina
[OK] SIS-33 encerrar jogo no menu -> programa termina
[OK] SIS-34 opcao invalida no menu -> pede de novo
[OK] SIS-35 fim inesperado da entrada -> encerra sem travar

Sistema: 35 testes, 0 falhas
```

</details>
