#ifndef CONFIGURACAO_H
#define CONFIGURACAO_H

// Tamanho usado para guardar o nome do algoritmo
#define TAM_NOME_ALGORITMO 50

// Os dois jeitos que a memoria pode trabalhar
// O invalido ajuda a perceber quando deu erro na leitura
// da politica no arquivo
typedef enum {
    POLITICA_INVALIDA,
    POLITICA_LOCAL,
    POLITICA_GLOBAL
} PoliticaMemoria;

// Guarda tudo que vem na primeira linha do arquivo
typedef struct {
    // Algoritmo usado para escolher os processos
    char algoritmoEscalonamento[TAM_NOME_ALGORITMO];

    // Tempo que um processo pode ficar na CPU por vez
    int fatiaCPU;

    // Diz se a troca de pagina vai ser local ou global
    PoliticaMemoria politicaMemoria;

    // Tamanho total da memoria principal em bytes
    int tamanhoMemoria;

    // Tamanho de cada pagina e de cada moldura em bytes
    int tamanhoPaginaMoldura;

    // Porcentagem maxima da memoria virtual do processo
    // que pode ficar na memoria principal
    int percentualAlocacao;
} ConfiguracaoSistema;

#endif