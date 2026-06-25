#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "processo.h"

// Remove \n ou \r do final da linha
void removerQuebraLinha(char texto[]) {
    int i = 0;

    while (texto[i] != '\0') {
        if (texto[i] == '\n' || texto[i] == '\r') {
            texto[i] = '\0';
            return;
        }

        i++;
    }
}

// Converte o texto para letras maiusculas
void paraMaiusculo(char texto[]) {
    int i = 0;

    while (texto[i] != '\0') {
        texto[i] = toupper(texto[i]);
        i++;
    }
}

// Verifica se o algoritmo informado e aceito
int algoritmoValido(char algoritmo[]) {
    if (strcmp(algoritmo, "RR") == 0) {
        return 1;
    }

    if (strcmp(algoritmo, "ALTERNANCIA") == 0) {
        return 1;
    }

    if (strcmp(algoritmo, "ALTERNANCIA_CIRCULAR") == 0) {
        return 1;
    }

    if (strcmp(algoritmo, "PRIORIDADE") == 0) {
        return 1;
    }

    if (strcmp(algoritmo, "LOTERIA") == 0) {
        return 1;
    }

    if (strcmp(algoritmo, "CFS") == 0) {
        return 1;
    }

    return 0;
}

// Verifica se o PID ja existe no vetor
int pidJaExiste(Processo processos[], int quantidade, int pid) {
    int i;

    for (i = 0; i < quantidade; i++) {
        if (processos[i].pid == pid) {
            return 1;
        }
    }

    return 0;
}

// Verifica se o processo ja foi criado e ainda nao terminou
int processoEstaPronto(Processo processos[], int i, int tempoAtual) {
    if (processos[i].momentoCriacao <= tempoAtual && processos[i].tempoRestante > 0) {
        return 1;
    }

    return 0;
}

// Verifica se ainda existe processo com tempo restante
int aindaExisteProcesso(Processo processos[], int quantidade) {
    int i;

    for (i = 0; i < quantidade; i++) {
        if (processos[i].tempoRestante > 0) {
            return 1;
        }
    }

    return 0;
}

// Atualiza o tempo pronto dos processos que estao esperando
void incrementarTempoPronto(
    Processo processos[],
    int quantidade,
    int tempoAtual,
    int indiceExecutando
) {
    int i;

    for (i = 0; i < quantidade; i++) {
        if (i != indiceExecutando) {
            if (processos[i].momentoCriacao <= tempoAtual && processos[i].tempoRestante > 0) {
                processos[i].tempoPronto++;
            }
        }
    }
}

// Mostra os resultados finais dos processos
void mostrarResultadoFinal(Processo processos[], int quantidade) {
    int i;
    int tempoTotal;

    double somaTempoTotal = 0;
    double somaTempoPronto = 0;
    double somaTempoResposta = 0;

    printf("\n================ RESULTADO FINAL ================\n");
    printf("PID\tCriacao\tExecucao\tConclusao\tTotal\tPronto\tResposta\n");

    for (i = 0; i < quantidade; i++) {
        // Tempo total entre criacao e conclusao
        tempoTotal = processos[i].tempoConclusao - processos[i].momentoCriacao;

        // Soma os valores para calcular as medias
        somaTempoTotal += tempoTotal;
        somaTempoPronto += processos[i].tempoPronto;
        somaTempoResposta += processos[i].tempoResposta;

        printf("%d\t%d\t%d\t\t%d\t\t%d\t%d\t%d\n",
            processos[i].pid,
            processos[i].momentoCriacao,
            processos[i].tempoExecucao,
            processos[i].tempoConclusao,
            tempoTotal,
            processos[i].tempoPronto,
            processos[i].tempoResposta
        );
    }

    printf("\n================ MEDIAS ================\n");
    printf("Tempo medio total: %.2f\n", somaTempoTotal / quantidade);
    printf("Tempo medio pronto: %.2f\n", somaTempoPronto / quantidade);
    printf("Tempo medio de resposta: %.2f\n", somaTempoResposta / quantidade);
}

