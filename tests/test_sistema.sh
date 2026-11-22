#!/bin/sh
# Executa o jogo com entradas prontas e confere a saida de cada cenario.
# Uso: sh tests/test_sistema.sh [executavel]

programa=${1:-build/hanoi}
total=0
falhas=0

# testar ID CENARIO ESPERADO ENTRADA TRECHO_DA_SAIDA [TRECHO_PROIBIDO]
# O trecho proibido distingue um comando de sair do simples fim da entrada.
testar() {
    total=$((total + 1))
    if saida=$(printf "$4" | "$programa" 2>&1) &&
        printf '%s\n' "$saida" | grep -Fq -- "$5" &&
        { [ -z "$6" ] || ! printf '%s\n' "$saida" | grep -Fq -- "$6"; }; then
        echo "[OK] $1 $2 -> $3"
    else
        falhas=$((falhas + 1))
        echo "[FALHA] $1 $2 -> $3"
    fi
}

# A solucao otima, um "origem destino" por movimento.
VENCER='1\n2\n1\n3\n2\n3\n1\n2\n3\n1\n3\n2\n1\n2\n1\n3\n2\n3\n2\n1\n3\n1\n2\n3\n1\n2\n1\n3\n2\n3\n'

testar SIS-01 "iniciar nova partida" "partida iniciada" '1\ns\n' "Nova partida iniciada"
testar SIS-02 "quantidade de discos" "4 discos na torre 1" '1\ns\n' "Torre 1: [1] -> [2] -> [3] -> [4] -> NULL"
testar SIS-03 "torre 2 no inicio" "vazia" '1\ns\n' "Torre 2: NULL"
testar SIS-04 "torre 3 no inicio" "vazia" '1\ns\n' "Torre 3: NULL"
testar SIS-05 "desenho das torres" "disco 4 na base" '1\ns\n' "====4===="
testar SIS-06 "contador no inicio" "0" '1\ns\n' "Movimentos: 0"
testar SIS-07 "escolher origem" "pede a origem" '1\ns\n' "Torre de origem"
testar SIS-08 "escolher destino" "pede o destino" '1\n1\ns\n' "Torre de destino"
testar SIS-09 "movimento valido" "disco 1 na torre 2" '1\n1\n2\ns\n' "Torre 2: [1] -> NULL"
testar SIS-10 "origem apos mover" "[2,3,4]" '1\n1\n2\ns\n' "Torre 1: [2] -> [3] -> [4] -> NULL"
testar SIS-11 "contador apos mover" "1" '1\n1\n2\ns\n' "Movimentos: 1"
testar SIS-12 "empilhar menor sobre maior" "[1,2] na torre 3" '1\n1\n2\n1\n3\n2\n3\ns\n' "Torre 3: [1] -> [2] -> NULL"
testar SIS-13 "retirar de torre vazia" "mensagem de erro" '1\n2\ns\n' "Movimento invalido: a torre de origem esta vazia"
testar SIS-14 "maior sobre menor" "mensagem de erro" '1\n1\n2\n1\n2\ns\n' "Movimento invalido: um disco maior nao pode ficar sobre um menor"
testar SIS-15 "mesma torre" "mensagem de erro" '1\n1\n1\ns\n' "Movimento invalido: origem e destino sao a mesma torre"
testar SIS-16 "invalido nao conta" "contador = 2" '1\n1\n2\n1\n2\n1\n3\ns\n' "Movimentos: 2"
testar SIS-17 "torre inexistente" "pede de novo" '1\n4\ns\n' "Entrada invalida"
testar SIS-18 "texto como torre" "pede de novo" '1\nabc\ns\n' "Entrada invalida"
testar SIS-19 "entrada vazia" "pede de novo" '1\n\ns\n' "Entrada invalida"
testar SIS-20 "destino inexistente" "pede de novo" '1\n1\n0\ns\n' "Entrada invalida"
testar SIS-21 "vencer a partida" "mensagem de vitoria" "1\n${VENCER}0\n" "Parabens, voce venceu em 15 movimentos"
testar SIS-22 "vitoria no minimo" "avisa que e o minimo" "1\n${VENCER}0\n" "Esse e o minimo possivel"
testar SIS-23 "estado na vitoria" "4 discos na torre 3" "1\n${VENCER}0\n" "Torre 3: [1] -> [2] -> [3] -> [4] -> NULL"
testar SIS-24 "vencer com mais movimentos" "informa o minimo" "1\n1\n2\n2\n1\n${VENCER}0\n" "O minimo possivel e 15"
testar SIS-25 "jogar de novo apos vencer" "nova contagem" "1\n${VENCER}1\n1\n2\n2\n1\n${VENCER}0\n" "voce venceu em 17 movimentos"
testar SIS-26 "reiniciar" "partida reiniciada" '1\n1\n2\nr\ns\n' "Partida reiniciada"
testar SIS-27 "contador apos reiniciar" "zera e volta a contar" "1\n1\n2\nr\n${VENCER}0\n" "voce venceu em 15 movimentos"
testar SIS-28 "reiniciar ao pedir destino" "partida reiniciada" '1\n1\nR\ns\n' "Partida reiniciada"
testar SIS-29 "encerrar partida" "volta ao menu" '1\n1\n2\nm\n0\n' "Partida encerrada. Movimentos feitos: 1"
testar SIS-30 "nova partida apos encerrar" "nova contagem" "1\n1\n2\nm\n1\n${VENCER}0\n" "voce venceu em 15 movimentos"
testar SIS-31 "encerrar jogo na partida" "programa termina" '1\ns\n' "Jogo encerrado" "Fim da entrada"
testar SIS-32 "encerrar jogo ao pedir destino" "programa termina" '1\n1\nS\n' "Jogo encerrado" "Fim da entrada"
testar SIS-33 "encerrar jogo no menu" "programa termina" '0\n' "Jogo encerrado" "Fim da entrada"
testar SIS-34 "opcao invalida no menu" "pede de novo" '7\n0\n' "Opcao invalida"
testar SIS-35 "fim inesperado da entrada" "encerra sem travar" '1\n1\n' "Fim da entrada"

echo
echo "Sistema: $total testes, $falhas falhas"
[ "$falhas" -eq 0 ]
