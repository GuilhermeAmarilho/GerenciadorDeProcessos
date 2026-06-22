#include <stdlib.h>
#include "arvore_rubro_negra.h"

// Pega a quantidade de bilhetes do processo
// Se tiver um valor errado, usa 1 mesmo
static long long obterQuantidadeBilhetesDoProcesso(Processo *processo) {
    long long bilhetes;

    bilhetes = processo->prioridadeOuBilhetes;

    if (bilhetes <= 0) {
        bilhetes = 1;
    }

    return bilhetes;
}

// Compara dois processos
// A comparacao muda dependendo do tipo da arvore
static int compararProcessos(
    ArvoreRN *arvore,
    Processo *processoA,
    Processo *processoB
) {
    // Arvore usada para procurar pelo PID
    if (arvore->tipoOrdenacao == ORDENAR_POR_PID) {
        if (processoA->pid < processoB->pid) {
            return -1;
        }

        if (processoA->pid > processoB->pid) {
            return 1;
        }

        return 0;
    }

    // Arvore usada para guardar os processos que ainda vao chegar
    if (arvore->tipoOrdenacao == ORDENAR_POR_CRIACAO) {
        if (processoA->momentoCriacao < processoB->momentoCriacao) {
            return -1;
        }

        if (processoA->momentoCriacao > processoB->momentoCriacao) {
            return 1;
        }
    }

    // Arvore usada no algoritmo de prioridade
    if (arvore->tipoOrdenacao == ORDENAR_POR_PRIORIDADE) {
        if (
            processoA->prioridadeOuBilhetes <
            processoB->prioridadeOuBilhetes
        ) {
            return -1;
        }

        if (
            processoA->prioridadeOuBilhetes >
            processoB->prioridadeOuBilhetes
        ) {
            return 1;
        }

        // Se a prioridade empatou, pega quem chegou primeiro
        if (processoA->momentoCriacao < processoB->momentoCriacao) {
            return -1;
        }

        if (processoA->momentoCriacao > processoB->momentoCriacao) {
            return 1;
        }
    }

    // Arvore usada no CFS
    if (arvore->tipoOrdenacao == ORDENAR_POR_VRUNTIME) {
        if (processoA->vruntime < processoB->vruntime) {
            return -1;
        }

        if (processoA->vruntime > processoB->vruntime) {
            return 1;
        }

        // Se o vruntime empatou, pega quem chegou primeiro
        if (processoA->momentoCriacao < processoB->momentoCriacao) {
            return -1;
        }

        if (processoA->momentoCriacao > processoB->momentoCriacao) {
            return 1;
        }
    }

    // Se ainda empatou, usa o PID para desempatar
    if (processoA->pid < processoB->pid) {
        return -1;
    }

    if (processoA->pid > processoB->pid) {
        return 1;
    }

    return 0;
}

// Atualiza a soma dos bilhetes de um no
static void atualizarSomaDoNo(
    ArvoreRN *arvore,
    NoRN *no
) {
    long long somaEsquerda;
    long long somaDireita;
    long long bilhetesDoNo;

    // O no nulo nao tem bilhete
    if (no == arvore->nulo) {
        return;
    }

    somaEsquerda = no->esquerda->somaBilhetesSubarvore;
    somaDireita = no->direita->somaBilhetesSubarvore;
    bilhetesDoNo = obterQuantidadeBilhetesDoProcesso(
        no->processo
    );

    no->somaBilhetesSubarvore =
        somaEsquerda +
        bilhetesDoNo +
        somaDireita;
}

// Vai atualizando as somas ate chegar na raiz
static void atualizarSomasAteRaiz(
    ArvoreRN *arvore,
    NoRN *no
) {
    NoRN *atual;

    atual = no;

    while (atual != arvore->nulo) {
        atualizarSomaDoNo(
            arvore,
            atual
        );

        atual = atual->pai;
    }
}

