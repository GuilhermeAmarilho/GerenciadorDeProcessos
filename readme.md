# Simulador de Escalonamento de Processos

Este projeto foi desenvolvido para a disciplina de Sistemas Operacionais.

O objetivo do programa é simular o módulo de gerenciamento de processos de um pequeno sistema operacional. O sistema lê um arquivo de entrada contendo os processos e executa o algoritmo de escalonamento informado na primeira linha do arquivo.

Durante a execução, o programa mostra qual processo está utilizando a CPU em cada momento, quanto tempo ainda falta para ele terminar e, ao final, apresenta uma tabela com os tempos calculados para cada processo.

## Situação do projeto

Este projeto foi iniciado na primeira etapa da disciplina com a implementação de um escalonador preemptivo baseado em fatia de CPU.

Nesta versão, o programa já possui os seguintes algoritmos de escalonamento:

* Alternância Circular / Round Robin
* Prioridade
* Loteria
* CFS

Para a segunda etapa da disciplina, o projeto será reorganizado e adaptado conforme os novos requisitos definidos pelo professor.

## Algoritmos implementados

### Alternância Circular / Round Robin

O algoritmo de Alternância Circular executa os processos em ordem circular, respeitando a fatia de CPU definida no arquivo de entrada.

Quando um processo termina sua fatia de CPU e ainda não foi concluído, ele volta para a fila de processos prontos e aguarda uma nova oportunidade de execução.

### Prioridade

O algoritmo por prioridade escolhe o processo pronto com maior prioridade.

Neste projeto, quanto menor o número informado no campo de prioridade, maior é a prioridade do processo. Por exemplo, um processo com prioridade `1` será escolhido antes de um processo com prioridade `3`.

Em caso de empate, o programa utiliza critérios de desempate para manter a execução previsível.

### Loteria

O algoritmo de loteria escolhe o processo por meio de um sorteio.

Cada processo possui uma quantidade de bilhetes. Quanto maior o número de bilhetes, maior a chance de o processo ser escolhido para executar.

Como existe sorteio, os resultados podem variar entre diferentes execuções do programa.

### CFS

O algoritmo CFS foi implementado de forma simplificada.

O programa utiliza o valor de `vruntime` para decidir qual processo será executado. O processo pronto com menor `vruntime` é escolhido.

No código, o último campo do arquivo é usado como peso. Quanto maior o peso, mais lentamente o `vruntime` do processo cresce.

## Estrutura dos arquivos

O projeto está organizado da seguinte forma:

```txt
GDP/
│
├── main.c
├── processo.c
├── processo.h
├── escalonador.c
├── escalonador.h
├── readme.md
├── uso_de_IA.md
└── entradas/
    ├── entrada_alternancia
    ├── entrada_prioridade
    ├── entrada_loteria
    └── entrada_cfs
```

### Descrição dos arquivos

* `main.c`: arquivo principal do programa. Recebe o caminho do arquivo de entrada, chama a leitura dos processos e inicia a execução do escalonador.
* `processo.h`: contém a estrutura `Processo`, constantes do projeto e declarações das funções relacionadas aos processos.
* `processo.c`: contém funções para leitura do arquivo, validação dos dados, controle dos tempos dos processos e exibição dos resultados finais.
* `escalonador.h`: contém as declarações das funções dos algoritmos de escalonamento.
* `escalonador.c`: contém a implementação dos algoritmos de escalonamento e da simulação principal.
* `entradas/`: pasta que contém os arquivos de entrada utilizados para testar cada algoritmo.
* `uso_de_IA.md`: arquivo com a descrição do uso de ferramentas de inteligência artificial durante o desenvolvimento do trabalho.

## Formato do arquivo de entrada

O arquivo de entrada deve seguir o formato definido pelo professor.

A primeira linha contém o algoritmo de escalonamento e a fatia de CPU:

```txt
algoritmoDeEscalonamento|fracaoDeCPU
```

As linhas seguintes representam os processos:

```txt
momentoDeCriacao|PID|tempoDeExecucao|prioridadeOuBilhetes
```

Onde:

* `algoritmoDeEscalonamento`: algoritmo que será utilizado para escalonar os processos.
* `fracaoDeCPU`: período máximo que um processo pode permanecer na CPU por vez.
* `momentoDeCriacao`: tempo em que o processo é criado.
* `PID`: identificador único do processo.
* `tempoDeExecucao`: quantidade total de tempo necessária para o processo terminar.
* `prioridadeOuBilhetes`: prioridade do processo ou quantidade de bilhetes, dependendo do algoritmo utilizado.