// Le o arquivo e carrega os dados dos processos
int carregarArquivo(
    char nomeArquivo[],
    char algoritmo[],
    int *fatiaCPU,
    Processo processos[],
    int *quantidade
) {
    FILE *arquivo;
    char linha[TAM_LINHA];
    char *parte;
    int numeroLinha = 1;
    int pidLido;

    // Abre o arquivo de entrada
    arquivo = fopen(nomeArquivo, "r");

    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo: %s\n", nomeArquivo);
        return 0;
    }

    // Le a primeira linha com algoritmo e fatia de CPU
    if (fgets(linha, TAM_LINHA, arquivo) == NULL) {
        printf("Arquivo vazio.\n");
        fclose(arquivo);
        return 0;
    }

    removerQuebraLinha(linha);

    // Separa o nome do algoritmo
    parte = strtok(linha, "|");
    if (parte == NULL) {
        printf("Erro na primeira linha do arquivo.\n");
        fclose(arquivo);
        return 0;
    }

    strcpy(algoritmo, parte);
    paraMaiusculo(algoritmo);

    // Valida o algoritmo escolhido
    if (!algoritmoValido(algoritmo)) {
        printf("Erro: algoritmo desconhecido: %s\n", algoritmo);
        printf("Use: RR, PRIORIDADE, LOTERIA ou CFS.\n");
        fclose(arquivo);
        return 0;
    }

    // Separa a fatia de CPU
    parte = strtok(NULL, "|");
    if (parte == NULL) {
        printf("Erro: fatia de CPU nao encontrada.\n");
        fclose(arquivo);
        return 0;
    }

    *fatiaCPU = atoi(parte);

    // Valida a fatia de CPU
    if (*fatiaCPU <= 0) {
        printf("Erro: a fatia de CPU precisa ser maior que zero.\n");
        fclose(arquivo);
        return 0;
    }

    *quantidade = 0;

    // Le os processos do arquivo
    while (fgets(linha, TAM_LINHA, arquivo) != NULL) {
        numeroLinha++;
        removerQuebraLinha(linha);

        // Ignora linhas vazias
        if (strlen(linha) == 0) {
            continue;
        }

        // Le o momento de criacao
        parte = strtok(linha, "|");
        if (parte == NULL) {
            printf("Erro na linha %d: momento de criacao ausente.\n", numeroLinha);
            fclose(arquivo);
            return 0;
        }

        processos[*quantidade].momentoCriacao = atoi(parte);

        // Le o PID
        parte = strtok(NULL, "|");
        if (parte == NULL) {
            printf("Erro na linha %d: PID ausente.\n", numeroLinha);
            fclose(arquivo);
            return 0;
        }

        pidLido = atoi(parte);

        // Verifica se o PID ja foi usado
        if (pidJaExiste(processos, *quantidade, pidLido)) {
            printf("Erro na linha %d: PID repetido: %d\n", numeroLinha, pidLido);
            fclose(arquivo);
            return 0;
        }

        processos[*quantidade].pid = pidLido;

        // Le o tempo de execucao
        parte = strtok(NULL, "|");
        if (parte == NULL) {
            printf("Erro na linha %d: tempo de execucao ausente.\n", numeroLinha);
            fclose(arquivo);
            return 0;
        }

        processos[*quantidade].tempoExecucao = atoi(parte);

        // Valida o tempo de execucao
        if (processos[*quantidade].tempoExecucao <= 0) {
            printf("Erro na linha %d: tempo de execucao invalido.\n", numeroLinha);
            fclose(arquivo);
            return 0;
        }

        // Le prioridade, peso ou bilhetes
        parte = strtok(NULL, "|");
        if (parte == NULL) {
            printf("Erro na linha %d: prioridade, peso ou bilhetes ausente.\n", numeroLinha);
            fclose(arquivo);
            return 0;
        }

        processos[*quantidade].prioridadeOuBilhetes = atoi(parte);

        // Garante valor minimo para o ultimo campo
        if (processos[*quantidade].prioridadeOuBilhetes <= 0) {
            printf("Aviso na linha %d: ultimo campo invalido. Sera usado valor 1.\n", numeroLinha);
            processos[*quantidade].prioridadeOuBilhetes = 1;
        }

        // Inicializa os dados de controle do processo
        processos[*quantidade].tempoRestante = processos[*quantidade].tempoExecucao;
        processos[*quantidade].tempoConclusao = -1;
        processos[*quantidade].tempoPronto = 0;
        processos[*quantidade].primeiraExecucao = -1;
        processos[*quantidade].tempoResposta = 0;
        processos[*quantidade].vruntime = 0.0;

        // Avanca para a proxima posicao do vetor
        (*quantidade)++;

        // Evita ultrapassar o limite do vetor
        if (*quantidade >= MAX_PROCESSOS) {
            printf("Limite maximo de processos atingido.\n");
            break;
        }
    }

    // Fecha o arquivo
    fclose(arquivo);
    return 1;
}