#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "processo.h"
#include "arvore_rubro_negra.h"
#include "escalonador.h"

// Verifica se o algoritmo escolhido e Round Robin
static int algoritmoEhRoundRobin(char algoritmo[]) {
    if (algoritmo == NULL) {
        return 0;
    }

    if (strcmp(algoritmo, "RR") == 0) {
        return 1;
    }

    if (strcmp(algoritmo, "ALTERNANCIA") == 0) {
        return 1;
    }

    if (strcmp(algoritmo, "ALTERNANCIA_CIRCULAR") == 0) {
        return 1;
    }

    return 0;
}

// Retira o menor processo da arvore de prontos
static Processo *retirarMenorProcessoDosProntos(
    Escalonador *escalonador
) {
    NoRN *menorNo;
    Processo *processo;

    if (escalonador == NULL) {
        return NULL;
    }

    menorNo = obterMenorNo(
        &escalonador->processosProntos
    );

    if (menorNo == NULL) {
        return NULL;
    }

    processo = menorNo->processo;

    if (
        !removerNoDaArvore(
            &escalonador->processosProntos,
            menorNo
        )
    ) {
        return NULL;
    }

    return processo;
}

// Gera um numero maior usando dois rand
// Isso ajuda quando tem bastante bilhete
static long long gerarNumeroAleatorioGrande(void) {
    long long primeiraParte;
    long long segundaParte;
    long long valor;

    primeiraParte = rand();
    segundaParte = rand();

    valor =
        primeiraParte *
        ((long long) RAND_MAX + 1);

    valor = valor + segundaParte;

    return valor;
}

// Sorteia um bilhete entre 1 e o total
static long long sortearBilhete(long long totalBilhetes) {
    long long numeroAleatorio;
    long long bilhete;

    if (totalBilhetes <= 0) {
        return 0;
    }

    numeroAleatorio = gerarNumeroAleatorioGrande();

    bilhete =
        (numeroAleatorio % totalBilhetes) + 1;

    return bilhete;
}

// Prepara uma fila vazia
void inicializarFilaRoundRobin(FilaRR *fila) {
    if (fila == NULL) {
        return;
    }

    fila->inicio = NULL;
    fila->fim = NULL;
    fila->quantidade = 0;
}

// Verifica se a fila esta vazia
int filaRoundRobinEstaVazia(FilaRR *fila) {
    if (fila == NULL) {
        return 1;
    }

    if (fila->inicio == NULL) {
        return 1;
    }

    return 0;
}

// Coloca um processo no fim da fila
int colocarNaFilaRoundRobin(
    FilaRR *fila,
    Processo *processo
) {
    NoFilaRR *novoNo;

    if (
        fila == NULL ||
        processo == NULL
    ) {
        return 0;
    }

    novoNo = malloc(sizeof(NoFilaRR));

    if (novoNo == NULL) {
        return 0;
    }

    novoNo->processo = processo;
    novoNo->proximo = NULL;

    // A fila ainda estava vazia
    if (fila->fim == NULL) {
        fila->inicio = novoNo;
        fila->fim = novoNo;
    } else {
        fila->fim->proximo = novoNo;
        fila->fim = novoNo;
    }

    fila->quantidade++;

    return 1;
}

// Retira o primeiro processo da fila
Processo *retirarDaFilaRoundRobin(FilaRR *fila) {
    NoFilaRR *primeiroNo;
    Processo *processo;

    if (filaRoundRobinEstaVazia(fila)) {
        return NULL;
    }

    primeiroNo = fila->inicio;
    processo = primeiroNo->processo;

    fila->inicio = primeiroNo->proximo;

    // Se tirou o ultimo, o fim tambem fica vazio
    if (fila->inicio == NULL) {
        fila->fim = NULL;
    }

    free(primeiroNo);

    fila->quantidade--;

    return processo;
}

// Libera os nos da fila
void destruirFilaRoundRobin(FilaRR *fila) {
    Processo *processo;

    if (fila == NULL) {
        return;
    }

    while (!filaRoundRobinEstaVazia(fila)) {
        processo = retirarDaFilaRoundRobin(fila);

        // O processo nao e liberado aqui
        // Ele continua guardado na arvore por PID
        (void) processo;
    }

    fila->inicio = NULL;
    fila->fim = NULL;
    fila->quantidade = 0;
}

