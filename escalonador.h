#ifndef ESCALONADOR_H
#define ESCALONADOR_H

#include "processo.h"
#include "configuracao.h"
#include "memoria.h"
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

// Junta tudo que o escalonador usa durante a simulacao
typedef struct {
    // Arvore com todos os processos
    // Ela fica organizada por PID
    ArvoreRN *processosPorPid;

    // Processos que ainda nao chegaram no sistema
    ArvoreRN *processosFuturos;

    // Processos que ja podem usar a CPU
    ArvoreRN processosProntos;

    // Fila usada somente no Round Robin
    FilaRR filaRoundRobin;

    // Processo que esta usando a CPU agora
    Processo *processoExecutando;

    // Configuracao que veio da primeira linha do arquivo
    const ConfiguracaoSistema *configuracao;

    // Guarda todos os acessos de memoria na ordem
    // em que aconteceram durante o escalonamento
    HistoricoAcessos *historicoAcessos;

    // Relogio atual da simulacao
    int tempoAtual;

    // Quantidade total de processos
    int quantidadeTotal;

    // Quantidade de processos que ja terminaram
    int quantidadeFinalizados;

    // Mostra ou esconde os detalhes da execucao
    int modoDetalhado;
} Escalonador;

// Prepara uma fila vazia
void inicializarFilaRoundRobin(
    FilaRR *fila
);

// Verifica se a fila esta vazia
int filaRoundRobinEstaVazia(
    FilaRR *fila
);

// Coloca um processo no final da fila
int colocarNaFilaRoundRobin(
    FilaRR *fila,
    Processo *processo
);

// Retira o primeiro processo da fila
Processo *retirarDaFilaRoundRobin(
    FilaRR *fila
);

// Libera os nos usados pela fila
// Os processos nao sao liberados aqui
void destruirFilaRoundRobin(
    FilaRR *fila
);

// Escolhe a ordem da arvore de processos prontos
TipoOrdenacaoArvore escolherOrdenacaoDosProntos(
    const char algoritmo[]
);

// Prepara tudo que o escalonador vai usar
int inicializarEscalonador(
    Escalonador *escalonador,
    const ConfiguracaoSistema *configuracao,
    ArvoreRN *processosPorPid,
    ArvoreRN *processosFuturos,
    int quantidadeTotal,
    int modoDetalhado,
    HistoricoAcessos *historicoAcessos
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
// Tambem registra um acesso de memoria em cada ciclo
// Retorna 1 se deu certo
// Retorna 0 se aconteceu algum erro
int executarFatiaDeCPU(
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
// Tambem nao libera o historico de acessos
void destruirEscalonador(
    Escalonador *escalonador
);

// Executa a simulacao inteira
// Retorna 1 se terminou normalmente
// Retorna 0 se aconteceu algum erro
int executarEscalonador(
    const ConfiguracaoSistema *configuracao,
    ArvoreRN *processosPorPid,
    ArvoreRN *processosFuturos,
    int quantidadeTotal,
    int modoDetalhado,
    HistoricoAcessos *historicoAcessos
);

#endif