## Algoritmos aceitos

O programa aceita os seguintes nomes na primeira linha do arquivo:

```txt
RR
ALTERNANCIA
ALTERNANCIA_CIRCULAR
PRIORIDADE
LOTERIA
CFS
```

## Exemplos de arquivos de entrada

### Alternância Circular

```txt
ALTERNANCIA|2
0|1|5|1
0|2|3|1
2|3|4|1
4|4|2|1
```

### Prioridade

```txt
PRIORIDADE|2
0|1|5|2
0|2|4|1
2|3|3|3
```

### Loteria

```txt
LOTERIA|2
0|1|5|10
0|2|4|20
2|3|3|5
```

### CFS

```txt
CFS|2
0|1|5|1
0|2|4|2
2|3|3|3
```

## Como compilar

Para compilar o programa, entre na pasta do projeto e execute:

```bash
gcc main.c processo.c escalonador.c -o escalonador
```

Também é possível compilar com avisos adicionais:

```bash
gcc -Wall -Wextra main.c processo.c escalonador.c -o escalonador
```

## Como executar

Para executar o programa, informe o caminho do arquivo de entrada:

```bash
./escalonador caminho_do_arquivo
```

Exemplo:

```bash
./escalonador ./entradas/entrada_alternancia
```

## Modo silencioso

O programa também possui um modo silencioso.

Nesse modo, a execução passo a passo não é exibida. O programa mostra apenas o resultado final.

Para usar o modo silencioso:

```bash
./escalonador caminho_do_arquivo silencioso
```

Exemplo:

```bash
./escalonador ./entradas/entrada_loteria silencioso
```

## Exemplos de execução

Para testar Alternância Circular:

```bash
./escalonador ./entradas/entrada_alternancia
```

Para testar Prioridade:

```bash
./escalonador ./entradas/entrada_prioridade
```

Para testar Loteria:

```bash
./escalonador ./entradas/entrada_loteria
```

Para testar CFS:

```bash
./escalonador ./entradas/entrada_cfs
```

## O que aparece na saída

Durante a execução normal, o programa mostra quando um processo entra na CPU, quanto tempo ele executa, quanto falta para terminar e quando ele sai da CPU.

Exemplo:

```txt
Tempo 0: PID 1 entrou na CPU
Tempo 0 -> 1: PID 1 executando | falta 4
Tempo 1 -> 2: PID 1 executando | falta 3
Tempo 2: PID 1 saiu da CPU
```

Quando um processo termina, o programa mostra uma mensagem semelhante a esta:

```txt
>>> PID 1 terminou no tempo 14
```

Ao final da execução, o programa apresenta uma tabela com os resultados de cada processo.

## Tempos calculados

O programa calcula os seguintes tempos:

### Tempo total

Representa o tempo entre a criação do processo e sua conclusão.

```txt
tempo total = tempo de conclusao - momento de criacao
```

### Tempo pronto

Representa o tempo em que o processo ficou em estado pronto, aguardando para utilizar a CPU.

### Tempo de resposta

Representa o tempo entre a criação do processo e a primeira vez em que ele entrou na CPU.

```txt
tempo de resposta = primeira execucao - momento de criacao
```

## Observações sobre a implementação atual

Nesta etapa, um processo somente sai da CPU quando termina sua fatia de tempo ou quando conclui sua execução.

Em etapas futuras, o simulador poderá ser expandido para tratar novas situações, como operações de entrada e saída, bloqueio de processos e novos estados de processo.

## Alterações previstas para a segunda etapa

Para a segunda etapa do trabalho, o projeto será adaptado conforme os novos requisitos da disciplina.

Entre as possíveis alterações estão:

* reorganização dos arquivos de entrada;
* atualização da documentação;
* inclusão de novos comportamentos de processos;
* adaptação da estrutura `Processo`;
* melhoria do controle de estados dos processos;
* atualização da lógica principal do escalonador.

Esta seção será atualizada conforme os requisitos finais do segundo bimestre forem definidos.

## Integrantes

* Antonio Olle Ramos - Matrícula
* Fernando da Cruz de Mello - Matrícula
* Guilherme Pereira do Amarilho - Matrícula
* Yuri Oliveira Serra - Matrícula