// Escolhe como a arvore de prontos vai ficar organizada
TipoOrdenacaoArvore escolherOrdenacaoDosProntos(
    char algoritmo[]
) {
    if (algoritmo == NULL) {
        return ORDENAR_POR_PID;
    }

    if (strcmp(algoritmo, "PRIORIDADE") == 0) {
        return ORDENAR_POR_PRIORIDADE;
    }

    if (strcmp(algoritmo, "CFS") == 0) {
        return ORDENAR_POR_VRUNTIME;
    }

    // Round Robin e loteria podem ficar por PID
    return ORDENAR_POR_PID;
}

// Prepara tudo que o escalonador vai usar
int inicializarEscalonador(
    Escalonador *escalonador,
    char algoritmo[],
    int fatiaCPU,
    ArvoreRN *processosPorPid,
    ArvoreRN *processosFuturos,
    int quantidadeTotal,
    int modoDetalhado
) {
    TipoOrdenacaoArvore ordenacao;

    if (
        escalonador == NULL ||
        algoritmo == NULL ||
        processosPorPid == NULL ||
        processosFuturos == NULL
    ) {
        return 0;
    }

    if (fatiaCPU <= 0) {
        return 0;
    }

    if (quantidadeTotal <= 0) {
        return 0;
    }

    if (strlen(algoritmo) >= sizeof(escalonador->algoritmo)) {
        return 0;
    }

    escalonador->processosPorPid = processosPorPid;
    escalonador->processosFuturos = processosFuturos;

    escalonador->processoExecutando = NULL;

    strcpy(
        escalonador->algoritmo,
        algoritmo
    );

    escalonador->fatiaCPU = fatiaCPU;
    escalonador->tempoAtual = 0;

    escalonador->quantidadeTotal = quantidadeTotal;
    escalonador->quantidadeFinalizados = 0;

    escalonador->modoDetalhado = modoDetalhado;

    inicializarFilaRoundRobin(
        &escalonador->filaRoundRobin
    );

    ordenacao = escolherOrdenacaoDosProntos(
        algoritmo
    );

    inicializarArvore(
        &escalonador->processosProntos,
        ordenacao
    );

    // Se o malloc da arvore deu errado
    if (escalonador->processosProntos.nulo == NULL) {
        return 0;
    }

    return 1;
}

// Move os processos que ja chegaram para pronto
int moverProcessosQueChegaram(
    Escalonador *escalonador
) {
    NoRN *menorNo;
    Processo *processo;

    int colocouNaArvore;
    int colocouNaFila;

    if (escalonador == NULL) {
        return 0;
    }

    while (
        !arvoreEstaVazia(
            escalonador->processosFuturos
        )
    ) {
        menorNo = obterMenorNo(
            escalonador->processosFuturos
        );

        if (menorNo == NULL) {
            return 0;
        }

        processo = menorNo->processo;

        // O proximo processo ainda nao chegou
        if (
            processo->momentoCriacao >
            escalonador->tempoAtual
        ) {
            break;
        }

        if (
            !processoPodeFicarPronto(
                processo,
                escalonador->tempoAtual
            )
        ) {
            printf(
                "Erro: PID %d esta na arvore de futuros com estado errado.\n",
                processo->pid
            );

            return 0;
        }

        // Tira da arvore de futuros
        if (
            !removerNoDaArvore(
                escalonador->processosFuturos,
                menorNo
            )
        ) {
            printf(
                "Erro ao retirar o PID %d da arvore de futuros.\n",
                processo->pid
            );

            return 0;
        }

        deixarProcessoPronto(processo);

        colocouNaArvore = inserirProcessoNaArvore(
            &escalonador->processosProntos,
            processo
        );

        if (!colocouNaArvore) {
            printf(
                "Erro ao colocar o PID %d na arvore de prontos.\n",
                processo->pid
            );

            processo->estado = PROCESSO_FUTURO;

            inserirProcessoNaArvore(
                escalonador->processosFuturos,
                processo
            );

            return 0;
        }

        // Round Robin tambem precisa colocar na fila
        if (
            algoritmoEhRoundRobin(
                escalonador->algoritmo
            )
        ) {
            colocouNaFila = colocarNaFilaRoundRobin(
                &escalonador->filaRoundRobin,
                processo
            );

            if (!colocouNaFila) {
                printf(
                    "Erro ao colocar o PID %d na fila do Round Robin.\n",
                    processo->pid
                );

                removerProcessoDaArvore(
                    &escalonador->processosProntos,
                    processo
                );

                processo->estado = PROCESSO_FUTURO;

                inserirProcessoNaArvore(
                    escalonador->processosFuturos,
                    processo
                );

                return 0;
            }
        }

        if (escalonador->modoDetalhado) {
            printf(
                "Tempo %d: PID %d ficou pronto\n",
                escalonador->tempoAtual,
                processo->pid
            );
        }
    }

    return 1;
}

