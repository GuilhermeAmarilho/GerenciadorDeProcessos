#ifndef ARVORE_RUBRO_NEGRA_H
#define ARVORE_RUBRO_NEGRA_H

#include "processo.h"

// Cor que cada no pode ter, ignorar o trocadilho rpzd kkkkkkkk 
typedef enum {
    VERMELHO,
    PRETO
} CorNo;

// Diz como os processos vao ficar organizados na arvore
typedef enum {
    ORDENAR_POR_PID,
    ORDENAR_POR_CRIACAO,
    ORDENAR_POR_PRIORIDADE,
    ORDENAR_POR_VRUNTIME
} TipoOrdenacaoArvore;

// Representa um no da arvore
typedef struct NoRN {
    // Processo guardado nesse no
    Processo *processo;

    // Cor do no
    CorNo cor;

    // Ponteiros usados pela arvore
    struct NoRN *pai;
    struct NoRN *esquerda;
    struct NoRN *direita;

    // Guarda a soma dos bilhetes desse no e dos filhos
    // Isso vai ajudar no algoritmo de loteria
    long long somaBilhetesSubarvore;
} NoRN;

// Representa a arvore inteira
typedef struct ArvoreRN {
    // Primeiro no da arvore
    NoRN *raiz;

    // No preto usado no lugar de NULL
    NoRN *nulo;

    // Quantidade de processos dentro da arvore
    int quantidade;

    // Forma usada para organizar os processos
    TipoOrdenacaoArvore tipoOrdenacao;
} ArvoreRN;

// Prepara a arvore para ser usada
void inicializarArvore(
    ArvoreRN *arvore,
    TipoOrdenacaoArvore tipoOrdenacao
);

// Verifica se a arvore esta vazia
int arvoreEstaVazia(ArvoreRN *arvore);

// Insere um processo na arvore
int inserirProcessoNaArvore(
    ArvoreRN *arvore,
    Processo *processo
);

// Procura o no de um processo usando a ordenacao da arvore
NoRN *buscarNoNaArvore(
    ArvoreRN *arvore,
    Processo *processo
);

// Procura um processo pelo PID
// Essa busca deve ser usada na arvore ordenada por PID
Processo *buscarProcessoPorPid(
    ArvoreRN *arvore,
    int pid
);

// Retorna o menor no da arvore
NoRN *obterMenorNo(
    ArvoreRN *arvore
);

// Retorna o processo que esta no menor no
Processo *obterMenorProcesso(
    ArvoreRN *arvore
);

// Retorna o proximo no da arvore
// Vai ser usado para percorrer os processos em ordem
NoRN *obterProximoNo(
    ArvoreRN *arvore,
    NoRN *noAtual
);

// Remove um no que ja foi encontrado
int removerNoDaArvore(
    ArvoreRN *arvore,
    NoRN *no
);

// Procura e remove um processo da arvore
int removerProcessoDaArvore(
    ArvoreRN *arvore,
    Processo *processo
);

// Retorna a soma de todos os bilhetes da arvore
long long obterTotalBilhetes(
    ArvoreRN *arvore
);

// Procura o processo dono de um bilhete sorteado
Processo *buscarProcessoPorBilhete(
    ArvoreRN *arvore,
    long long bilheteSorteado
);

// Libera os nos usados pela arvore
// Se liberarProcessos for 1, tambem libera os processos
// Se for 0, libera somente os nos da arvore
void destruirArvore(
    ArvoreRN *arvore,
    int liberarProcessos
);

#endif
