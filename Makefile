CC ?= cc
CFLAGS ?= -std=c99 -Wall -Wextra -pedantic

MODULOS = src/pilha.c src/jogo.c
CABECALHOS = src/pilha.h src/jogo.h

all: build/hanoi

build/hanoi: src/main.c $(MODULOS) $(CABECALHOS)
	@mkdir -p build
	$(CC) $(CFLAGS) src/main.c $(MODULOS) -o $@

build/testes: tests/test_unitarios.c $(MODULOS) $(CABECALHOS)
	@mkdir -p build
	$(CC) $(CFLAGS) tests/test_unitarios.c $(MODULOS) -o $@

run: build/hanoi
	./build/hanoi

test: build/testes build/hanoi
	./build/testes
	sh tests/test_sistema.sh

clean:
	rm -rf build

.PHONY: all run test clean