// Cria um no novo
static NoRN *criarNo(
    ArvoreRN *arvore,
    Processo *processo
) {
    NoRN *novoNo;

    novoNo = malloc(sizeof(NoRN));

    // Deu ruim na memoria
    if (novoNo == NULL) {
        return NULL;
    }

    novoNo->processo = processo;
    novoNo->cor = VERMELHO;

    novoNo->pai = arvore->nulo;
    novoNo->esquerda = arvore->nulo;
    novoNo->direita = arvore->nulo;

    novoNo->somaBilhetesSubarvore =
        obterQuantidadeBilhetesDoProcesso(
            processo
        );

    return novoNo;
}

// Faz uma rotacao para a esquerda
static void rotacionarEsquerda(
    ArvoreRN *arvore,
    NoRN *no
) {
    NoRN *filhoDireito;

    filhoDireito = no->direita;

    // O filho esquerdo do outro no muda de lugar
    no->direita = filhoDireito->esquerda;

    if (filhoDireito->esquerda != arvore->nulo) {
        filhoDireito->esquerda->pai = no;
    }

    filhoDireito->pai = no->pai;

    // Se o no era a raiz, o filho vira a nova raiz
    if (no->pai == arvore->nulo) {
        arvore->raiz = filhoDireito;
    } else {
        if (no == no->pai->esquerda) {
            no->pai->esquerda = filhoDireito;
        } else {
            no->pai->direita = filhoDireito;
        }
    }

    filhoDireito->esquerda = no;
    no->pai = filhoDireito;

    // Depois da rotacao tem que arrumar as somas
    atualizarSomaDoNo(
        arvore,
        no
    );

    atualizarSomaDoNo(
        arvore,
        filhoDireito
    );
}

// Faz uma rotacao para a direita
static void rotacionarDireita(
    ArvoreRN *arvore,
    NoRN *no
) {
    NoRN *filhoEsquerdo;

    filhoEsquerdo = no->esquerda;

    // O filho direito do outro no muda de lugar
    no->esquerda = filhoEsquerdo->direita;

    if (filhoEsquerdo->direita != arvore->nulo) {
        filhoEsquerdo->direita->pai = no;
    }

    filhoEsquerdo->pai = no->pai;

    // Se o no era a raiz, o filho vira a nova raiz
    if (no->pai == arvore->nulo) {
        arvore->raiz = filhoEsquerdo;
    } else {
        if (no == no->pai->direita) {
            no->pai->direita = filhoEsquerdo;
        } else {
            no->pai->esquerda = filhoEsquerdo;
        }
    }

    filhoEsquerdo->direita = no;
    no->pai = filhoEsquerdo;

    // Depois da rotacao tem que arrumar as somas
    atualizarSomaDoNo(
        arvore,
        no
    );

    atualizarSomaDoNo(
        arvore,
        filhoEsquerdo
    );
}

// Arruma as cores depois de inserir
static void corrigirInsercao(
    ArvoreRN *arvore,
    NoRN *no
) {
    NoRN *tio;

    // Enquanto o pai for vermelho, tem coisa errada
    while (no->pai->cor == VERMELHO) {
        // O pai esta do lado esquerdo do avo
        if (no->pai == no->pai->pai->esquerda) {
            tio = no->pai->pai->direita;

            // Pai e tio vermelhos
            if (tio->cor == VERMELHO) {
                no->pai->cor = PRETO;
                tio->cor = PRETO;

                no->pai->pai->cor = VERMELHO;

                no = no->pai->pai;
            } else {
                // O no esta fazendo um formato meio torto
                if (no == no->pai->direita) {
                    no = no->pai;

                    rotacionarEsquerda(
                        arvore,
                        no
                    );
                }

                no->pai->cor = PRETO;
                no->pai->pai->cor = VERMELHO;

                rotacionarDireita(
                    arvore,
                    no->pai->pai
                );
            }
        } else {
            // Aqui e a mesma coisa, mas do outro lado
            tio = no->pai->pai->esquerda;

            if (tio->cor == VERMELHO) {
                no->pai->cor = PRETO;
                tio->cor = PRETO;

                no->pai->pai->cor = VERMELHO;

                no = no->pai->pai;
            } else {
                if (no == no->pai->esquerda) {
                    no = no->pai;

                    rotacionarDireita(
                        arvore,
                        no
                    );
                }

                no->pai->cor = PRETO;
                no->pai->pai->cor = VERMELHO;

                rotacionarEsquerda(
                    arvore,
                    no->pai->pai
                );
            }
        }
    }

    // A raiz sempre precisa ser preta
    arvore->raiz->cor = PRETO;
    arvore->raiz->pai = arvore->nulo;
}

