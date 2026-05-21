#ifndef ESCALONADOR_H
#define ESCALONADOR_H

#include "processo.h"

// Escolhe o proximo processo no Round Robin
int escolherRoundRobin(
    Processo processos[],
    int quantidade,
    int tempoAtual,
    int *ultimoIndice
);

// Escolhe o processo com maior prioridade
int escolherPrioridade(
    Processo processos[],
    int quantidade,
    int tempoAtual
);

// Escolhe um processo por sorteio de bilhetes
int escolherLoteria(
    Processo processos[],
    int quantidade,
    int tempoAtual,
    int modoDetalhado
);

// Escolhe o processo com menor vruntime
int escolherCFS(
    Processo processos[],
    int quantidade,
    int tempoAtual
);

// Escolhe qual algoritmo sera usado
int escolherProcesso(
    char algoritmo[],
    Processo processos[],
    int quantidade,
    int tempoAtual,
    int *ultimoIndiceRR,
    int modoDetalhado
);

// Executa a simulacao do escalonador
void executarEscalonador(
    char algoritmo[],
    int fatiaCPU,
    Processo processos[],
    int quantidade,
    int modoDetalhado
);

#endif