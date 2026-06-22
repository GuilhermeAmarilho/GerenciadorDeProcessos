#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "memoria.h"

// Quantidade inicial de espaco no historico
#define CAPACIDADE_INICIAL_HISTORICO 32

// Nomes usados so dentro deste arquivo
typedef enum {
    ALGORITMO_INTERNO_FIFO,
    ALGORITMO_INTERNO_LRU,
    ALGORITMO_INTERNO_NUF,
    ALGORITMO_INTERNO_OTIMO
} AlgoritmoMemoriaInterno;

// Guarda o que tem em uma moldura
typedef struct {
    // Diz se a moldura esta sendo usada
    int ocupada;

    // Processo dono da pagina
    int pid;

    // Pagina que esta na moldura
    int pagina;

    // Momento em que a pagina entrou
    int momentoEntrada;

    // Ultimo momento em que ela foi usada
    int ultimoAcesso;

    // Quantas vezes ela foi usada desde que entrou
    int quantidadeAcessos;
} Moldura;

// Guarda umas informacoes de cada processo durante a simulacao
typedef struct {
    // PID do processo
    int pid;

    // Maximo de molduras que ele pode ocupar
    int limiteMolduras;

    // Quantos acessos dele ainda faltam
    int acessosRestantes;
} ControleProcessoMemoria;

// Deixa uma moldura vazia
static void limparMoldura(Moldura *moldura) {
    if (moldura == NULL) {
        return;
    }

    moldura->ocupada = 0;
    moldura->pid = -1;
    moldura->pagina = -1;
    moldura->momentoEntrada = -1;
    moldura->ultimoAcesso = -1;
    moldura->quantidadeAcessos = 0;
}

// Prepara todas as molduras vazias
static void inicializarMolduras(
    Moldura molduras[],
    int quantidadeMolduras
) {
    int i;

    i = 0;

    while (i < quantidadeMolduras) {
        limparMoldura(&molduras[i]);
        i++;
    }
}

// Calcula quantas paginas o processo tem
static int calcularQuantidadePaginas(
    Processo *processo,
    const ConfiguracaoSistema *configuracao
) {
    int quantidadePaginas;

    if (
        processo == NULL ||
        configuracao == NULL ||
        configuracao->tamanhoPaginaMoldura <= 0
    ) {
        return 0;
    }

    quantidadePaginas =
        processo->quantidadeMemoria /
        configuracao->tamanhoPaginaMoldura;

    if (
        processo->quantidadeMemoria %
        configuracao->tamanhoPaginaMoldura != 0
    ) {
        quantidadePaginas++;
    }

    return quantidadePaginas;
}

// Calcula o maximo de molduras que um processo pode usar
static int calcularLimiteMolduras(
    Processo *processo,
    const ConfiguracaoSistema *configuracao,
    int quantidadeTotalMolduras
) {
    int quantidadePaginas;
    int limite;

    quantidadePaginas = calcularQuantidadePaginas(
        processo,
        configuracao
    );

    if (quantidadePaginas <= 0) {
        return 0;
    }

    limite =
        quantidadePaginas *
        configuracao->percentualAlocacao /
        100;

    // Um processo precisa conseguir ter pelo menos uma pagina
    if (limite <= 0) {
        limite = 1;
    }

    if (limite > quantidadePaginas) {
        limite = quantidadePaginas;
    }

    if (limite > quantidadeTotalMolduras) {
        limite = quantidadeTotalMolduras;
    }

    return limite;
}