// Procura o menor no partindo de um lugar da arvore
static NoRN *obterMenorNoPartindoDe(
    ArvoreRN *arvore,
    NoRN *no
) {
    NoRN *atual;

    atual = no;

    // O menor sempre fica o maximo possivel para a esquerda
    while (atual->esquerda != arvore->nulo) {
        atual = atual->esquerda;
    }

    return atual;
}

// Troca uma parte da arvore por outra
static void trocarSubarvore(
    ArvoreRN *arvore,
    NoRN *noAntigo,
    NoRN *noNovo
) {
    // O no antigo era a raiz
    if (noAntigo->pai == arvore->nulo) {
        arvore->raiz = noNovo;
    } else {
        if (noAntigo == noAntigo->pai->esquerda) {
            noAntigo->pai->esquerda = noNovo;
        } else {
            noAntigo->pai->direita = noNovo;
        }
    }

    noNovo->pai = noAntigo->pai;
}

// Arruma as cores depois de remover
static void corrigirRemocao(
    ArvoreRN *arvore,
    NoRN *no
) {
    NoRN *irmao;

    // Continua ate chegar na raiz ou ate o no deixar de ser preto
    while (
        no != arvore->raiz &&
        no->cor == PRETO
    ) {
        // O no esta no lado esquerdo
        if (no == no->pai->esquerda) {
            irmao = no->pai->direita;

            // O irmao esta vermelho
            if (irmao->cor == VERMELHO) {
                irmao->cor = PRETO;
                no->pai->cor = VERMELHO;

                rotacionarEsquerda(
                    arvore,
                    no->pai
                );

                irmao = no->pai->direita;
            }

            // Os dois filhos do irmao sao pretos
            if (
                irmao->esquerda->cor == PRETO &&
                irmao->direita->cor == PRETO
            ) {
                irmao->cor = VERMELHO;
                no = no->pai;
            } else {
                // O filho direito esta preto
                if (irmao->direita->cor == PRETO) {
                    irmao->esquerda->cor = PRETO;
                    irmao->cor = VERMELHO;

                    rotacionarDireita(
                        arvore,
                        irmao
                    );

                    irmao = no->pai->direita;
                }

                irmao->cor = no->pai->cor;
                no->pai->cor = PRETO;
                irmao->direita->cor = PRETO;

                rotacionarEsquerda(
                    arvore,
                    no->pai
                );

                no = arvore->raiz;
            }
        } else {
            // Aqui e a mesma coisa, mas do outro lado
            irmao = no->pai->esquerda;

            if (irmao->cor == VERMELHO) {
                irmao->cor = PRETO;
                no->pai->cor = VERMELHO;

                rotacionarDireita(
                    arvore,
                    no->pai
                );

                irmao = no->pai->esquerda;
            }

            if (
                irmao->direita->cor == PRETO &&
                irmao->esquerda->cor == PRETO
            ) {
                irmao->cor = VERMELHO;
                no = no->pai;
            } else {
                if (irmao->esquerda->cor == PRETO) {
                    irmao->direita->cor = PRETO;
                    irmao->cor = VERMELHO;

                    rotacionarEsquerda(
                        arvore,
                        irmao
                    );

                    irmao = no->pai->esquerda;
                }

                irmao->cor = no->pai->cor;
                no->pai->cor = PRETO;
                irmao->esquerda->cor = PRETO;

                rotacionarDireita(
                    arvore,
                    no->pai
                );

                no = arvore->raiz;
            }
        }
    }

    no->cor = PRETO;
}