// Escolhe o proximo do Round Robin
Processo *escolherRoundRobin(
    Escalonador *escalonador
) {
    Processo *processo;

    if (escalonador == NULL) {
        return NULL;
    }

    processo = retirarDaFilaRoundRobin(
        &escalonador->filaRoundRobin
    );

    if (processo == NULL) {
        return NULL;
    }

    // Tambem tira da arvore de prontos
    if (
        !removerProcessoDaArvore(
            &escalonador->processosProntos,
            processo
        )
    ) {
        // Tenta devolver para a fila
        colocarNaFilaRoundRobin(
            &escalonador->filaRoundRobin,
            processo
        );

        return NULL;
    }

    return processo;
}

// Escolhe o processo de maior prioridade
Processo *escolherPrioridade(
    Escalonador *escalonador
) {
    if (escalonador == NULL) {
        return NULL;
    }

    // Menor numero significa maior prioridade
    return retirarMenorProcessoDosProntos(
        escalonador
    );
}

// Escolhe um processo pela loteria
Processo *escolherLoteria(
    Escalonador *escalonador
) {
    Processo *processo;

    long long totalBilhetes;
    long long bilheteSorteado;

    if (escalonador == NULL) {
        return NULL;
    }

    totalBilhetes = obterTotalBilhetes(
        &escalonador->processosProntos
    );

    if (totalBilhetes <= 0) {
        return NULL;
    }

    bilheteSorteado = sortearBilhete(
        totalBilhetes
    );

    processo = buscarProcessoPorBilhete(
        &escalonador->processosProntos,
        bilheteSorteado
    );

    if (processo == NULL) {
        return NULL;
    }

    if (escalonador->modoDetalhado) {
        printf(
            "\nLoteria no tempo %d\n",
            escalonador->tempoAtual
        );

        printf(
            "Total de bilhetes: %lld\n",
            totalBilhetes
        );

        printf(
            "Bilhete sorteado: %lld\n",
            bilheteSorteado
        );

        printf(
            "Processo escolhido pela loteria: PID %d\n",
            processo->pid
        );
    }

    // Tira da arvore enquanto ele usa a CPU
    if (
        !removerProcessoDaArvore(
            &escalonador->processosProntos,
            processo
        )
    ) {
        return NULL;
    }

    return processo;
}

// Escolhe o menor vruntime
Processo *escolherCFS(
    Escalonador *escalonador
) {
    if (escalonador == NULL) {
        return NULL;
    }

    return retirarMenorProcessoDosProntos(
        escalonador
    );
}

// Chama o algoritmo certo
Processo *escolherProximoProcesso(
    Escalonador *escalonador
) {
    if (escalonador == NULL) {
        return NULL;
    }

    if (
        algoritmoEhRoundRobin(
            escalonador->algoritmo
        )
    ) {
        return escolherRoundRobin(
            escalonador
        );
    }

    if (
        strcmp(
            escalonador->algoritmo,
            "PRIORIDADE"
        ) == 0
    ) {
        return escolherPrioridade(
            escalonador
        );
    }

    if (
        strcmp(
            escalonador->algoritmo,
            "LOTERIA"
        ) == 0
    ) {
        return escolherLoteria(
            escalonador
        );
    }

    if (
        strcmp(
            escalonador->algoritmo,
            "CFS"
        ) == 0
    ) {
        return escolherCFS(
            escalonador
        );
    }

    return NULL;
}

// Essa funcao ficou aqui porque esta no arquivo .h
// Nao precisa mais percorrer todos os processos a cada tempo
// O tempo pronto e calculado quando o processo termina
void atualizarTempoDosProcessosProntos(
    Escalonador *escalonador
) {
    if (escalonador == NULL) {
        return;
    }

    // Nao faz nada de proposito
    // Evita percorrer a arvore inteira toda hora
}