// Cria o controle dos processos na ordem dos PIDs
static ControleProcessoMemoria *criarControlesDosProcessos(
    ArvoreRN *processosPorPid,
    const ConfiguracaoSistema *configuracao,
    int quantidadeTotalMolduras,
    int *quantidadeControles
) {
    ControleProcessoMemoria *controles;
    NoRN *noAtual;
    Processo *processo;
    int posicao;

    if (quantidadeControles != NULL) {
        *quantidadeControles = 0;
    }

    if (
        processosPorPid == NULL ||
        configuracao == NULL ||
        quantidadeControles == NULL
    ) {
        return NULL;
    }

    if (processosPorPid->quantidade <= 0) {
        return NULL;
    }

    controles = malloc(
        sizeof(ControleProcessoMemoria) *
        processosPorPid->quantidade
    );

    if (controles == NULL) {
        return NULL;
    }

    posicao = 0;
    noAtual = obterMenorNo(processosPorPid);

    while (noAtual != NULL) {
        processo = noAtual->processo;

        controles[posicao].pid = processo->pid;

        controles[posicao].limiteMolduras = calcularLimiteMolduras(
            processo,
            configuracao,
            quantidadeTotalMolduras
        );

        controles[posicao].acessosRestantes =
            processo->quantidadeAcessos;

        if (controles[posicao].limiteMolduras <= 0) {
            free(controles);
            return NULL;
        }

        posicao++;

        noAtual = obterProximoNo(
            processosPorPid,
            noAtual
        );
    }

    *quantidadeControles = posicao;

    return controles;
}

// Procura o controle pelo PID
// Como a arvore estava por PID, o vetor tambem ficou por PID
static ControleProcessoMemoria *buscarControlePorPid(
    ControleProcessoMemoria controles[],
    int quantidadeControles,
    int pid
) {
    int inicio;
    int fim;
    int meio;

    if (controles == NULL) {
        return NULL;
    }

    inicio = 0;
    fim = quantidadeControles - 1;

    while (inicio <= fim) {
        meio = inicio + (fim - inicio) / 2;

        if (controles[meio].pid == pid) {
            return &controles[meio];
        }

        if (pid < controles[meio].pid) {
            fim = meio - 1;
        } else {
            inicio = meio + 1;
        }
    }

    return NULL;
}

// Procura uma pagina que ja esta na memoria
static int procurarPaginaNaMemoria(
    Moldura molduras[],
    int quantidadeMolduras,
    int pid,
    int pagina
) {
    int i;

    i = 0;

    while (i < quantidadeMolduras) {
        if (
            molduras[i].ocupada &&
            molduras[i].pid == pid &&
            molduras[i].pagina == pagina
        ) {
            return i;
        }

        i++;
    }

    return -1;
}

// Procura uma moldura vazia
static int procurarMolduraVazia(
    Moldura molduras[],
    int quantidadeMolduras
) {
    int i;

    i = 0;

    while (i < quantidadeMolduras) {
        if (!molduras[i].ocupada) {
            return i;
        }

        i++;
    }

    return -1;
}

// Conta quantas molduras um processo esta usando
static int contarMoldurasDoProcesso(
    Moldura molduras[],
    int quantidadeMolduras,
    int pid
) {
    int quantidade;
    int i;

    quantidade = 0;
    i = 0;

    while (i < quantidadeMolduras) {
        if (
            molduras[i].ocupada &&
            molduras[i].pid == pid
        ) {
            quantidade++;
        }

        i++;
    }

    return quantidade;
}

// Coloca uma pagina dentro de uma moldura
static void colocarPaginaNaMoldura(
    Moldura *moldura,
    int pid,
    int pagina,
    int momento
) {
    if (moldura == NULL) {
        return;
    }

    moldura->ocupada = 1;
    moldura->pid = pid;
    moldura->pagina = pagina;
    moldura->momentoEntrada = momento;
    moldura->ultimoAcesso = momento;
    moldura->quantidadeAcessos = 1;
}

// Diz se uma moldura pode entrar na escolha da vitima
static int molduraPodeSerVitima(
    const Moldura *moldura,
    PoliticaMemoria politica,
    int pidDoAcesso,
    int processoChegouNoLimite
) {
    if (moldura == NULL) {
        return 0;
    }

    if (!moldura->ocupada) {
        return 0;
    }

    // Na politica local so pode tirar pagina do mesmo processo
    if (politica == POLITICA_LOCAL) {
        if (moldura->pid != pidDoAcesso) {
            return 0;
        }
    }

    // Mesmo no global, se o processo ja chegou no limite
    // ele precisa tirar uma pagina dele para nao passar do maximo
    if (
        politica == POLITICA_GLOBAL &&
        processoChegouNoLimite
    ) {
        if (moldura->pid != pidDoAcesso) {
            return 0;
        }
    }

    return 1;
}

