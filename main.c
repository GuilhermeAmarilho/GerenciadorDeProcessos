#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "configuracao.h"
#include "entrada.h"
#include "processo.h"
#include "arvore_rubro_negra.h"
#include "escalonador.h"
#include "memoria.h"

int main(int argc, char *argv[]) {
    ConfiguracaoSistema configuracao;

    ArvoreRN processosPorPid;
    ArvoreRN processosFuturos;

    HistoricoAcessos historicoAcessos;
    ResultadoMemoria resultadoMemoria;

    int quantidadeProcessos;
    int modoDetalhado;

    int carregouArquivo;
    int iniciouHistorico;
    int executouEscalonador;
    int executouMemoria;

    // Por padrao mostra tudo que esta acontecendo
    modoDetalhado = 1;

    quantidadeProcessos = 0;

    // Faz a loteria mudar de resultado entre as execucoes
    srand(
        (unsigned int) time(NULL)
    );

    // Precisa informar pelo menos o arquivo de entrada
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

    // Essa arvore guarda todos os processos por PID
    // Ela tambem ajuda a perceber PID repetido
    inicializarArvore(
        &processosPorPid,
        ORDENAR_POR_PID
    );

    // Essa arvore guarda os processos que ainda vao chegar
    inicializarArvore(
        &processosFuturos,
        ORDENAR_POR_CRIACAO
    );

    // Confere se as duas arvores foram criadas
    if (
        processosPorPid.nulo == NULL ||
        processosFuturos.nulo == NULL
    ) {
        printf(
            "Erro: nao foi possivel criar as arvores.\n"
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

    // Le a configuracao e os processos do arquivo
    carregouArquivo = carregarArquivo(
        argv[1],
        &configuracao,
        &processosPorPid,
        &processosFuturos,
        &quantidadeProcessos
    );

    if (!carregouArquivo) {
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

    // So para garantir que tem alguma coisa para executar
    if (quantidadeProcessos <= 0) {
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

    // Prepara o vetor que vai guardar os acessos de memoria
    iniciouHistorico = inicializarHistoricoAcessos(
        &historicoAcessos
    );

    if (!iniciouHistorico) {
        printf(
            "Erro: nao foi possivel criar o historico de acessos.\n"
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

    if (modoDetalhado) {
        printf(
            "\n================ CONFIGURACAO ================\n"
        );

        printf(
            "Algoritmo de escalonamento: %s\n",
            configuracao.algoritmoEscalonamento
        );

        printf(
            "Fatia de CPU: %d\n",
            configuracao.fatiaCPU
        );

        if (
            configuracao.politicaMemoria ==
            POLITICA_LOCAL
        ) {
            printf(
                "Politica de memoria: LOCAL\n"
            );
        } else {
            printf(
                "Politica de memoria: GLOBAL\n"
            );
        }

        printf(
            "Tamanho da memoria: %d bytes\n",
            configuracao.tamanhoMemoria
        );

        printf(
            "Tamanho da pagina e moldura: %d bytes\n",
            configuracao.tamanhoPaginaMoldura
        );

        printf(
            "Quantidade total de molduras: %d\n",
            configuracao.tamanhoMemoria /
            configuracao.tamanhoPaginaMoldura
        );

        printf(
            "Percentual de alocacao: %d%%\n",
            configuracao.percentualAlocacao
        );

        printf(
            "Quantidade de processos: %d\n",
            quantidadeProcessos
        );
    }

    // Executa os processos e monta a ordem dos acessos
    executouEscalonador = executarEscalonador(
        &configuracao,
        &processosPorPid,
        &processosFuturos,
        quantidadeProcessos,
        modoDetalhado,
        &historicoAcessos
    );

    if (!executouEscalonador) {
        printf(
            "Erro: o escalonador nao terminou direito.\n"
        );

        destruirHistoricoAcessos(
            &historicoAcessos
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

    if (modoDetalhado) {
        printf(
            "\nQuantidade de acessos de memoria: %d\n",
            historicoAcessos.quantidade
        );

        printf(
            "\n================ ALGORITMOS DE MEMORIA ================\n"
        );
    }

    // Executa os quatro algoritmos com o mesmo historico
    executouMemoria = executarAlgoritmosMemoria(
        &historicoAcessos,
        &configuracao,
        &processosPorPid,
        &resultadoMemoria
    );

    if (!executouMemoria) {
        printf(
            "Erro: os algoritmos de memoria nao terminaram direito.\n"
        );

        destruirHistoricoAcessos(
            &historicoAcessos
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

    // Essa precisa ser a ultima linha da saida
    mostrarResultadoMemoria(
        &resultadoMemoria
    );

    // Libera o vetor com os acessos
    destruirHistoricoAcessos(
        &historicoAcessos
    );

    // A arvore de futuros nao e dona dos processos
    destruirArvore(
        &processosFuturos,
        0
    );

    // A arvore por PID e a dona dos processos
    destruirArvore(
        &processosPorPid,
        1
    );

    return 0;
}