// Libera os nos usando recursao
static void destruirNos(
    ArvoreRN *arvore,
    NoRN *no,
    int liberarProcessos
) {
    // Chegou no fim dessa parte da arvore
    if (no == arvore->nulo) {
        return;
    }

    destruirNos(
        arvore,
        no->esquerda,
        liberarProcessos
    );

    destruirNos(
        arvore,
        no->direita,
        liberarProcessos
    );

    // So libera o processo se foi pedido
    if (liberarProcessos == 1) {
        destruirProcesso(
            no->processo
        );
    }

    free(no);
}

// Prepara uma arvore vazia
void inicializarArvore(
    ArvoreRN *arvore,
    TipoOrdenacaoArvore tipoOrdenacao
) {
    // Cria o no nulo que vai ser usado no lugar de NULL
    arvore->nulo = malloc(sizeof(NoRN));

    // Nao conseguiu criar o no nulo
    if (arvore->nulo == NULL) {
        arvore->raiz = NULL;
        arvore->quantidade = 0;
        arvore->tipoOrdenacao = tipoOrdenacao;

        return;
    }

    arvore->nulo->processo = NULL;
    arvore->nulo->cor = PRETO;

    arvore->nulo->pai = arvore->nulo;
    arvore->nulo->esquerda = arvore->nulo;
    arvore->nulo->direita = arvore->nulo;

    arvore->nulo->somaBilhetesSubarvore = 0;

    arvore->raiz = arvore->nulo;
    arvore->quantidade = 0;
    arvore->tipoOrdenacao = tipoOrdenacao;
}

// Verifica se a arvore esta vazia
int arvoreEstaVazia(ArvoreRN *arvore) {
    if (arvore == NULL) {
        return 1;
    }

    if (arvore->nulo == NULL) {
        return 1;
    }

    if (arvore->raiz == arvore->nulo) {
        return 1;
    }

    return 0;
}

// Coloca um processo na arvore
int inserirProcessoNaArvore(
    ArvoreRN *arvore,
    Processo *processo
) {
    NoRN *novoNo;
    NoRN *pai;
    NoRN *atual;

    int comparacao;

    if (
        arvore == NULL ||
        processo == NULL
    ) {
        return 0;
    }

    if (arvore->nulo == NULL) {
        return 0;
    }

    novoNo = criarNo(
        arvore,
        processo
    );

    if (novoNo == NULL) {
        return 0;
    }

    pai = arvore->nulo;
    atual = arvore->raiz;

    // Procura o lugar certo para colocar o processo
    while (atual != arvore->nulo) {
        pai = atual;

        comparacao = compararProcessos(
            arvore,
            processo,
            atual->processo
        );

        // Ja tem um processo com a mesma chave
        if (comparacao == 0) {
            free(novoNo);
            return 0;
        }

        if (comparacao < 0) {
            atual = atual->esquerda;
        } else {
            atual = atual->direita;
        }
    }

    novoNo->pai = pai;

    // Se nao tinha pai, virou a raiz
    if (pai == arvore->nulo) {
        arvore->raiz = novoNo;
    } else {
        comparacao = compararProcessos(
            arvore,
            processo,
            pai->processo
        );

        if (comparacao < 0) {
            pai->esquerda = novoNo;
        } else {
            pai->direita = novoNo;
        }
    }

    // Atualiza os bilhetes antes e depois de arrumar a arvore
    atualizarSomasAteRaiz(
        arvore,
        novoNo
    );

    corrigirInsercao(
        arvore,
        novoNo
    );

    atualizarSomasAteRaiz(
        arvore,
        novoNo
    );

    arvore->quantidade++;

    return 1;
}

// Procura um no usando a ordenacao da arvore
NoRN *buscarNoNaArvore(
    ArvoreRN *arvore,
    Processo *processo
) {
    NoRN *atual;
    int comparacao;

    if (
        arvore == NULL ||
        processo == NULL
    ) {
        return NULL;
    }

    if (arvore->nulo == NULL) {
        return NULL;
    }

    atual = arvore->raiz;

    while (atual != arvore->nulo) {
        comparacao = compararProcessos(
            arvore,
            processo,
            atual->processo
        );

        if (comparacao == 0) {
            return atual;
        }

        if (comparacao < 0) {
            atual = atual->esquerda;
        } else {
            atual = atual->direita;
        }
    }

    return NULL;
}