// Devolve um processo que ainda nao acabou
int devolverProcessoParaProntos(
    Escalonador *escalonador,
    Processo *processo
) {
    int colocouNaArvore;
    int colocouNaFila;

    if (
        escalonador == NULL ||
        processo == NULL
    ) {
        return 0;
    }

    if (processoTerminou(processo)) {
        return 0;
    }

    deixarProcessoPronto(processo);

    colocouNaArvore = inserirProcessoNaArvore(
        &escalonador->processosProntos,
        processo
    );

    if (!colocouNaArvore) {
        printf(
            "Erro ao devolver o PID %d para a arvore de prontos.\n",
            processo->pid
        );

        return 0;
    }

    // No Round Robin volta para o fim da fila
    if (
        algoritmoEhRoundRobin(
            escalonador->algoritmo
        )
    ) {
        colocouNaFila = colocarNaFilaRoundRobin(
            &escalonador->filaRoundRobin,
            processo
        );

        if (!colocouNaFila) {
            printf(
                "Erro ao devolver o PID %d para a fila.\n",
                processo->pid
            );

            removerProcessoDaArvore(
                &escalonador->processosProntos,
                processo
            );

            return 0;
        }
    }

    return 1;
}

// Executa uma fatia de CPU
void executarFatiaDeCPU(
    Escalonador *escalonador,
    Processo *processo
) {
    int tempoNaCPU;
    double peso;

    if (
        escalonador == NULL ||
        processo == NULL
    ) {
        return;
    }

    escalonador->processoExecutando = processo;

    deixarProcessoExecutando(processo);

    // Guarda a primeira vez que entrou na CPU
    if (processo->primeiraExecucao == -1) {
        processo->primeiraExecucao =
            escalonador->tempoAtual;

        processo->tempoResposta =
            escalonador->tempoAtual -
            processo->momentoCriacao;
    }

    if (escalonador->modoDetalhado) {
        printf(
            "\nTempo %d: PID %d entrou na CPU\n",
            escalonador->tempoAtual,
            processo->pid
        );
    }

    tempoNaCPU = 0;

    while (
        tempoNaCPU < escalonador->fatiaCPU &&
        processo->tempoRestante > 0
    ) {
        processo->tempoRestante--;

        if (escalonador->modoDetalhado) {
            printf(
                "Tempo %d -> %d: PID %d executando | falta %d\n",
                escalonador->tempoAtual,
                escalonador->tempoAtual + 1,
                processo->pid,
                processo->tempoRestante
            );
        }

        escalonador->tempoAtual++;
        tempoNaCPU++;

        // O CFS muda o vruntime enquanto executa
        if (
            strcmp(
                escalonador->algoritmo,
                "CFS"
            ) == 0
        ) {
            peso = processo->prioridadeOuBilhetes;

            if (peso <= 0.0) {
                peso = 1.0;
            }

            processo->vruntime =
                processo->vruntime +
                (1.0 / peso);
        }

        // Processos podem chegar enquanto outro esta na CPU
        if (
            !moverProcessosQueChegaram(
                escalonador
            )
        ) {
            printf(
                "Erro ao mover os processos que chegaram.\n"
            );

            escalonador->quantidadeFinalizados = -1;
            escalonador->processoExecutando = NULL;

            return;
        }
    }

    if (escalonador->modoDetalhado) {
        printf(
            "Tempo %d: PID %d saiu da CPU\n",
            escalonador->tempoAtual,
            processo->pid
        );
    }

    // O processo acabou
    if (processoTerminou(processo)) {
        finalizarProcesso(
            processo,
            escalonador->tempoAtual
        );

        // Como ainda nao tem entrada e saida, da para calcular direto
        processo->tempoPronto =
            processo->tempoConclusao -
            processo->momentoCriacao -
            processo->tempoExecucao;

        if (processo->tempoPronto < 0) {
            processo->tempoPronto = 0;
        }

        escalonador->quantidadeFinalizados++;

        if (escalonador->modoDetalhado) {
            printf(
                ">>> PID %d terminou no tempo %d\n",
                processo->pid,
                escalonador->tempoAtual
            );
        }
    } else {
        // Ainda falta tempo, entao volta para pronto
        if (
            !devolverProcessoParaProntos(
                escalonador,
                processo
            )
        ) {
            printf(
                "Erro ao devolver o PID %d para pronto.\n",
                processo->pid
            );

            escalonador->quantidadeFinalizados = -1;
            escalonador->processoExecutando = NULL;

            return;
        }
    }

    escalonador->processoExecutando = NULL;
}

