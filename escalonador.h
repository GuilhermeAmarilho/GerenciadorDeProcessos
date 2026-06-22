#ifndef ESCALONADOR_H
#define ESCALONADOR_H
#include "processo.h"
#include "arvore_rubro_negra.h"

// No usado na fila do Round Robin
typedef struct NoFilaRR {
    // Processo guardado nessa posicao
    Processo *processo;

    // Proximo processo da fila
    struct NoFilaRR *proximo;
} NoFilaRR;

// Fila usada pelo Round Robin
typedef struct {
    // Primeiro processo da fila
    NoFilaRR *inicio;

    // Ultimo processo da fila
    NoFilaRR *fim;

    // Quantidade de processos na fila
    int quantidade;
} FilaRR;

// Junta as estruturas usadas durante a simulacao
typedef struct {
    // Arvore com todos os processos
    // Ela fica organizada por PID
    ArvoreRN *processosPorPid;

    // Processos que ainda nao chegaram
    ArvoreRN *processosFuturos;

    // Processos que ja podem executar
    ArvoreRN processosProntos;

    // Fila usada somente no Round Robin
    FilaRR filaRoundRobin;

    // Processo que esta usando a CPU
    Processo *processoExecutando;

    // Nome do algoritmo atual
    char algoritmo[50];

    // Tempo que cada processo pode ficar na CPU
    int fatiaCPU;

    // Relogio da simulacao
    int tempoAtual;

    // Quantidade total de processos
    int quantidadeTotal;

    // Quantidade de processos que terminaram
    int quantidadeFinalizados;

    // Mostra ou esconde os detalhes da execucao
    int modoDetalhado;
} Escalonador;

// Prepara uma fila vazia
void inicializarFilaRoundRobin(FilaRR *fila);

// Verifica se a fila esta vazia
int filaRoundRobinEstaVazia(FilaRR *fila);

// Coloca um processo no final da fila
int colocarNaFilaRoundRobin(
    FilaRR *fila,
    Processo *processo
);

// Retira o primeiro processo da fila
Processo *retirarDaFilaRoundRobin(FilaRR *fila);

// Libera os nos usados pela fila
// Os processos nao sao liberados aqui
void destruirFilaRoundRobin(FilaRR *fila);

// Escolhe a ordenacao da arvore de prontos
TipoOrdenacaoArvore escolherOrdenacaoDosProntos(
    char algoritmo[]
);

// Prepara as estruturas usadas pelo escalonador
int inicializarEscalonador(
    Escalonador *escalonador,
    char algoritmo[],
    int fatiaCPU,
    ArvoreRN *processosPorPid,
    ArvoreRN *processosFuturos,
    int quantidadeTotal,
    int modoDetalhado
);

// Move os processos que chegaram para o estado pronto
int moverProcessosQueChegaram(
    Escalonador *escalonador
);

// Escolhe o proximo processo no Round Robin
Processo *escolherRoundRobin(
    Escalonador *escalonador
);

// Escolhe o processo com maior prioridade
Processo *escolherPrioridade(
    Escalonador *escalonador
);

// Escolhe um processo usando os bilhetes
Processo *escolherLoteria(
    Escalonador *escalonador
);

// Escolhe o processo com menor vruntime
Processo *escolherCFS(
    Escalonador *escalonador
);

// Chama a escolha certa dependendo do algoritmo
Processo *escolherProximoProcesso(
    Escalonador *escalonador
);

// Soma o tempo pronto dos processos que estao esperando
void atualizarTempoDosProcessosProntos(
    Escalonador *escalonador
);

// Executa uma fatia de CPU
void executarFatiaDeCPU(
    Escalonador *escalonador,
    Processo *processo
);

// Coloca de volta um processo que ainda nao terminou
int devolverProcessoParaProntos(
    Escalonador *escalonador,
    Processo *processo
);

// Libera as estruturas internas do escalonador
// Nao libera as arvores recebidas pelo main
void destruirEscalonador(Escalonador *escalonador);

// Executa a simulacao inteira
void executarEscalonador(
    char algoritmo[],
    int fatiaCPU,
    ArvoreRN *processosPorPid,
    ArvoreRN *processosFuturos,
    int quantidadeTotal,
    int modoDetalhado
);

#endif