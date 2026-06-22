#include <stdio.h>
#include <stdlib.h>
#include "processo.h"
#include "arvore_rubro_negra.h"

// Cria um processo novo na memoria
Processo *criarProcesso(
    int momentoCriacao,
    int pid,
    int tempoExecucao,
    int prioridadeOuBilhetes,
    int quantidadeMemoria,
    int sequenciaPaginas[],
    int quantidadeAcessos
) {
    Processo *novoProcesso;
    int i;

    // Sem a sequencia nao da para criar o processo direito
    if (sequenciaPaginas == NULL) {
        return NULL;
    }

    if (quantidadeAcessos <= 0) {
        return NULL;
    }

    novoProcesso = malloc(sizeof(Processo));

    if (novoProcesso == NULL) {
        return NULL;
    }

    // Cria um vetor so para guardar as paginas deste processo
    novoProcesso->sequenciaPaginas = malloc(
        sizeof(int) * quantidadeAcessos
    );

    if (novoProcesso->sequenciaPaginas == NULL) {
        free(novoProcesso);
        return NULL;
    }

    // Copia a sequencia recebida para dentro do processo
    i = 0;

    while (i < quantidadeAcessos) {
        novoProcesso->sequenciaPaginas[i] = sequenciaPaginas[i];
        i++;
    }

    novoProcesso->momentoCriacao = momentoCriacao;
    novoProcesso->pid = pid;
    novoProcesso->tempoExecucao = tempoExecucao;
    novoProcesso->prioridadeOuBilhetes = prioridadeOuBilhetes;

    novoProcesso->quantidadeMemoria = quantidadeMemoria;
    novoProcesso->quantidadeAcessos = quantidadeAcessos;
    novoProcesso->proximoAcesso = 0;

    novoProcesso->tempoRestante = tempoExecucao;
    novoProcesso->tempoConclusao = -1;
    novoProcesso->tempoPronto = 0;
    novoProcesso->primeiraExecucao = -1;
    novoProcesso->tempoResposta = 0;
    novoProcesso->vruntime = 0.0;

    // Quando ele e criado ainda nao chegou no sistema
    novoProcesso->estado = PROCESSO_FUTURO;

    return novoProcesso;
}

// Libera um processo da memoria
void destruirProcesso(Processo *processo) {
    if (processo == NULL) {
        return;
    }

    // Primeiro libera o vetor de paginas
    if (processo->sequenciaPaginas != NULL) {
        free(processo->sequenciaPaginas);
    }

    // Depois libera o processo inteiro
    free(processo);
}

// Verifica se o processo ja pode ir para pronto
int processoPodeFicarPronto(
    Processo *processo,
    int tempoAtual
) {
    if (processo == NULL) {
        return 0;
    }

    if (processo->estado != PROCESSO_FUTURO) {
        return 0;
    }

    if (processo->tempoRestante <= 0) {
        return 0;
    }

    if (processo->momentoCriacao > tempoAtual) {
        return 0;
    }

    return 1;
}

// Verifica se o processo acabou
int processoTerminou(Processo *processo) {
    if (processo == NULL) {
        return 0;
    }

    if (processo->tempoRestante <= 0) {
        return 1;
    }

    return 0;
}

// Coloca o processo como pronto
void deixarProcessoPronto(Processo *processo) {
    if (processo == NULL) {
        return;
    }

    if (processoTerminou(processo)) {
        return;
    }

    processo->estado = PROCESSO_PRONTO;
}

// Coloca o processo como executando
void deixarProcessoExecutando(Processo *processo) {
    if (processo == NULL) {
        return;
    }

    if (processoTerminou(processo)) {
        return;
    }

    processo->estado = PROCESSO_EXECUTANDO;
}

// Finaliza o processo
void finalizarProcesso(
    Processo *processo,
    int tempoAtual
) {
    if (processo == NULL) {
        return;
    }

    processo->tempoRestante = 0;
    processo->tempoConclusao = tempoAtual;
    processo->estado = PROCESSO_FINALIZADO;
}

// Soma um tempo na espera do processo
void incrementarTempoPronto(Processo *processo) {
    if (processo == NULL) {
        return;
    }

    // So soma se ele realmente estiver esperando a CPU
    if (processo->estado != PROCESSO_PRONTO) {
        return;
    }

    processo->tempoPronto++;
}

// Mostra os resultados finais dos processos
void mostrarResultadoFinal(ArvoreRN *processosPorPid) {
    NoRN *noAtual;
    NoRN *proximoNo;
    Processo *processo;

    int quantidade;
    int tempoTotal;

    double somaTempoTotal;
    double somaTempoPronto;
    double somaTempoResposta;

    if (processosPorPid == NULL) {
        printf("Erro: arvore de processos nao encontrada.\n");
        return;
    }

    if (arvoreEstaVazia(processosPorPid)) {
        printf("Nenhum processo para mostrar.\n");
        return;
    }

    quantidade = processosPorPid->quantidade;

    somaTempoTotal = 0.0;
    somaTempoPronto = 0.0;
    somaTempoResposta = 0.0;

    printf("\n================ RESULTADO FINAL ================\n");

    printf(
        "PID\tCriacao\tExecucao\tConclusao\tTotal\tPronto\tResposta\n"
    );

    noAtual = obterMenorNo(processosPorPid);

    // A arvore esta por PID, entao mostra na ordem dos PIDs
    while (noAtual != NULL) {
        processo = noAtual->processo;

        tempoTotal =
            processo->tempoConclusao -
            processo->momentoCriacao;

        somaTempoTotal += tempoTotal;
        somaTempoPronto += processo->tempoPronto;
        somaTempoResposta += processo->tempoResposta;

        printf(
            "%d\t%d\t%d\t\t%d\t\t%d\t%d\t%d\n",
            processo->pid,
            processo->momentoCriacao,
            processo->tempoExecucao,
            processo->tempoConclusao,
            tempoTotal,
            processo->tempoPronto,
            processo->tempoResposta
        );

        proximoNo = obterProximoNo(
            processosPorPid,
            noAtual
        );

        noAtual = proximoNo;
    }

    printf("\n================ MEDIAS ================\n");

    printf(
        "Tempo medio total: %.2f\n",
        somaTempoTotal / quantidade
    );

    printf(
        "Tempo medio pronto: %.2f\n",
        somaTempoPronto / quantidade
    );

    printf(
        "Tempo medio de resposta: %.2f\n",
        somaTempoResposta / quantidade
    );
}