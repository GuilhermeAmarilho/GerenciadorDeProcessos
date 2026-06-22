#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include "processo.h"
#include "arvore_rubro_negra.h"

// O main usa um texto de 50 posicoes para o algoritmo
#define TAM_NOME_ALGORITMO 50

// Verifica se a linha esta vazia ou so tem espaco
static int linhaEstaVazia(char texto[]) {
    int i;

    i = 0;

    while (texto[i] != '\0') {
        if (!isspace((unsigned char) texto[i])) {
            return 0;
        }

        i++;
    }

    return 1;
}

// Tira os espacos do comeco e do final
static void removerEspacosDasPontas(char texto[]) {
    int inicio;
    int final;
    int tamanho;

    inicio = 0;

    while (
        texto[inicio] != '\0' &&
        isspace((unsigned char) texto[inicio])
    ) {
        inicio++;
    }

    // Move o texto para o comeco
    if (inicio > 0) {
        tamanho = strlen(texto);

        memmove(
            texto,
            texto + inicio,
            tamanho - inicio + 1
        );
    }

    final = strlen(texto);

    // Vai apagando os espacos do final
    while (final > 0) {
        if (!isspace((unsigned char) texto[final - 1])) {
            break;
        }

        texto[final - 1] = '\0';
        final--;
    }
}

// Converte um texto para inteiro
// Retorna 1 se deu certo e 0 se o texto estava errado
static int converterTextoParaInteiro(
    char texto[],
    int *valor
) {
    char *fim;
    long numero;

    if (
        texto == NULL ||
        valor == NULL
    ) {
        return 0;
    }

    removerEspacosDasPontas(texto);

    if (texto[0] == '\0') {
        return 0;
    }

    errno = 0;

    numero = strtol(
        texto,
        &fim,
        10
    );

    // Nao conseguiu ler nenhum numero
    if (fim == texto) {
        return 0;
    }

    // Ignora espacos depois do numero
    while (
        fim[0] != '\0' &&
        isspace((unsigned char) fim[0])
    ) {
        fim++;
    }

    // Tinha alguma coisa estranha depois do numero
    if (fim[0] != '\0') {
        return 0;
    }

    // O numero ficou grande demais ou pequeno demais
    if (
        errno == ERANGE ||
        numero < INT_MIN ||
        numero > INT_MAX
    ) {
        return 0;
    }

    *valor = (int) numero;

    return 1;
}

// Limpa tudo que ja tinha sido carregado
// Isso e usado quando aparece algum erro no arquivo
static void limparProcessosCarregados(
    ArvoreRN *processosPorPid,
    ArvoreRN *processosFuturos,
    int *quantidade
) {
    // Primeiro tira os nos da arvore de futuros
    // Nao libera os processos porque eles tambem estao na arvore por PID
    destruirArvore(
        processosFuturos,
        0
    );

    // Aqui libera os nos e tambem os processos
    destruirArvore(
        processosPorPid,
        1
    );

    if (quantidade != NULL) {
        *quantidade = 0;
    }
}

// Remove a quebra de linha
void removerQuebraLinha(char texto[]) {
    int i;

    i = 0;

    while (texto[i] != '\0') {
        if (
            texto[i] == '\n' ||
            texto[i] == '\r'
        ) {
            texto[i] = '\0';
            return;
        }

        i++;
    }
}

// Coloca o texto em maiusculo
void paraMaiusculo(char texto[]) {
    int i;

    i = 0;

    while (texto[i] != '\0') {
        texto[i] = (char) toupper(
            (unsigned char) texto[i]
        );

        i++;
    }
}

// Verifica se o algoritmo existe
int algoritmoValido(char algoritmo[]) {
    if (algoritmo == NULL) {
        return 0;
    }

    if (strcmp(algoritmo, "RR") == 0) {
        return 1;
    }

    if (strcmp(algoritmo, "ALTERNANCIA") == 0) {
        return 1;
    }

    if (strcmp(algoritmo, "ALTERNANCIA_CIRCULAR") == 0) {
        return 1;
    }

    if (strcmp(algoritmo, "PRIORIDADE") == 0) {
        return 1;
    }

    if (strcmp(algoritmo, "LOTERIA") == 0) {
        return 1;
    }

    if (strcmp(algoritmo, "CFS") == 0) {
        return 1;
    }

    return 0;
}

