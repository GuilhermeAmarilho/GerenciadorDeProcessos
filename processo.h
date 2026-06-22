#ifndef PROCESSO_H
#define PROCESSO_H

// Tamanho maximo de uma linha do arquivo
#define TAM_LINHA 200

// Estados que um processo pode ter
typedef enum {
    PROCESSO_FUTURO,
    PROCESSO_PRONTO,
    PROCESSO_EXECUTANDO,
    PROCESSO_FINALIZADO
} EstadoProcesso;

// Representa um processo do simulador
typedef struct Processo {
    // Tempo em que o processo aparece no sistema
    int momentoCriacao;

    // Identificador unico do processo
    int pid;

    // Tempo total que ele precisa executar
    int tempoExecucao;

    // Prioridade, bilhetes ou peso
    int prioridadeOuBilhetes;

    // Tempo que ainda falta para terminar
    int tempoRestante;

    // Tempo em que ele terminou
    int tempoConclusao;

    // Tempo que ficou esperando a CPU
    int tempoPronto;

    // Primeira vez que entrou na CPU
    int primeiraExecucao;

    // Tempo entre a criacao e a primeira execucao
    int tempoResposta;

    // Tempo virtual usado pelo CFS
    double vruntime;

    // Estado atual do processo
    EstadoProcesso estado;
} Processo;

// Avisando que existe uma estrutura de arvore
// A estrutura completa fica no arquivo da arvore
typedef struct ArvoreRN ArvoreRN;

// Remove a quebra de linha de uma string
void removerQuebraLinha(char texto[]);

// Converte uma string para maiusculo
void paraMaiusculo(char texto[]);

// Verifica se o algoritmo informado existe
int algoritmoValido(char algoritmo[]);

// Cria um processo novo na memoria
Processo *criarProcesso(
    int momentoCriacao,
    int pid,
    int tempoExecucao,
    int prioridadeOuBilhetes
);

// Libera um processo da memoria
void destruirProcesso(Processo *processo);

// Verifica se o processo ja pode entrar na fila de prontos
int processoPodeFicarPronto(
    Processo *processo,
    int tempoAtual
);

// Verifica se o processo terminou
int processoTerminou(Processo *processo);

// Coloca o processo no estado pronto
void deixarProcessoPronto(Processo *processo);

// Coloca o processo no estado executando
void deixarProcessoExecutando(Processo *processo);

// Coloca o processo no estado finalizado
void finalizarProcesso(
    Processo *processo,
    int tempoAtual
);

// Soma um tempo no estado pronto
void incrementarTempoPronto(Processo *processo);

// Mostra os resultados finais
// A arvore deve estar organizada por PID
void mostrarResultadoFinal(ArvoreRN *processosPorPid);

// Le o arquivo e coloca os processos nas arvores
int carregarArquivo(
    char nomeArquivo[],
    char algoritmo[],
    int *fatiaCPU,
    ArvoreRN *processosPorPid,
    ArvoreRN *processosFuturos,
    int *quantidade
);

#endif