#ifndef MEMORIA_H
#define MEMORIA_H

#include "configuracao.h"
#include "arvore_rubro_negra.h"

// Guarda um acesso feito por um processo
typedef struct {
    // Momento em que o acesso aconteceu
    int tempo;

    // Processo que fez o acesso
    int pid;

    // Pagina que foi acessada
    int pagina;
} AcessoMemoria;

// Guarda todos os acessos na ordem em que aconteceram
typedef struct {
    // Vetor com os acessos
    AcessoMemoria *acessos;

    // Quantidade de acessos guardados
    int quantidade;

    // Tamanho atual do vetor
    int capacidade;
} HistoricoAcessos;

// Diz qual algoritmo ficou mais perto do otimo
typedef enum {
    MELHOR_ALGORITMO_INVALIDO,
    MELHOR_ALGORITMO_FIFO,
    MELHOR_ALGORITMO_LRU,
    MELHOR_ALGORITMO_NUF,
    MELHOR_ALGORITMO_EMPATE
} MelhorAlgoritmoMemoria;

// Guarda o resultado final dos algoritmos
typedef struct {
    // Quantidade de trocas feitas pelo FIFO
    int trocasFIFO;

    // Quantidade de trocas feitas pelo LRU
    int trocasLRU;

    // Quantidade de trocas feitas pelo NUF
    int trocasNUF;

    // Quantidade de trocas feitas pelo algoritmo otimo
    int trocasOtimo;

    // Algoritmo que ficou mais perto do otimo
    MelhorAlgoritmoMemoria melhorAlgoritmo;
} ResultadoMemoria;

// Prepara o historico para comecar vazio
// Retorna 1 se deu certo
// Retorna 0 se deu erro
int inicializarHistoricoAcessos(
    HistoricoAcessos *historico
);

// Libera o vetor usado pelo historico
void destruirHistoricoAcessos(
    HistoricoAcessos *historico
);

// Guarda um novo acesso no final do historico
// Retorna 1 se conseguiu guardar
// Retorna 0 se deu erro
int registrarAcessoMemoria(
    HistoricoAcessos *historico,
    int tempo,
    int pid,
    int pagina
);

// Executa FIFO, LRU, NUF e otimo usando o mesmo historico
// Retorna 1 se todos terminaram direito
// Retorna 0 se deu algum erro
int executarAlgoritmosMemoria(
    const HistoricoAcessos *historico,
    const ConfiguracaoSistema *configuracao,
    ArvoreRN *processosPorPid,
    ResultadoMemoria *resultado
);

// Mostra a ultima linha pedida pelo trabalho
void mostrarResultadoMemoria(
    const ResultadoMemoria *resultado
);

#endif