// Cria um processo novo na memoria
Processo *criarProcesso(
    int momentoCriacao,
    int pid,
    int tempoExecucao,
    int prioridadeOuBilhetes
) {
    Processo *novoProcesso;

    novoProcesso = malloc(sizeof(Processo));

    if (novoProcesso == NULL) {
        return NULL;
    }

    novoProcesso->momentoCriacao = momentoCriacao;
    novoProcesso->pid = pid;
    novoProcesso->tempoExecucao = tempoExecucao;
    novoProcesso->prioridadeOuBilhetes = prioridadeOuBilhetes;

    novoProcesso->tempoRestante = tempoExecucao;
    novoProcesso->tempoConclusao = -1;
    novoProcesso->tempoPronto = 0;
    novoProcesso->primeiraExecucao = -1;
    novoProcesso->tempoResposta = 0;
    novoProcesso->vruntime = 0.0;

    // Quando le o arquivo, ele ainda nao chegou no sistema
    novoProcesso->estado = PROCESSO_FUTURO;

    return novoProcesso;
}

// Libera um processo da memoria
void destruirProcesso(Processo *processo) {
    if (processo == NULL) {
        return;
    }

    free(processo);
}

// Verifica se o processo ja pode ir para pronto
int processoPodeFicarPronto(
    Processo *processo,
    int tempoAtual
) {
    if (processo == NULL) {
        return 0;
    }

    if (processo->estado != PROCESSO_FUTURO) {
        return 0;
    }

    if (processo->tempoRestante <= 0) {
        return 0;
    }

    if (processo->momentoCriacao > tempoAtual) {
        return 0;
    }

    return 1;
}

// Verifica se o processo acabou
int processoTerminou(Processo *processo) {
    if (processo == NULL) {
        return 0;
    }

    if (processo->tempoRestante <= 0) {
        return 1;
    }

    return 0;
}

// Coloca o processo como pronto
void deixarProcessoPronto(Processo *processo) {
    if (processo == NULL) {
        return;
    }

    if (processoTerminou(processo)) {
        return;
    }

    processo->estado = PROCESSO_PRONTO;
}

// Coloca o processo como executando
void deixarProcessoExecutando(Processo *processo) {
    if (processo == NULL) {
        return;
    }

    if (processoTerminou(processo)) {
        return;
    }

    processo->estado = PROCESSO_EXECUTANDO;
}

// Finaliza o processo
void finalizarProcesso(
    Processo *processo,
    int tempoAtual
) {
    if (processo == NULL) {
        return;
    }

    processo->tempoRestante = 0;
    processo->tempoConclusao = tempoAtual;
    processo->estado = PROCESSO_FINALIZADO;
}

// Soma um tempo na espera do processo
void incrementarTempoPronto(Processo *processo) {
    if (processo == NULL) {
        return;
    }

    // So soma se ele realmente estiver esperando CPU
    if (processo->estado != PROCESSO_PRONTO) {
        return;
    }

    processo->tempoPronto++;
}

// Mostra os resultados finais
void mostrarResultadoFinal(ArvoreRN *processosPorPid) {
    NoRN *noAtual;
    NoRN *proximoNo;
    Processo *processo;

    int quantidade;
    int tempoTotal;

    double somaTempoTotal;
    double somaTempoPronto;
    double somaTempoResposta;

    if (processosPorPid == NULL) {
        printf("Erro: arvore de processos nao encontrada.\n");
        return;
    }

    if (arvoreEstaVazia(processosPorPid)) {
        printf("Nenhum processo para mostrar.\n");
        return;
    }

    quantidade = processosPorPid->quantidade;

    somaTempoTotal = 0.0;
    somaTempoPronto = 0.0;
    somaTempoResposta = 0.0;

    printf("\n================ RESULTADO FINAL ================\n");
    printf(
        "PID\tCriacao\tExecucao\tConclusao\tTotal\tPronto\tResposta\n"
    );

    noAtual = obterMenorNo(processosPorPid);

    // Como a arvore esta por PID, vai mostrar em ordem de PID
    while (noAtual != NULL) {
        processo = noAtual->processo;

        tempoTotal =
            processo->tempoConclusao -
            processo->momentoCriacao;

        somaTempoTotal += tempoTotal;
        somaTempoPronto += processo->tempoPronto;
        somaTempoResposta += processo->tempoResposta;

        printf(
            "%d\t%d\t%d\t\t%d\t\t%d\t%d\t%d\n",
            processo->pid,
            processo->momentoCriacao,
            processo->tempoExecucao,
            processo->tempoConclusao,
            tempoTotal,
            processo->tempoPronto,
            processo->tempoResposta
        );

        // Pega antes porque depois a gente muda de no
        proximoNo = obterProximoNo(
            processosPorPid,
            noAtual
        );

        noAtual = proximoNo;
    }

    printf("\n================ MEDIAS ================\n");

    printf(
        "Tempo medio total: %.2f\n",
        somaTempoTotal / quantidade
    );

    printf(
        "Tempo medio pronto: %.2f\n",
        somaTempoPronto / quantidade
    );

    printf(
        "Tempo medio de resposta: %.2f\n",
        somaTempoResposta / quantidade
    );
}

