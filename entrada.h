#ifndef ENTRADA_H
#define ENTRADA_H

#include "configuracao.h"

// A estrutura completa da arvore fica em outro arquivo
typedef struct ArvoreRN ArvoreRN;

// Le a configuracao e todos os processos do arquivo
// Retorna 1 se conseguiu carregar tudo
// Retorna 0 se encontrou algum erro
int carregarArquivo(
    const char nomeArquivo[],
    ConfiguracaoSistema *configuracao,
    ArvoreRN *processosPorPid,
    ArvoreRN *processosFuturos,
    int *quantidadeProcessos
);

#endif