// Procura quando uma pagina vai aparecer de novo
// Quanto maior o valor, mais longe esta o proximo uso
static int procurarProximoUso(
    const HistoricoAcessos *historico,
    int posicaoAtual,
    int pid,
    int pagina
) {
    int i;

    i = posicaoAtual + 1;

    while (i < historico->quantidade) {
        if (
            historico->acessos[i].pid == pid &&
            historico->acessos[i].pagina == pagina
        ) {
            return i;
        }

        i++;
    }

    // Se nunca mais vai ser usada, ela e a melhor para sair
    return INT_MAX;
}

// Escolhe uma vitima pelo FIFO
static int escolherVitimaFIFO(
    Moldura molduras[],
    int quantidadeMolduras,
    PoliticaMemoria politica,
    int pidDoAcesso,
    int processoChegouNoLimite
) {
    int vitima;
    int i;

    vitima = -1;
    i = 0;

    while (i < quantidadeMolduras) {
        if (
            molduraPodeSerVitima(
                &molduras[i],
                politica,
                pidDoAcesso,
                processoChegouNoLimite
            )
        ) {
            if (vitima == -1) {
                vitima = i;
            } else if (
                molduras[i].momentoEntrada <
                molduras[vitima].momentoEntrada
            ) {
                vitima = i;
            }
        }

        i++;
    }

    return vitima;
}

// Escolhe uma vitima pelo LRU
static int escolherVitimaLRU(
    Moldura molduras[],
    int quantidadeMolduras,
    PoliticaMemoria politica,
    int pidDoAcesso,
    int processoChegouNoLimite
) {
    int vitima;
    int i;

    vitima = -1;
    i = 0;

    while (i < quantidadeMolduras) {
        if (
            molduraPodeSerVitima(
                &molduras[i],
                politica,
                pidDoAcesso,
                processoChegouNoLimite
            )
        ) {
            if (vitima == -1) {
                vitima = i;
            } else if (
                molduras[i].ultimoAcesso <
                molduras[vitima].ultimoAcesso
            ) {
                vitima = i;
            }
        }

        i++;
    }

    return vitima;
}

// Escolhe uma vitima pelo NUF
static int escolherVitimaNUF(
    Moldura molduras[],
    int quantidadeMolduras,
    PoliticaMemoria politica,
    int pidDoAcesso,
    int processoChegouNoLimite
) {
    int vitima;
    int i;

    vitima = -1;
    i = 0;

    while (i < quantidadeMolduras) {
        if (
            molduraPodeSerVitima(
                &molduras[i],
                politica,
                pidDoAcesso,
                processoChegouNoLimite
            )
        ) {
            if (vitima == -1) {
                vitima = i;
            } else if (
                molduras[i].quantidadeAcessos <
                molduras[vitima].quantidadeAcessos
            ) {
                vitima = i;
            } else if (
                molduras[i].quantidadeAcessos ==
                molduras[vitima].quantidadeAcessos
            ) {
                // O trabalho manda tirar a pagina de menor ID
                if (
                    molduras[i].pagina <
                    molduras[vitima].pagina
                ) {
                    vitima = i;
                } else if (
                    molduras[i].pagina ==
                    molduras[vitima].pagina &&
                    molduras[i].pid <
                    molduras[vitima].pid
                ) {
                    // Isso so ajuda quando dois processos tem a mesma pagina
                    vitima = i;
                }
            }
        }

        i++;
    }

    return vitima;
}

