#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "processo.h"
#include "escalonador.h"

// Escolhe o proximo processo no Round Robin
int escolherRoundRobin(Processo processos[], int quantidade, int tempoAtual, int *ultimoIndice) {
    int contador;
    int indice;

    // Procura a partir do ultimo processo executado
    for (contador = 1; contador <= quantidade; contador++) {
        indice = (*ultimoIndice + contador) % quantidade;

        // Retorna o primeiro processo pronto encontrado
        if (processoEstaPronto(processos, indice, tempoAtual)) {
            *ultimoIndice = indice;
            return indice;
        }
    }

    return -1;
}

// Escolhe o processo com maior prioridade
int escolherPrioridade(Processo processos[], int quantidade, int tempoAtual) {
    int i;
    int escolhido = -1;

    for (i = 0; i < quantidade; i++) {
        if (processoEstaPronto(processos, i, tempoAtual)) {
            if (escolhido == -1) {
                escolhido = i;
            } else {
                // Menor numero significa maior prioridade
                if (processos[i].prioridadeOuBilhetes < processos[escolhido].prioridadeOuBilhetes) {
                    escolhido = i;
                } else if (processos[i].prioridadeOuBilhetes == processos[escolhido].prioridadeOuBilhetes) {
                    // Em caso de empate, escolhe quem foi criado antes
                    if (processos[i].momentoCriacao < processos[escolhido].momentoCriacao) {
                        escolhido = i;
                    } else if (processos[i].momentoCriacao == processos[escolhido].momentoCriacao) {
                        // Se ainda empatar, escolhe o menor PID
                        if (processos[i].pid < processos[escolhido].pid) {
                            escolhido = i;
                        }
                    }
                }
            }
        }
    }

    return escolhido;
}

// Escolhe um processo por sorteio de bilhetes
int escolherLoteria(
    Processo processos[],
    int quantidade,
    int tempoAtual,
    int modoDetalhado
) {
    int i;
    int totalBilhetes = 0;
    int sorteado;
    int acumulado = 0;

    // Soma os bilhetes dos processos prontos
    for (i = 0; i < quantidade; i++) {
        if (processoEstaPronto(processos, i, tempoAtual)) {
            if (processos[i].prioridadeOuBilhetes <= 0) {
                totalBilhetes += 1;
            } else {
                totalBilhetes += processos[i].prioridadeOuBilhetes;
            }
        }
    }

    // Nenhum processo pronto
    if (totalBilhetes == 0) {
        return -1;
    }

    // Sorteia um bilhete
    sorteado = (rand() % totalBilhetes) + 1;

    if (modoDetalhado) {
        printf("\nLoteria no tempo %d\n", tempoAtual);
        printf("Total de bilhetes: %d\n", totalBilhetes);
        printf("Bilhete sorteado: %d\n", sorteado);
    }

    // Procura o dono do bilhete sorteado
    for (i = 0; i < quantidade; i++) {
        if (processoEstaPronto(processos, i, tempoAtual)) {
            if (processos[i].prioridadeOuBilhetes <= 0) {
                acumulado += 1;
            } else {
                acumulado += processos[i].prioridadeOuBilhetes;
            }

            if (sorteado <= acumulado) {
                if (modoDetalhado) {
                    printf("Processo escolhido pela loteria: PID %d\n", processos[i].pid);
                }

                return i;
            }
        }
    }
    return -1;
}

// Escolhe o processo com menor vruntime
int escolherCFS(
    Processo processos[],
    int quantidade,
    int tempoAtual
) {
    int i;
    int escolhido = -1;

    for (i = 0; i < quantidade; i++) {
        if (processoEstaPronto(processos, i, tempoAtual)) {
            if (escolhido == -1) {
                escolhido = i;
            } else {
                // Menor vruntime executa primeiro
                if (processos[i].vruntime < processos[escolhido].vruntime) {
                    escolhido = i;
                } else if (processos[i].vruntime == processos[escolhido].vruntime) {
                    // Em caso de empate, escolhe quem foi criado antes
                    if (processos[i].momentoCriacao < processos[escolhido].momentoCriacao) {
                        escolhido = i;
                    } else if (processos[i].momentoCriacao == processos[escolhido].momentoCriacao) {
                        // Se ainda empatar, escolhe o menor PID
                        if (processos[i].pid < processos[escolhido].pid) {
                            escolhido = i;
                        }
                    }
                }
            }
        }
    }

    return escolhido;
}

