#ifndef PROCESSO_H
#define PROCESSO_H

// Estados que um processo pode ter
typedef enum {
    PROCESSO_FUTURO,
    PROCESSO_PRONTO,
    PROCESSO_EXECUTANDO,
    PROCESSO_FINALIZADO
} EstadoProcesso;

// Guarda todas as informacoes de um processo
typedef struct Processo {
    // Momento em que o processo entra no sistema
    int momentoCriacao;

    // Numero usado para identificar o processo
    int pid;

    // Tempo total que o processo precisa usar a CPU
    int tempoExecucao;

    // Pode ser prioridade, bilhetes ou peso
    int prioridadeOuBilhetes;

    // Quantidade de memoria que o processo precisa
    int quantidadeMemoria;

    // Ordem das paginas que o processo vai acessar
    int *sequenciaPaginas;

    // Quantidade de acessos que existem na sequencia
    int quantidadeAcessos;

    // Posicao do proximo acesso que vai ser feito
    int proximoAcesso;

    // Tempo que ainda falta para o processo terminar
    int tempoRestante;

    // Momento em que o processo terminou
    int tempoConclusao;

    // Tempo que o processo ficou esperando na fila
    int tempoPronto;

    // Momento em que ele entrou na CPU pela primeira vez
    int primeiraExecucao;

    // Tempo entre a criacao e a primeira execucao
    int tempoResposta;

    // Tempo virtual usado pelo CFS
    double vruntime;

    // Estado atual do processo
    EstadoProcesso estado;
} Processo;

// A estrutura completa da arvore fica em outro arquivo
typedef struct ArvoreRN ArvoreRN;

// Cria um processo novo
Processo *criarProcesso(
    int momentoCriacao,
    int pid,
    int tempoExecucao,
    int prioridadeOuBilhetes,
    int quantidadeMemoria,
    int sequenciaPaginas[],
    int quantidadeAcessos
);

// Libera o processo e a sequencia de paginas
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

// Soma um ciclo no tempo de espera
void incrementarTempoPronto(Processo *processo);

// Mostra os resultados finais dos processos
void mostrarResultadoFinal(ArvoreRN *processosPorPid);

#endif