// Escolhe uma vitima pelo algoritmo otimo
static int escolherVitimaOtimo(
    Moldura molduras[],
    int quantidadeMolduras,
    PoliticaMemoria politica,
    int pidDoAcesso,
    int processoChegouNoLimite,
    const HistoricoAcessos *historico,
    int posicaoAtual
) {
    int vitima;
    int proximoUsoDaVitima;
    int proximoUsoAtual;
    int i;

    vitima = -1;
    proximoUsoDaVitima = -1;
    i = 0;

    while (i < quantidadeMolduras) {
        if (
            molduraPodeSerVitima(
                &molduras[i],
                politica,
                pidDoAcesso,
                processoChegouNoLimite
            )
        ) {
            proximoUsoAtual = procurarProximoUso(
                historico,
                posicaoAtual,
                molduras[i].pid,
                molduras[i].pagina
            );

            if (vitima == -1) {
                vitima = i;
                proximoUsoDaVitima = proximoUsoAtual;
            } else if (
                proximoUsoAtual > proximoUsoDaVitima
            ) {
                vitima = i;
                proximoUsoDaVitima = proximoUsoAtual;
            } else if (
                proximoUsoAtual == proximoUsoDaVitima
            ) {
                // O desempate aqui nao muda o numero de trocas
                // mas deixa o resultado sempre igual
                if (
                    molduras[i].pagina <
                    molduras[vitima].pagina
                ) {
                    vitima = i;
                    proximoUsoDaVitima = proximoUsoAtual;
                } else if (
                    molduras[i].pagina ==
                    molduras[vitima].pagina &&
                    molduras[i].pid <
                    molduras[vitima].pid
                ) {
                    vitima = i;
                    proximoUsoDaVitima = proximoUsoAtual;
                }
            }
        }

        i++;
    }

    return vitima;
}

// Escolhe a vitima usando o algoritmo pedido
static int escolherVitima(
    AlgoritmoMemoriaInterno algoritmo,
    Moldura molduras[],
    int quantidadeMolduras,
    PoliticaMemoria politica,
    int pidDoAcesso,
    int processoChegouNoLimite,
    const HistoricoAcessos *historico,
    int posicaoAtual
) {
    if (algoritmo == ALGORITMO_INTERNO_FIFO) {
        return escolherVitimaFIFO(
            molduras,
            quantidadeMolduras,
            politica,
            pidDoAcesso,
            processoChegouNoLimite
        );
    }

    if (algoritmo == ALGORITMO_INTERNO_LRU) {
        return escolherVitimaLRU(
            molduras,
            quantidadeMolduras,
            politica,
            pidDoAcesso,
            processoChegouNoLimite
        );
    }

    if (algoritmo == ALGORITMO_INTERNO_NUF) {
        return escolherVitimaNUF(
            molduras,
            quantidadeMolduras,
            politica,
            pidDoAcesso,
            processoChegouNoLimite
        );
    }

    return escolherVitimaOtimo(
        molduras,
        quantidadeMolduras,
        politica,
        pidDoAcesso,
        processoChegouNoLimite,
        historico,
        posicaoAtual
    );
}

// Libera todas as paginas de um processo quando ele termina
static void liberarMoldurasDoProcesso(
    Moldura molduras[],
    int quantidadeMolduras,
    int pid
) {
    int i;

    i = 0;

    while (i < quantidadeMolduras) {
        if (
            molduras[i].ocupada &&
            molduras[i].pid == pid
        ) {
            limparMoldura(&molduras[i]);
        }

        i++;
    }
}

// Confere se todos os acessos dos processos foram usados
static int todosOsAcessosForamFeitos(
    ControleProcessoMemoria controles[],
    int quantidadeControles
) {
    int i;

    i = 0;

    while (i < quantidadeControles) {
        if (controles[i].acessosRestantes != 0) {
            return 0;
        }

        i++;
    }

    return 1;
}