// Le o arquivo e coloca os processos nas arvores
int carregarArquivo(
    char nomeArquivo[],
    char algoritmo[],
    int *fatiaCPU,
    ArvoreRN *processosPorPid,
    ArvoreRN *processosFuturos,
    int *quantidade
) {
    FILE *arquivo;

    char linha[TAM_LINHA];
    char *separador;

    char textoFatia[TAM_LINHA];
    char caractereExtra;

    int numeroLinha;

    int momentoCriacao;
    int pid;
    int tempoExecucao;
    int prioridadeOuBilhetes;

    int quantidadeLida;
    int inseriuPorPid;
    int inseriuNosFuturos;

    Processo *novoProcesso;

    if (quantidade != NULL) {
        *quantidade = 0;
    }

    // Confere se recebeu tudo que precisava
    if (
        nomeArquivo == NULL ||
        algoritmo == NULL ||
        fatiaCPU == NULL ||
        processosPorPid == NULL ||
        processosFuturos == NULL ||
        quantidade == NULL
    ) {
        printf("Erro: dados faltando para carregar o arquivo.\n");
        return 0;
    }

    // As arvores precisam ter sido inicializadas no main
    if (
        processosPorPid->nulo == NULL ||
        processosFuturos->nulo == NULL
    ) {
        printf("Erro: as arvores nao foram inicializadas.\n");
        return 0;
    }

    // Confere se cada arvore foi criada do jeito certo
    if (
        processosPorPid->tipoOrdenacao != ORDENAR_POR_PID ||
        processosFuturos->tipoOrdenacao != ORDENAR_POR_CRIACAO
    ) {
        printf("Erro: as arvores foram criadas com a ordenacao errada.\n");
        return 0;
    }

    arquivo = fopen(
        nomeArquivo,
        "r"
    );

    if (arquivo == NULL) {
        printf(
            "Erro ao abrir o arquivo: %s\n",
            nomeArquivo
        );

        return 0;
    }

    numeroLinha = 1;

    // Le a primeira linha
    if (fgets(linha, TAM_LINHA, arquivo) == NULL) {
        printf("Erro: o arquivo esta vazio.\n");

        fclose(arquivo);
        return 0;
    }

    removerQuebraLinha(linha);

    // Procura o separador entre algoritmo e fatia
    separador = strchr(
        linha,
        '|'
    );

    if (separador == NULL) {
        printf("Erro na primeira linha do arquivo.\n");

        fclose(arquivo);
        return 0;
    }

    // Corta a linha no lugar do separador
    separador[0] = '\0';

    removerEspacosDasPontas(linha);

    if (linha[0] == '\0') {
        printf("Erro: algoritmo nao informado.\n");

        fclose(arquivo);
        return 0;
    }

    // Evita passar do tamanho do texto do algoritmo
    if (strlen(linha) >= TAM_NOME_ALGORITMO) {
        printf("Erro: nome do algoritmo muito grande.\n");

        fclose(arquivo);
        return 0;
    }

    strcpy(
        algoritmo,
        linha
    );

    paraMaiusculo(algoritmo);

    if (!algoritmoValido(algoritmo)) {
        printf(
            "Erro: algoritmo desconhecido: %s\n",
            algoritmo
        );

        printf(
            "Use: RR, PRIORIDADE, LOTERIA ou CFS.\n"
        );

        fclose(arquivo);
        return 0;
    }

    // Copia o que ficou depois do separador
    strcpy(
        textoFatia,
        separador + 1
    );

    // Nao pode ter outro separador depois
    if (strchr(textoFatia, '|') != NULL) {
        printf("Erro: primeira linha com campos demais.\n");

        fclose(arquivo);
        return 0;
    }

    if (
        !converterTextoParaInteiro(
            textoFatia,
            fatiaCPU
        )
    ) {
        printf("Erro: fatia de CPU invalida.\n");

        fclose(arquivo);
        return 0;
    }

    if (*fatiaCPU <= 0) {
        printf("Erro: a fatia de CPU precisa ser maior que zero.\n");

        fclose(arquivo);
        return 0;
    }

    // Le uma linha de processo por vez
    while (fgets(linha, TAM_LINHA, arquivo) != NULL) {
        numeroLinha++;

        removerQuebraLinha(linha);

        // Ignora linhas vazias
        if (linhaEstaVazia(linha)) {
            continue;
        }

        caractereExtra = '\0';

        // Tenta ler os quatro campos
        quantidadeLida = sscanf(
            linha,
            " %d | %d | %d | %d %c",
            &momentoCriacao,
            &pid,
            &tempoExecucao,
            &prioridadeOuBilhetes,
            &caractereExtra
        );

        // Se leu menos de quatro ou encontrou coisa sobrando
        if (quantidadeLida != 4) {
            printf(
                "Erro na linha %d: formato invalido.\n",
                numeroLinha
            );

            fclose(arquivo);

            limparProcessosCarregados(
                processosPorPid,
                processosFuturos,
                quantidade
            );

            return 0;
        }

        if (momentoCriacao < 0) {
            printf(
                "Erro na linha %d: momento de criacao invalido.\n",
                numeroLinha
            );

            fclose(arquivo);

            limparProcessosCarregados(
                processosPorPid,
                processosFuturos,
                quantidade
            );

            return 0;
        }

        if (pid < 0) {
            printf(
                "Erro na linha %d: PID invalido.\n",
                numeroLinha
            );

            fclose(arquivo);

            limparProcessosCarregados(
                processosPorPid,
                processosFuturos,
                quantidade
            );

            return 0;
        }

        if (tempoExecucao <= 0) {
            printf(
                "Erro na linha %d: tempo de execucao invalido.\n",
                numeroLinha
            );

            fclose(arquivo);

            limparProcessosCarregados(
                processosPorPid,
                processosFuturos,
                quantidade
            );

            return 0;
        }

        // Mantem o mesmo comportamento do programa antigo
        if (prioridadeOuBilhetes <= 0) {
            printf(
                "Aviso na linha %d: ultimo campo invalido. Sera usado valor 1.\n",
                numeroLinha
            );

            prioridadeOuBilhetes = 1;
        }

        // A busca agora usa a arvore e nao percorre um vetor
        if (
            buscarProcessoPorPid(
                processosPorPid,
                pid
            ) != NULL
        ) {
            printf(
                "Erro na linha %d: PID repetido: %d\n",
                numeroLinha,
                pid
            );

            fclose(arquivo);

            limparProcessosCarregados(
                processosPorPid,
                processosFuturos,
                quantidade
            );

            return 0;
        }

        novoProcesso = criarProcesso(
            momentoCriacao,
            pid,
            tempoExecucao,
            prioridadeOuBilhetes
        );

        if (novoProcesso == NULL) {
            printf(
                "Erro: nao foi possivel criar o processo da linha %d.\n",
                numeroLinha
            );

            fclose(arquivo);

            limparProcessosCarregados(
                processosPorPid,
                processosFuturos,
                quantidade
            );

            return 0;
        }

        // Guarda na arvore usada para busca e resultado final
        inseriuPorPid = inserirProcessoNaArvore(
            processosPorPid,
            novoProcesso
        );

        if (!inseriuPorPid) {
            printf(
                "Erro ao guardar o processo da linha %d na arvore por PID.\n",
                numeroLinha
            );

            destruirProcesso(novoProcesso);

            fclose(arquivo);

            limparProcessosCarregados(
                processosPorPid,
                processosFuturos,
                quantidade
            );

            return 0;
        }

        // Guarda tambem na arvore dos processos que ainda vao chegar
        inseriuNosFuturos = inserirProcessoNaArvore(
            processosFuturos,
            novoProcesso
        );

        if (!inseriuNosFuturos) {
            printf(
                "Erro ao guardar o processo da linha %d na arvore de futuros.\n",
                numeroLinha
            );

            // Tira da primeira arvore antes de liberar o processo
            removerProcessoDaArvore(
                processosPorPid,
                novoProcesso
            );

            destruirProcesso(novoProcesso);

            fclose(arquivo);

            limparProcessosCarregados(
                processosPorPid,
                processosFuturos,
                quantidade
            );

            return 0;
        }

        (*quantidade)++;
    }

    // Verifica se aconteceu algum erro durante a leitura
    if (ferror(arquivo)) {
        printf("Erro durante a leitura do arquivo.\n");

        fclose(arquivo);

        limparProcessosCarregados(
            processosPorPid,
            processosFuturos,
            quantidade
        );

        return 0;
    }

    fclose(arquivo);

    return 1;
}