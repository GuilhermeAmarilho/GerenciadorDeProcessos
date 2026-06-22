#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include "entrada.h"
#include "processo.h"
#include "arvore_rubro_negra.h"

// Um valor grande para caber uma sequencia grandinha de paginas
#define TAM_LINHA_ENTRADA 10000

// Tamanho usado para ler o nome da politica
#define TAM_NOME_POLITICA 20

// Verifica se a linha esta vazia ou so tem espaco
static int linhaEstaVazia(char texto[]) {
    int i;

    if (texto == NULL) {
        return 1;
    }

    i = 0;

    while (texto[i] != '\0') {
        if (!isspace((unsigned char) texto[i])) {
            return 0;
        }

        i++;
    }

    return 1;
}

// Tira a quebra de linha do final
static void removerQuebraLinha(char texto[]) {
    int i;

    if (texto == NULL) {
        return;
    }

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

// Tira os espacos do comeco e do final
static void removerEspacosDasPontas(char texto[]) {
    int inicio;
    int final;
    int tamanho;

    if (texto == NULL) {
        return;
    }

    inicio = 0;

    while (
        texto[inicio] != '\0' &&
        isspace((unsigned char) texto[inicio])
    ) {
        inicio++;
    }

    if (inicio > 0) {
        tamanho = (int) strlen(texto);

        memmove(
            texto,
            texto + inicio,
            tamanho - inicio + 1
        );
    }

    final = (int) strlen(texto);

    while (final > 0) {
        if (!isspace((unsigned char) texto[final - 1])) {
            break;
        }

        texto[final - 1] = '\0';
        final--;
    }
}

// Coloca o texto em maiusculo
static void paraMaiusculo(char texto[]) {
    int i;

    if (texto == NULL) {
        return;
    }

    i = 0;

    while (texto[i] != '\0') {
        texto[i] = (char) toupper(
            (unsigned char) texto[i]
        );

        i++;
    }
}

// Converte texto para inteiro e percebe se tinha coisa errada
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

    if (fim == texto) {
        return 0;
    }

    while (
        fim[0] != '\0' &&
        isspace((unsigned char) fim[0])
    ) {
        fim++;
    }

    if (fim[0] != '\0') {
        return 0;
    }

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

// Separa uma linha usando o caractere |
// Retorna quantos campos foram encontrados
static int separarCampos(
    char linha[],
    char *campos[],
    int limiteCampos
) {
    int quantidade;
    int i;

    if (
        linha == NULL ||
        campos == NULL ||
        limiteCampos <= 0
    ) {
        return 0;
    }

    quantidade = 1;
    campos[0] = linha;
    i = 0;

    while (linha[i] != '\0') {
        if (linha[i] == '|') {
            linha[i] = '\0';

            if (quantidade >= limiteCampos) {
                return limiteCampos + 1;
            }

            campos[quantidade] = linha + i + 1;
            quantidade++;
        }

        i++;
    }

    return quantidade;
}

// Verifica se o algoritmo informado existe
static int algoritmoValido(char algoritmo[]) {
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

// Converte o texto local ou global para o enum
static PoliticaMemoria converterPoliticaMemoria(char texto[]) {
    if (texto == NULL) {
        return POLITICA_INVALIDA;
    }

    removerEspacosDasPontas(texto);
    paraMaiusculo(texto);

    if (strcmp(texto, "LOCAL") == 0) {
        return POLITICA_LOCAL;
    }

    if (strcmp(texto, "GLOBAL") == 0) {
        return POLITICA_GLOBAL;
    }

    return POLITICA_INVALIDA;
}

// Le uma linha e tambem percebe se ela ficou maior que o vetor
static int lerLinhaDoArquivo(
    FILE *arquivo,
    char linha[],
    int tamanhoLinha
) {
    int caractere;

    if (
        arquivo == NULL ||
        linha == NULL ||
        tamanhoLinha <= 1
    ) {
        return 0;
    }

    if (fgets(linha, tamanhoLinha, arquivo) == NULL) {
        return 0;
    }

    if (
        strchr(linha, '\n') == NULL &&
        !feof(arquivo)
    ) {
        caractere = fgetc(arquivo);

        while (
            caractere != '\n' &&
            caractere != EOF
        ) {
            caractere = fgetc(arquivo);
        }

        return -1;
    }

    removerQuebraLinha(linha);

    return 1;
}

// Limpa os processos quando aparece um erro no meio da leitura
static void limparProcessosCarregados(
    ArvoreRN *processosPorPid,
    ArvoreRN *processosFuturos,
    int *quantidadeProcessos
) {
    destruirArvore(
        processosFuturos,
        0
    );

    destruirArvore(
        processosPorPid,
        1
    );

    if (quantidadeProcessos != NULL) {
        *quantidadeProcessos = 0;
    }
}

// Le os seis campos da primeira linha
static int lerConfiguracao(
    char linha[],
    ConfiguracaoSistema *configuracao
) {
    char *campos[6];
    char textoPolitica[TAM_NOME_POLITICA];

    int quantidadeCampos;
    int i;

    if (
        linha == NULL ||
        configuracao == NULL
    ) {
        return 0;
    }

    quantidadeCampos = separarCampos(
        linha,
        campos,
        6
    );

    if (quantidadeCampos != 6) {
        printf("Erro: a primeira linha precisa ter 6 campos.\n");
        return 0;
    }

    i = 0;

    while (i < 6) {
        removerEspacosDasPontas(campos[i]);

        if (campos[i][0] == '\0') {
            printf("Erro: tem campo vazio na primeira linha.\n");
            return 0;
        }

        i++;
    }

    if (strlen(campos[0]) >= TAM_NOME_ALGORITMO) {
        printf("Erro: nome do algoritmo muito grande.\n");
        return 0;
    }

    strcpy(
        configuracao->algoritmoEscalonamento,
        campos[0]
    );

    paraMaiusculo(
        configuracao->algoritmoEscalonamento
    );

    if (!algoritmoValido(configuracao->algoritmoEscalonamento)) {
        printf(
            "Erro: algoritmo desconhecido: %s\n",
            configuracao->algoritmoEscalonamento
        );

        printf(
            "Use: RR, ALTERNANCIA, PRIORIDADE, LOTERIA ou CFS.\n"
        );

        return 0;
    }

    if (
        !converterTextoParaInteiro(
            campos[1],
            &configuracao->fatiaCPU
        )
    ) {
        printf("Erro: fatia de CPU invalida.\n");
        return 0;
    }

    if (configuracao->fatiaCPU <= 0) {
        printf("Erro: a fatia de CPU precisa ser maior que zero.\n");
        return 0;
    }

    if (strlen(campos[2]) >= TAM_NOME_POLITICA) {
        printf("Erro: nome da politica de memoria muito grande.\n");
        return 0;
    }

    strcpy(
        textoPolitica,
        campos[2]
    );

    configuracao->politicaMemoria = converterPoliticaMemoria(
        textoPolitica
    );

    if (configuracao->politicaMemoria == POLITICA_INVALIDA) {
        printf("Erro: politica de memoria invalida.\n");
        printf("Use: LOCAL ou GLOBAL.\n");
        return 0;
    }

    if (
        !converterTextoParaInteiro(
            campos[3],
            &configuracao->tamanhoMemoria
        )
    ) {
        printf("Erro: tamanho da memoria invalido.\n");
        return 0;
    }

    if (configuracao->tamanhoMemoria <= 0) {
        printf("Erro: o tamanho da memoria precisa ser maior que zero.\n");
        return 0;
    }

    if (
        !converterTextoParaInteiro(
            campos[4],
            &configuracao->tamanhoPaginaMoldura
        )
    ) {
        printf("Erro: tamanho da pagina e moldura invalido.\n");
        return 0;
    }

    if (configuracao->tamanhoPaginaMoldura <= 0) {
        printf("Erro: o tamanho da pagina precisa ser maior que zero.\n");
        return 0;
    }

    if (
        configuracao->tamanhoMemoria <
        configuracao->tamanhoPaginaMoldura
    ) {
        printf("Erro: a memoria precisa caber pelo menos uma moldura.\n");
        return 0;
    }

    if (
        !converterTextoParaInteiro(
            campos[5],
            &configuracao->percentualAlocacao
        )
    ) {
        printf("Erro: percentual de alocacao invalido.\n");
        return 0;
    }

    if (
        configuracao->percentualAlocacao <= 0 ||
        configuracao->percentualAlocacao > 100
    ) {
        printf("Erro: o percentual precisa ficar entre 1 e 100.\n");
        return 0;
    }

    return 1;
}

// Le a lista de paginas de um processo
static int lerSequenciaPaginas(
    char texto[],
    int quantidadeEsperada,
    int quantidadePaginasProcesso,
    int numeroLinha,
    int **sequenciaPaginas
) {
    char *parte;
    int *sequencia;
    int quantidadeLida;
    int pagina;

    if (
        texto == NULL ||
        sequenciaPaginas == NULL ||
        quantidadeEsperada <= 0 ||
        quantidadePaginasProcesso <= 0
    ) {
        return 0;
    }

    sequencia = malloc(
        sizeof(int) * quantidadeEsperada
    );

    if (sequencia == NULL) {
        printf(
            "Erro: faltou memoria para ler a linha %d.\n",
            numeroLinha
        );

        return 0;
    }

    quantidadeLida = 0;

    parte = strtok(
        texto,
        " \t"
    );

    while (parte != NULL) {
        if (quantidadeLida >= quantidadeEsperada) {
            printf(
                "Erro na linha %d: a sequencia tem acessos demais.\n",
                numeroLinha
            );

            free(sequencia);
            return 0;
        }

        if (!converterTextoParaInteiro(parte, &pagina)) {
            printf(
                "Erro na linha %d: pagina invalida na sequencia.\n",
                numeroLinha
            );

            free(sequencia);
            return 0;
        }

        if (
            pagina <= 0 ||
            pagina > quantidadePaginasProcesso
        ) {
            printf(
                "Erro na linha %d: pagina %d nao pertence ao processo.\n",
                numeroLinha,
                pagina
            );

            free(sequencia);
            return 0;
        }

        sequencia[quantidadeLida] = pagina;
        quantidadeLida++;

        parte = strtok(
            NULL,
            " \t"
        );
    }

    if (quantidadeLida != quantidadeEsperada) {
        printf(
            "Erro na linha %d: eram esperados %d acessos, mas foram lidos %d.\n",
            numeroLinha,
            quantidadeEsperada,
            quantidadeLida
        );

        free(sequencia);
        return 0;
    }

    *sequenciaPaginas = sequencia;

    return 1;
}

// Le uma linha de processo e cria ele
static Processo *lerProcesso(
    char linha[],
    int numeroLinha,
    ConfiguracaoSistema *configuracao,
    ArvoreRN *processosPorPid
) {
    char *campos[6];

    int quantidadeCampos;
    int momentoCriacao;
    int pid;
    int tempoExecucao;
    int prioridadeOuBilhetes;
    int quantidadeMemoria;
    int quantidadePaginasProcesso;

    int *sequenciaPaginas;
    Processo *novoProcesso;

    quantidadeCampos = separarCampos(
        linha,
        campos,
        6
    );

    if (quantidadeCampos != 6) {
        printf(
            "Erro na linha %d: a linha do processo precisa ter 6 campos.\n",
            numeroLinha
        );

        return NULL;
    }

    if (!converterTextoParaInteiro(campos[0], &momentoCriacao)) {
        printf(
            "Erro na linha %d: momento de criacao invalido.\n",
            numeroLinha
        );

        return NULL;
    }

    if (momentoCriacao < 0) {
        printf(
            "Erro na linha %d: momento de criacao nao pode ser negativo.\n",
            numeroLinha
        );

        return NULL;
    }

    if (!converterTextoParaInteiro(campos[1], &pid)) {
        printf(
            "Erro na linha %d: PID invalido.\n",
            numeroLinha
        );

        return NULL;
    }

    if (pid < 0) {
        printf(
            "Erro na linha %d: PID nao pode ser negativo.\n",
            numeroLinha
        );

        return NULL;
    }

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

        return NULL;
    }

    if (!converterTextoParaInteiro(campos[2], &tempoExecucao)) {
        printf(
            "Erro na linha %d: tempo de execucao invalido.\n",
            numeroLinha
        );

        return NULL;
    }

    if (tempoExecucao <= 0) {
        printf(
            "Erro na linha %d: tempo de execucao precisa ser maior que zero.\n",
            numeroLinha
        );

        return NULL;
    }

    if (!converterTextoParaInteiro(campos[3], &prioridadeOuBilhetes)) {
        printf(
            "Erro na linha %d: prioridade ou bilhetes invalido.\n",
            numeroLinha
        );

        return NULL;
    }

    if (prioridadeOuBilhetes <= 0) {
        printf(
            "Aviso na linha %d: prioridade ou bilhetes invalido. Sera usado 1.\n",
            numeroLinha
        );

        prioridadeOuBilhetes = 1;
    }

    if (!converterTextoParaInteiro(campos[4], &quantidadeMemoria)) {
        printf(
            "Erro na linha %d: quantidade de memoria invalida.\n",
            numeroLinha
        );

        return NULL;
    }

    if (quantidadeMemoria <= 0) {
        printf(
            "Erro na linha %d: quantidade de memoria precisa ser maior que zero.\n",
            numeroLinha
        );

        return NULL;
    }

    quantidadePaginasProcesso =
        quantidadeMemoria /
        configuracao->tamanhoPaginaMoldura;

    if (
        quantidadeMemoria %
        configuracao->tamanhoPaginaMoldura != 0
    ) {
        quantidadePaginasProcesso++;
    }

    sequenciaPaginas = NULL;

    if (
        !lerSequenciaPaginas(
            campos[5],
            tempoExecucao,
            quantidadePaginasProcesso,
            numeroLinha,
            &sequenciaPaginas
        )
    ) {
        return NULL;
    }

    novoProcesso = criarProcesso(
        momentoCriacao,
        pid,
        tempoExecucao,
        prioridadeOuBilhetes,
        quantidadeMemoria,
        sequenciaPaginas,
        tempoExecucao
    );

    free(sequenciaPaginas);

    if (novoProcesso == NULL) {
        printf(
            "Erro: nao foi possivel criar o processo da linha %d.\n",
            numeroLinha
        );

        return NULL;
    }

    return novoProcesso;
}

// Le a configuracao e todos os processos do arquivo
int carregarArquivo(
    const char nomeArquivo[],
    ConfiguracaoSistema *configuracao,
    ArvoreRN *processosPorPid,
    ArvoreRN *processosFuturos,
    int *quantidadeProcessos
) {
    FILE *arquivo;
    char linha[TAM_LINHA_ENTRADA];

    int resultadoLeitura;
    int numeroLinha;
    int inseriuPorPid;
    int inseriuNosFuturos;

    Processo *novoProcesso;

    if (quantidadeProcessos != NULL) {
        *quantidadeProcessos = 0;
    }

    if (
        nomeArquivo == NULL ||
        configuracao == NULL ||
        processosPorPid == NULL ||
        processosFuturos == NULL ||
        quantidadeProcessos == NULL
    ) {
        printf("Erro: faltaram dados para carregar o arquivo.\n");
        return 0;
    }

    if (
        processosPorPid->nulo == NULL ||
        processosFuturos->nulo == NULL
    ) {
        printf("Erro: as arvores nao foram inicializadas.\n");
        return 0;
    }

    if (
        processosPorPid->tipoOrdenacao != ORDENAR_POR_PID ||
        processosFuturos->tipoOrdenacao != ORDENAR_POR_CRIACAO
    ) {
        printf("Erro: as arvores foram criadas com a ordem errada.\n");
        return 0;
    }

    configuracao->algoritmoEscalonamento[0] = '\0';
    configuracao->fatiaCPU = 0;
    configuracao->politicaMemoria = POLITICA_INVALIDA;
    configuracao->tamanhoMemoria = 0;
    configuracao->tamanhoPaginaMoldura = 0;
    configuracao->percentualAlocacao = 0;

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

    resultadoLeitura = lerLinhaDoArquivo(
        arquivo,
        linha,
        TAM_LINHA_ENTRADA
    );

    if (resultadoLeitura == 0) {
        printf("Erro: o arquivo esta vazio.\n");
        fclose(arquivo);
        return 0;
    }

    if (resultadoLeitura == -1) {
        printf("Erro: a primeira linha ficou grande demais.\n");
        fclose(arquivo);
        return 0;
    }

    if (!lerConfiguracao(linha, configuracao)) {
        fclose(arquivo);
        return 0;
    }

    while (1) {
        resultadoLeitura = lerLinhaDoArquivo(
            arquivo,
            linha,
            TAM_LINHA_ENTRADA
        );

        if (resultadoLeitura == 0) {
            break;
        }

        numeroLinha++;

        if (resultadoLeitura == -1) {
            printf(
                "Erro: a linha %d ficou grande demais.\n",
                numeroLinha
            );

            fclose(arquivo);

            limparProcessosCarregados(
                processosPorPid,
                processosFuturos,
                quantidadeProcessos
            );

            return 0;
        }

        if (linhaEstaVazia(linha)) {
            continue;
        }

        novoProcesso = lerProcesso(
            linha,
            numeroLinha,
            configuracao,
            processosPorPid
        );

        if (novoProcesso == NULL) {
            fclose(arquivo);

            limparProcessosCarregados(
                processosPorPid,
                processosFuturos,
                quantidadeProcessos
            );

            return 0;
        }

        inseriuPorPid = inserirProcessoNaArvore(
            processosPorPid,
            novoProcesso
        );

        if (!inseriuPorPid) {
            printf(
                "Erro ao guardar o processo da linha %d por PID.\n",
                numeroLinha
            );

            destruirProcesso(novoProcesso);
            fclose(arquivo);

            limparProcessosCarregados(
                processosPorPid,
                processosFuturos,
                quantidadeProcessos
            );

            return 0;
        }

        inseriuNosFuturos = inserirProcessoNaArvore(
            processosFuturos,
            novoProcesso
        );

        if (!inseriuNosFuturos) {
            printf(
                "Erro ao guardar o processo da linha %d nos futuros.\n",
                numeroLinha
            );

            removerProcessoDaArvore(
                processosPorPid,
                novoProcesso
            );

            destruirProcesso(novoProcesso);
            fclose(arquivo);

            limparProcessosCarregados(
                processosPorPid,
                processosFuturos,
                quantidadeProcessos
            );

            return 0;
        }

        (*quantidadeProcessos)++;
    }

    if (ferror(arquivo)) {
        printf("Erro durante a leitura do arquivo.\n");

        fclose(arquivo);

        limparProcessosCarregados(
            processosPorPid,
            processosFuturos,
            quantidadeProcessos
        );

        return 0;
    }

    fclose(arquivo);

    if (*quantidadeProcessos <= 0) {
        printf("Erro: o arquivo nao tem nenhum processo.\n");

        limparProcessosCarregados(
            processosPorPid,
            processosFuturos,
            quantidadeProcessos
        );

        return 0;
    }

    return 1;
}