#ifndef PROCESSO_H
#define PROCESSO_H

// Limite maximo de processos lidos
#define MAX_PROCESSOS 100

// Tamanho maximo de uma linha do arquivo
#define TAM_LINHA 200

// Representa um processo do simulador
typedef struct {
    // Tempo em que o processo aparece no sistema
    int momentoCriacao;

    // Identificador unico do processo
    int pid;

    // Tempo total que o processo precisa executar
    int tempoExecucao;

    // Prioridade, bilhetes ou peso, dependendo do algoritmo
    int prioridadeOuBilhetes;

    // Tempo que ainda falta para terminar
    int tempoRestante;

    // Tempo em que o processo terminou
    int tempoConclusao;

    // Tempo que o processo ficou pronto esperando CPU
    int tempoPronto;

    // Primeiro momento em que entrou na CPU
    int primeiraExecucao;

    // Tempo entre criacao e primeira execucao
    int tempoResposta;

    // Tempo virtual usado no CFS
    double vruntime;
} Processo;

// Remove a quebra de linha de uma string
void removerQuebraLinha(char texto[]);

// Converte uma string para maiusculo
void paraMaiusculo(char texto[]);

// Verifica se o algoritmo informado existe
int algoritmoValido(char algoritmo[]);

// Verifica se um PID ja foi cadastrado
int pidJaExiste(Processo processos[], int quantidade, int pid);

// Verifica se o processo pode executar no tempo atual
int processoEstaPronto(Processo processos[], int i, int tempoAtual);

// Verifica se ainda existe processo para executar
int aindaExisteProcesso(Processo processos[], int quantidade);

// Soma tempo pronto para os processos que esperam CPU
void incrementarTempoPronto(
    Processo processos[],
    int quantidade,
    int tempoAtual,
    int indiceExecutando
);

// Mostra os resultados finais da execucao
void mostrarResultadoFinal(Processo processos[], int quantidade);

// Le o arquivo de entrada e carrega os processos
int carregarArquivo(
    char nomeArquivo[],
    char algoritmo[],
    int *fatiaCPU,
    Processo processos[],
    int *quantidade
);

#endif