// Faz a simulacao de um dos quatro algoritmos
static int executarUmAlgoritmo(
    AlgoritmoMemoriaInterno algoritmo,
    const HistoricoAcessos *historico,
    const ConfiguracaoSistema *configuracao,
    ArvoreRN *processosPorPid,
    int *quantidadeTrocas
) {
    Moldura *molduras;
    ControleProcessoMemoria *controles;
    ControleProcessoMemoria *controleAtual;
    const AcessoMemoria *acesso;

    int quantidadeMolduras;
    int quantidadeControles;
    int posicaoPagina;
    int posicaoVazia;
    int posicaoVitima;
    int moldurasDoProcesso;
    int processoChegouNoLimite;
    int i;
    int trocas;

    if (quantidadeTrocas != NULL) {
        *quantidadeTrocas = 0;
    }

    if (
        historico == NULL ||
        configuracao == NULL ||
        processosPorPid == NULL ||
        quantidadeTrocas == NULL
    ) {
        return 0;
    }

    quantidadeMolduras =
        configuracao->tamanhoMemoria /
        configuracao->tamanhoPaginaMoldura;

    if (quantidadeMolduras <= 0) {
        printf("Erro: a memoria nao tem nenhuma moldura.\n");
        return 0;
    }

    molduras = malloc(
        sizeof(Moldura) * quantidadeMolduras
    );

    if (molduras == NULL) {
        printf("Erro: faltou memoria para criar as molduras.\n");
        return 0;
    }

    inicializarMolduras(
        molduras,
        quantidadeMolduras
    );

    controles = criarControlesDosProcessos(
        processosPorPid,
        configuracao,
        quantidadeMolduras,
        &quantidadeControles
    );

    if (controles == NULL) {
        printf("Erro: nao foi possivel preparar os processos para a memoria.\n");
        free(molduras);
        return 0;
    }

    trocas = 0;
    i = 0;

    while (i < historico->quantidade) {
        acesso = &historico->acessos[i];

        controleAtual = buscarControlePorPid(
            controles,
            quantidadeControles,
            acesso->pid
        );

        if (controleAtual == NULL) {
            printf(
                "Erro: apareceu acesso de um PID que nao existe: %d\n",
                acesso->pid
            );

            free(controles);
            free(molduras);
            return 0;
        }

        if (controleAtual->acessosRestantes <= 0) {
            printf(
                "Erro: o PID %d fez acessos demais.\n",
                acesso->pid
            );

            free(controles);
            free(molduras);
            return 0;
        }

        posicaoPagina = procurarPaginaNaMemoria(
            molduras,
            quantidadeMolduras,
            acesso->pid,
            acesso->pagina
        );

        // Se a pagina ja esta na memoria, so atualiza os dados dela
        if (posicaoPagina >= 0) {
            molduras[posicaoPagina].ultimoAcesso = i;
            molduras[posicaoPagina].quantidadeAcessos++;
        } else {
            moldurasDoProcesso = contarMoldurasDoProcesso(
                molduras,
                quantidadeMolduras,
                acesso->pid
            );

            processoChegouNoLimite = 0;

            if (
                moldurasDoProcesso >=
                controleAtual->limiteMolduras
            ) {
                processoChegouNoLimite = 1;
            }

            posicaoVazia = procurarMolduraVazia(
                molduras,
                quantidadeMolduras
            );

            // Usa uma moldura vazia se o processo ainda pode crescer
            if (
                posicaoVazia >= 0 &&
                !processoChegouNoLimite
            ) {
                colocarPaginaNaMoldura(
                    &molduras[posicaoVazia],
                    acesso->pid,
                    acesso->pagina,
                    i
                );
            } else {
                posicaoVitima = escolherVitima(
                    algoritmo,
                    molduras,
                    quantidadeMolduras,
                    configuracao->politicaMemoria,
                    acesso->pid,
                    processoChegouNoLimite,
                    historico,
                    i
                );

                if (posicaoVitima < 0) {
                    printf(
                        "Erro: nao foi encontrada uma pagina para substituir no acesso %d.\n",
                        i
                    );

                    free(controles);
                    free(molduras);
                    return 0;
                }

                // Aqui uma pagina ocupada sai e outra entra
                // Entao isso conta como uma troca
                colocarPaginaNaMoldura(
                    &molduras[posicaoVitima],
                    acesso->pid,
                    acesso->pagina,
                    i
                );

                trocas++;
            }
        }

        controleAtual->acessosRestantes--;

        // Quando o processo acaba, as paginas dele ficam livres
        // Isso nao conta como troca
        if (controleAtual->acessosRestantes == 0) {
            liberarMoldurasDoProcesso(
                molduras,
                quantidadeMolduras,
                acesso->pid
            );
        }

        i++;
    }

    if (
        !todosOsAcessosForamFeitos(
            controles,
            quantidadeControles
        )
    ) {
        printf("Erro: o historico terminou antes dos acessos dos processos.\n");

        free(controles);
        free(molduras);
        return 0;
    }

    *quantidadeTrocas = trocas;

    free(controles);
    free(molduras);

    return 1;
}