// Procura um processo pelo PID
Processo *buscarProcessoPorPid(
    ArvoreRN *arvore,
    int pid
) {
    NoRN *atual;

    if (arvore == NULL) {
        return NULL;
    }

    if (arvore->nulo == NULL) {
        return NULL;
    }

    // Essa funcao so funciona rapido se a arvore estiver por PID
    if (arvore->tipoOrdenacao != ORDENAR_POR_PID) {
        return NULL;
    }

    atual = arvore->raiz;

    while (atual != arvore->nulo) {
        if (pid == atual->processo->pid) {
            return atual->processo;
        }

        if (pid < atual->processo->pid) {
            atual = atual->esquerda;
        } else {
            atual = atual->direita;
        }
    }

    return NULL;
}

// Retorna o menor no da arvore
NoRN *obterMenorNo(ArvoreRN *arvore) {
    if (arvoreEstaVazia(arvore)) {
        return NULL;
    }

    return obterMenorNoPartindoDe(
        arvore,
        arvore->raiz
    );
}

// Retorna o processo do menor no
Processo *obterMenorProcesso(ArvoreRN *arvore) {
    NoRN *menorNo;

    menorNo = obterMenorNo(arvore);

    if (menorNo == NULL) {
        return NULL;
    }

    return menorNo->processo;
}

// Retorna o proximo no da arvore
NoRN *obterProximoNo(
    ArvoreRN *arvore,
    NoRN *noAtual
) {
    NoRN *pai;
    NoRN *atual;

    if (
        arvore == NULL ||
        noAtual == NULL
    ) {
        return NULL;
    }

    if (
        arvore->nulo == NULL ||
        noAtual == arvore->nulo
    ) {
        return NULL;
    }

    // Se tem filho direito, procura o menor daquele lado
    if (noAtual->direita != arvore->nulo) {
        return obterMenorNoPartindoDe(
            arvore,
            noAtual->direita
        );
    }

    atual = noAtual;
    pai = atual->pai;

    // Vai subindo ate encontrar o proximo
    while (
        pai != arvore->nulo &&
        atual == pai->direita
    ) {
        atual = pai;
        pai = pai->pai;
    }

    if (pai == arvore->nulo) {
        return NULL;
    }

    return pai;
}

// Remove um no da arvore
int removerNoDaArvore(
    ArvoreRN *arvore,
    NoRN *no
) {
    NoRN *noMovido;
    NoRN *filhoParaCorrigir;
    NoRN *paiParaAtualizar;

    CorNo corOriginal;

    if (
        arvore == NULL ||
        no == NULL
    ) {
        return 0;
    }

    if (
        arvore->nulo == NULL ||
        no == arvore->nulo
    ) {
        return 0;
    }

    noMovido = no;
    corOriginal = noMovido->cor;
    paiParaAtualizar = arvore->nulo;

    // O no nao tem filho esquerdo
    if (no->esquerda == arvore->nulo) {
        filhoParaCorrigir = no->direita;
        paiParaAtualizar = no->pai;

        trocarSubarvore(
            arvore,
            no,
            no->direita
        );
    } else {
        // O no nao tem filho direito
        if (no->direita == arvore->nulo) {
            filhoParaCorrigir = no->esquerda;
            paiParaAtualizar = no->pai;

            trocarSubarvore(
                arvore,
                no,
                no->esquerda
            );
        } else {
            // O no tem os dois filhos
            noMovido = obterMenorNoPartindoDe(
                arvore,
                no->direita
            );

            corOriginal = noMovido->cor;
            filhoParaCorrigir = noMovido->direita;

            if (noMovido->pai == no) {
                filhoParaCorrigir->pai = noMovido;
            } else {
                paiParaAtualizar = noMovido->pai;

                trocarSubarvore(
                    arvore,
                    noMovido,
                    noMovido->direita
                );

                atualizarSomasAteRaiz(
                    arvore,
                    paiParaAtualizar
                );

                noMovido->direita = no->direita;
                noMovido->direita->pai = noMovido;
            }

            trocarSubarvore(
                arvore,
                no,
                noMovido
            );

            noMovido->esquerda = no->esquerda;
            noMovido->esquerda->pai = noMovido;

            noMovido->cor = no->cor;

            atualizarSomaDoNo(
                arvore,
                noMovido
            );

            atualizarSomasAteRaiz(
                arvore,
                noMovido
            );
        }
    }

    if (paiParaAtualizar != arvore->nulo) {
        atualizarSomasAteRaiz(
            arvore,
            paiParaAtualizar
        );
    }

    // Se saiu um no preto, precisa arrumar as cores
    if (corOriginal == PRETO) {
        corrigirRemocao(
            arvore,
            filhoParaCorrigir
        );
    }

    // Arruma a soma dos bilhetes depois da remocao
    if (filhoParaCorrigir == arvore->nulo) {
        atualizarSomasAteRaiz(
            arvore,
            filhoParaCorrigir->pai
        );
    } else {
        atualizarSomasAteRaiz(
            arvore,
            filhoParaCorrigir
        );
    }

    // Libera somente o no
    // O processo continua existindo
    free(no);

    arvore->quantidade--;

    // Arruma o pai da raiz
    if (arvore->raiz != arvore->nulo) {
        arvore->raiz->pai = arvore->nulo;
    } else {
        arvore->nulo->pai = arvore->nulo;
    }

    return 1;
}