// Chama o algoritmo escolhido no arquivo
int escolherProcesso(
    char algoritmo[],
    Processo processos[],
    int quantidade,
    int tempoAtual,
    int *ultimoIndiceRR,
    int modoDetalhado
) {
    if (strcmp(algoritmo, "RR") == 0 || strcmp(algoritmo, "ALTERNANCIA") == 0 || strcmp(algoritmo, "ALTERNANCIA_CIRCULAR") == 0) {
        return escolherRoundRobin(processos, quantidade, tempoAtual, ultimoIndiceRR);
    }
    if (strcmp(algoritmo, "PRIORIDADE") == 0) {
        return escolherPrioridade(processos, quantidade, tempoAtual);
    }
    if (strcmp(algoritmo, "LOTERIA") == 0) {
        return escolherLoteria(processos, quantidade, tempoAtual, modoDetalhado);
    }
    if (strcmp(algoritmo, "CFS") == 0) {
        return escolherCFS(processos, quantidade, tempoAtual);
    }
    return -1;
}
// Executa a simulacao do escalonador
void executarEscalonador(char algoritmo[], int fatiaCPU, Processo processos[], int quantidade, int modoDetalhado) {
    int tempoAtual = 0;
    int ultimoIndiceRR = -1;
    int indiceExecutando;
    int tempoNaCPU;
    double peso;

    printf("\nAlgoritmo: %s\n", algoritmo);
    printf("Fatia de CPU: %d\n", fatiaCPU);

    if (modoDetalhado) {
        printf("\n================ EXECUCAO ================\n");
    }

    // Executa enquanto existir processo inacabado
    while (aindaExisteProcesso(processos, quantidade)) {
        // Escolhe o proximo processo
        indiceExecutando = escolherProcesso(algoritmo, processos, quantidade, tempoAtual, &ultimoIndiceRR, modoDetalhado);

        // Se nao houver processo pronto, a CPU fica ociosa
        if (indiceExecutando == -1) {
            if (modoDetalhado) {
                printf("Tempo %d: CPU ociosa\n", tempoAtual);
            }

            tempoAtual++;
            continue;
        }

        // Registra a primeira vez que entrou na CPU
        if (processos[indiceExecutando].primeiraExecucao == -1) {
            processos[indiceExecutando].primeiraExecucao = tempoAtual;
            processos[indiceExecutando].tempoResposta =
                tempoAtual - processos[indiceExecutando].momentoCriacao;
        }

        if (modoDetalhado) {
            printf("\nTempo %d: PID %d entrou na CPU\n",
                tempoAtual,
                processos[indiceExecutando].pid
            );
        }

        tempoNaCPU = 0;

        // Executa ate acabar a fatia ou o processo terminar
        while (tempoNaCPU < fatiaCPU && processos[indiceExecutando].tempoRestante > 0) {
            processos[indiceExecutando].tempoRestante--;

            // Atualiza o tempo de espera dos outros processos
            incrementarTempoPronto(processos, quantidade, tempoAtual, indiceExecutando);

            if (modoDetalhado) {
                printf("Tempo %d -> %d: PID %d executando | falta %d\n",
                    tempoAtual,
                    tempoAtual + 1,
                    processos[indiceExecutando].pid,
                    processos[indiceExecutando].tempoRestante
                );
            }

            tempoAtual++;
            tempoNaCPU++;

            // Atualiza o tempo virtual do CFS
            if (strcmp(algoritmo, "CFS") == 0) {
                peso = processos[indiceExecutando].prioridadeOuBilhetes;
                processos[indiceExecutando].vruntime += 1.0 / peso;
            }
        }

        if (modoDetalhado) {
            printf("Tempo %d: PID %d saiu da CPU\n",
                tempoAtual,
                processos[indiceExecutando].pid
            );
        }

        // Marca o tempo de conclusao
        if (processos[indiceExecutando].tempoRestante == 0 &&
            processos[indiceExecutando].tempoConclusao == -1) {

            processos[indiceExecutando].tempoConclusao = tempoAtual;

            if (modoDetalhado) {
                printf(">>> PID %d terminou no tempo %d\n",
                    processos[indiceExecutando].pid,
                    tempoAtual
                );
            }
        }
    }

    // Mostra os dados finais
    mostrarResultadoFinal(processos, quantidade);
}