// Descobre quem ficou mais perto do otimo
static MelhorAlgoritmoMemoria descobrirMelhorAlgoritmo(
    const ResultadoMemoria *resultado
) {
    int diferencaFIFO;
    int diferencaLRU;
    int diferencaNUF;
    int menorDiferenca;
    int quantidadeMelhores;
    MelhorAlgoritmoMemoria melhor;

    if (resultado == NULL) {
        return MELHOR_ALGORITMO_INVALIDO;
    }

    diferencaFIFO =
        resultado->trocasFIFO -
        resultado->trocasOtimo;

    diferencaLRU =
        resultado->trocasLRU -
        resultado->trocasOtimo;

    diferencaNUF =
        resultado->trocasNUF -
        resultado->trocasOtimo;

    // O otimo normalmente vai ser menor, mas isso evita numero negativo
    if (diferencaFIFO < 0) {
        diferencaFIFO = -diferencaFIFO;
    }

    if (diferencaLRU < 0) {
        diferencaLRU = -diferencaLRU;
    }

    if (diferencaNUF < 0) {
        diferencaNUF = -diferencaNUF;
    }

    menorDiferenca = diferencaFIFO;
    melhor = MELHOR_ALGORITMO_FIFO;

    if (diferencaLRU < menorDiferenca) {
        menorDiferenca = diferencaLRU;
        melhor = MELHOR_ALGORITMO_LRU;
    }

    if (diferencaNUF < menorDiferenca) {
        menorDiferenca = diferencaNUF;
        melhor = MELHOR_ALGORITMO_NUF;
    }

    quantidadeMelhores = 0;

    if (diferencaFIFO == menorDiferenca) {
        quantidadeMelhores++;
    }

    if (diferencaLRU == menorDiferenca) {
        quantidadeMelhores++;
    }

    if (diferencaNUF == menorDiferenca) {
        quantidadeMelhores++;
    }

    if (quantidadeMelhores > 1) {
        return MELHOR_ALGORITMO_EMPATE;
    }

    return melhor;
}

// Prepara o historico para comecar vazio
int inicializarHistoricoAcessos(
    HistoricoAcessos *historico
) {
    if (historico == NULL) {
        return 0;
    }

    historico->acessos = malloc(
        sizeof(AcessoMemoria) *
        CAPACIDADE_INICIAL_HISTORICO
    );

    if (historico->acessos == NULL) {
        historico->quantidade = 0;
        historico->capacidade = 0;
        return 0;
    }

    historico->quantidade = 0;
    historico->capacidade =
        CAPACIDADE_INICIAL_HISTORICO;

    return 1;
}

// Libera o vetor usado pelo historico
void destruirHistoricoAcessos(
    HistoricoAcessos *historico
) {
    if (historico == NULL) {
        return;
    }

    if (historico->acessos != NULL) {
        free(historico->acessos);
    }

    historico->acessos = NULL;
    historico->quantidade = 0;
    historico->capacidade = 0;
}

// Guarda um acesso no final do historico
int registrarAcessoMemoria(
    HistoricoAcessos *historico,
    int tempo,
    int pid,
    int pagina
) {
    AcessoMemoria *novoVetor;
    int novaCapacidade;

    if (historico == NULL) {
        return 0;
    }

    if (
        historico->acessos == NULL ||
        historico->capacidade <= 0
    ) {
        return 0;
    }

    if (
        tempo < 0 ||
        pid < 0 ||
        pagina <= 0
    ) {
        return 0;
    }

    // Se o vetor encheu, dobra o tamanho dele
    if (historico->quantidade >= historico->capacidade) {
        novaCapacidade =
            historico->capacidade * 2;

        novoVetor = realloc(
            historico->acessos,
            sizeof(AcessoMemoria) * novaCapacidade
        );

        if (novoVetor == NULL) {
            return 0;
        }

        historico->acessos = novoVetor;
        historico->capacidade = novaCapacidade;
    }

    historico->acessos[historico->quantidade].tempo =
        tempo;

    historico->acessos[historico->quantidade].pid =
        pid;

    historico->acessos[historico->quantidade].pagina =
        pagina;

    historico->quantidade++;

    return 1;
}