// Libera as partes internas do escalonador
void destruirEscalonador(Escalonador *escalonador) {
    if (escalonador == NULL) {
        return;
    }

    destruirFilaRoundRobin(
        &escalonador->filaRoundRobin
    );

    // Libera so os nos
    // Os processos continuam na arvore por PID
    destruirArvore(
        &escalonador->processosProntos,
        0
    );

    escalonador->processosPorPid = NULL;
    escalonador->processosFuturos = NULL;
    escalonador->processoExecutando = NULL;

    escalonador->quantidadeTotal = 0;
    escalonador->quantidadeFinalizados = 0;
}

// Executa a simulacao inteira
void executarEscalonador(
    char algoritmo[],
    int fatiaCPU,
    ArvoreRN *processosPorPid,
    ArvoreRN *processosFuturos,
    int quantidadeTotal,
    int modoDetalhado
) {
    Escalonador escalonador;

    Processo *processo;
    Processo *proximoProcessoFuturo;

    int iniciou;

    iniciou = inicializarEscalonador(
        &escalonador,
        algoritmo,
        fatiaCPU,
        processosPorPid,
        processosFuturos,
        quantidadeTotal,
        modoDetalhado
    );

    if (!iniciou) {
        printf(
            "Erro: nao foi possivel iniciar o escalonador.\n"
        );

        return;
    }

    printf(
        "\nAlgoritmo: %s\n",
        escalonador.algoritmo
    );

    printf(
        "Fatia de CPU: %d\n",
        escalonador.fatiaCPU
    );

    if (escalonador.modoDetalhado) {
        printf(
            "\n================ EXECUCAO ================\n"
        );
    }

    // Move os processos criados no tempo zero
    if (
        !moverProcessosQueChegaram(
            &escalonador
        )
    ) {
        printf(
            "Erro ao preparar os primeiros processos.\n"
        );

        destruirEscalonador(
            &escalonador
        );

        return;
    }

    while (
        escalonador.quantidadeFinalizados <
        escalonador.quantidadeTotal
    ) {
        // Valor negativo significa que deu erro grave
        if (escalonador.quantidadeFinalizados < 0) {
            break;
        }

        // Confere novamente se chegou alguem
        if (
            !moverProcessosQueChegaram(
                &escalonador
            )
        ) {
            printf(
                "Erro ao mover os processos para pronto.\n"
            );

            escalonador.quantidadeFinalizados = -1;
            break;
        }

        processo = escolherProximoProcesso(
            &escalonador
        );

        // Nao tem processo pronto agora
        if (processo == NULL) {
            // Procura o proximo processo que ainda vai chegar
            proximoProcessoFuturo = obterMenorProcesso(
                escalonador.processosFuturos
            );

            // Ainda tem processo futuro
            if (proximoProcessoFuturo != NULL) {
                if (
                    proximoProcessoFuturo->momentoCriacao >
                    escalonador.tempoAtual
                ) {
                    if (escalonador.modoDetalhado) {
                        printf(
                            "Tempo %d ate %d: CPU ociosa\n",
                            escalonador.tempoAtual,
                            proximoProcessoFuturo->momentoCriacao
                        );
                    }

                    // Pula direto para a proxima chegada
                    escalonador.tempoAtual =
                        proximoProcessoFuturo->momentoCriacao;
                }

                continue;
            }

            // Se nao tem pronto nem futuro, alguma coisa ficou errada
            printf(
                "Erro: nao existe processo pronto, mas a simulacao nao terminou.\n"
            );

            escalonador.quantidadeFinalizados = -1;
            break;
        }

        executarFatiaDeCPU(
            &escalonador,
            processo
        );
    }

    if (
        escalonador.quantidadeFinalizados ==
        escalonador.quantidadeTotal
    ) {
        mostrarResultadoFinal(
            escalonador.processosPorPid
        );
    } else {
        printf(
            "A simulacao foi encerrada por causa de um erro.\n"
        );
    }

    destruirEscalonador(
        &escalonador
    );
}