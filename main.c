#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "processo.h"
#include "arvore_rubro_negra.h"
#include "escalonador.h"

int main(int argc, char *argv[]) {
    ArvoreRN processosPorPid;
    ArvoreRN processosFuturos;

    char algoritmo[50];

    int fatiaCPU;
    int quantidade;
    int modoDetalhado;
    int carregouArquivo;

    // Por padrao mostra tudo que esta acontecendo
    modoDetalhado = 1;

    // Faz a loteria gerar resultados diferentes
    srand(
        (unsigned int) time(NULL)
    );

    // Precisa passar pelo menos o arquivo de entrada
    if (argc < 2) {
        printf("Uso correto:\n");
        printf("./escalonador entrada.txt\n");
        printf("./escalonador entrada.txt silencioso\n");

        return 1;
    }

    // Confere se foi pedido o modo silencioso
    if (argc >= 3) {
        if (
            strcmp(
                argv[2],
                "silencioso"
            ) == 0
        ) {
            modoDetalhado = 0;
        } else {
            printf(
                "Aviso: opcao desconhecida: %s\n",
                argv[2]
            );

            printf(
                "O programa vai continuar no modo detalhado.\n"
            );
        }
    }

    // Essa arvore guarda todos os processos
    // Ela tambem serve para procurar PID repetido
    inicializarArvore(
        &processosPorPid,
        ORDENAR_POR_PID
    );

    // Essa arvore guarda os processos que ainda vao chegar
    inicializarArvore(
        &processosFuturos,
        ORDENAR_POR_CRIACAO
    );

    // Confere se as arvores conseguiram criar o no nulo
    if (
        processosPorPid.nulo == NULL ||
        processosFuturos.nulo == NULL
    ) {
        printf(
            "Erro: nao foi possivel criar as arvores.\n"
        );

        // Pode chamar mesmo se uma delas deu erro
        destruirArvore(
            &processosFuturos,
            0
        );

        destruirArvore(
            &processosPorPid,
            1
        );

        return 1;
    }

    // Le o arquivo e coloca os processos nas arvores
    carregouArquivo = carregarArquivo(
        argv[1],
        algoritmo,
        &fatiaCPU,
        &processosPorPid,
        &processosFuturos,
        &quantidade
    );

    if (!carregouArquivo) {
        // Em alguns erros o carregarArquivo ja limpou as arvores
        // A funcao destruirArvore aceita isso sem problema
        destruirArvore(
            &processosFuturos,
            0
        );

        destruirArvore(
            &processosPorPid,
            1
        );

        return 1;
    }

    // Nao tem nada para executar
    if (quantidade == 0) {
        printf(
            "Erro: nenhum processo foi carregado.\n"
        );

        destruirArvore(
            &processosFuturos,
            0
        );

        destruirArvore(
            &processosPorPid,
            1
        );

        return 1;
    }

    // Comeca a simulacao
    executarEscalonador(
        algoritmo,
        fatiaCPU,
        &processosPorPid,
        &processosFuturos,
        quantidade,
        modoDetalhado
    );

    // A arvore de futuros nao libera os processos
    // Eles tambem estao guardados na arvore por PID
    destruirArvore(
        &processosFuturos,
        0
    );

    // Essa e a arvore dona dos processos
    // Por isso ela libera os nos e os processos
    destruirArvore(
        &processosPorPid,
        1
    );

    return 0;
}