// Executa os quatro algoritmos usando o mesmo historico
int executarAlgoritmosMemoria(
    const HistoricoAcessos *historico,
    const ConfiguracaoSistema *configuracao,
    ArvoreRN *processosPorPid,
    ResultadoMemoria *resultado
) {
    if (resultado != NULL) {
        resultado->trocasFIFO = 0;
        resultado->trocasLRU = 0;
        resultado->trocasNUF = 0;
        resultado->trocasOtimo = 0;
        resultado->melhorAlgoritmo =
            MELHOR_ALGORITMO_INVALIDO;
    }

    if (
        historico == NULL ||
        configuracao == NULL ||
        processosPorPid == NULL ||
        resultado == NULL
    ) {
        return 0;
    }

    if (
        historico->acessos == NULL ||
        historico->quantidade <= 0
    ) {
        printf("Erro: o historico de acessos esta vazio.\n");
        return 0;
    }

    if (
        configuracao->politicaMemoria != POLITICA_LOCAL &&
        configuracao->politicaMemoria != POLITICA_GLOBAL
    ) {
        printf("Erro: politica de memoria invalida.\n");
        return 0;
    }

    if (
        configuracao->tamanhoMemoria <= 0 ||
        configuracao->tamanhoPaginaMoldura <= 0 ||
        configuracao->percentualAlocacao <= 0 ||
        configuracao->percentualAlocacao > 100
    ) {
        printf("Erro: configuracao da memoria invalida.\n");
        return 0;
    }

    if (
        !executarUmAlgoritmo(
            ALGORITMO_INTERNO_FIFO,
            historico,
            configuracao,
            processosPorPid,
            &resultado->trocasFIFO
        )
    ) {
        return 0;
    }

    if (
        !executarUmAlgoritmo(
            ALGORITMO_INTERNO_LRU,
            historico,
            configuracao,
            processosPorPid,
            &resultado->trocasLRU
        )
    ) {
        return 0;
    }

    if (
        !executarUmAlgoritmo(
            ALGORITMO_INTERNO_NUF,
            historico,
            configuracao,
            processosPorPid,
            &resultado->trocasNUF
        )
    ) {
        return 0;
    }

    if (
        !executarUmAlgoritmo(
            ALGORITMO_INTERNO_OTIMO,
            historico,
            configuracao,
            processosPorPid,
            &resultado->trocasOtimo
        )
    ) {
        return 0;
    }

    resultado->melhorAlgoritmo =
        descobrirMelhorAlgoritmo(resultado);

    return 1;
}

// Mostra a linha final que o trabalho pediu
void mostrarResultadoMemoria(
    const ResultadoMemoria *resultado
) {
    const char *nomeMelhor;

    if (resultado == NULL) {
        return;
    }

    nomeMelhor = "invalido";

    if (
        resultado->melhorAlgoritmo ==
        MELHOR_ALGORITMO_FIFO
    ) {
        nomeMelhor = "FIFO";
    } else if (
        resultado->melhorAlgoritmo ==
        MELHOR_ALGORITMO_LRU
    ) {
        nomeMelhor = "LRU";
    } else if (
        resultado->melhorAlgoritmo ==
        MELHOR_ALGORITMO_NUF
    ) {
        nomeMelhor = "NUF";
    } else if (
        resultado->melhorAlgoritmo ==
        MELHOR_ALGORITMO_EMPATE
    ) {
        nomeMelhor = "empate";
    }

    printf(
        "%d|%d|%d|%d|%s\n",
        resultado->trocasFIFO,
        resultado->trocasLRU,
        resultado->trocasNUF,
        resultado->trocasOtimo,
        nomeMelhor
    );
}