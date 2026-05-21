#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "processo.h"
#include "escalonador.h"

int main(int argc, char *argv[]) {
    Processo processos[MAX_PROCESSOS];
    char algoritmo[50];
    int fatiaCPU;
    int quantidade;

    // Por padrao, mostra a execucao detalhada
    int modoDetalhado = 1;

    // Inicializa a aleatoriedade da loteria
    srand(time(NULL));

    // Verifica se o arquivo de entrada foi informado
    if (argc < 2) {
        printf("Uso correto:\n");
        printf("./escalonador entrada.txt\n");
        printf("./escalonador entrada.txt silencioso\n");
        return 1;
    }

    // Ativa o modo silencioso, se informado
    if (argc >= 3) {
        if (strcmp(argv[2], "silencioso") == 0) {
            modoDetalhado = 0;
        }
    }

    // Le o arquivo e carrega os processos
    if (!carregarArquivo(argv[1], algoritmo, &fatiaCPU, processos, &quantidade)) {
        return 1;
    }

    // Verifica se algum processo foi carregado
    if (quantidade == 0) {
        printf("Erro: nenhum processo foi carregado.\n");
        return 1;
    }

    // Executa o algoritmo de escalonamento
    executarEscalonador(algoritmo, fatiaCPU, processos, quantidade, modoDetalhado);

    return 0;
}