// Procura o processo e remove o no dele
int removerProcessoDaArvore(
    ArvoreRN *arvore,
    Processo *processo
) {
    NoRN *no;

    no = buscarNoNaArvore(
        arvore,
        processo
    );

    if (no == NULL) {
        return 0;
    }

    return removerNoDaArvore(
        arvore,
        no
    );
}

// Retorna a quantidade total de bilhetes
long long obterTotalBilhetes(
    ArvoreRN *arvore
) {
    if (arvoreEstaVazia(arvore)) {
        return 0;
    }

    return arvore->raiz->somaBilhetesSubarvore;
}

// Procura o processo que ganhou o sorteio
Processo *buscarProcessoPorBilhete(
    ArvoreRN *arvore,
    long long bilheteSorteado
) {
    NoRN *atual;

    long long somaEsquerda;
    long long bilhetesDoAtual;
    long long totalBilhetes;

    if (arvoreEstaVazia(arvore)) {
        return NULL;
    }

    totalBilhetes = obterTotalBilhetes(
        arvore
    );

    // O bilhete precisa estar dentro do total
    if (
        bilheteSorteado < 1 ||
        bilheteSorteado > totalBilhetes
    ) {
        return NULL;
    }

    atual = arvore->raiz;

    while (atual != arvore->nulo) {
        somaEsquerda =
            atual->esquerda->somaBilhetesSubarvore;

        bilhetesDoAtual =
            obterQuantidadeBilhetesDoProcesso(
                atual->processo
            );

        // O bilhete esta na parte esquerda
        if (bilheteSorteado <= somaEsquerda) {
            atual = atual->esquerda;
        } else {
            // O bilhete pertence ao processo atual
            if (
                bilheteSorteado <=
                somaEsquerda + bilhetesDoAtual
            ) {
                return atual->processo;
            }

            // Tira os bilhetes que ja foram pulados
            bilheteSorteado =
                bilheteSorteado -
                somaEsquerda -
                bilhetesDoAtual;

            atual = atual->direita;
        }
    }

    return NULL;
}

// Libera a arvore inteira
void destruirArvore(
    ArvoreRN *arvore,
    int liberarProcessos
) {
    if (arvore == NULL) {
        return;
    }

    if (arvore->nulo == NULL) {
        return;
    }

    destruirNos(
        arvore,
        arvore->raiz,
        liberarProcessos
    );

    free(arvore->nulo);

    arvore->raiz = NULL;
    arvore->nulo = NULL;
    arvore->quantidade = 0;
}