# Simulador de Escalonamento de Processos

Este trabalho foi desenvolvido para a disciplina de Sistemas Operacionais.

A ideia do programa é simular o gerenciamento de processos de um pequeno sistema operacional. O programa lê um arquivo `.txt` com os dados dos processos e executa o algoritmo de escalonamento informado na primeira linha do arquivo.

Durante a execução, ele mostra qual processo está usando a CPU, quanto tempo ainda falta para ele terminar e, no final, mostra alguns tempos importantes de cada processo.

## Integrantes

- Antonio Olle Ramos
- Fernando da Cruz de Mello
- Guilherme Pereira do Amarilho
- Yuri Oliveira Serra

## Algoritmos feitos

Neste trabalho foram implementados quatro algoritmos:

- Round Robin
- Prioridade
- Loteria
- CFS

O Round Robin executa os processos em ordem circular, usando a fatia de CPU informada no arquivo.

O algoritmo por prioridade escolhe primeiro o processo com maior prioridade. No nosso código, quanto menor o número, maior é a prioridade. Então, por exemplo, prioridade `1` vem antes de prioridade `3`.

A loteria escolhe o processo por sorteio. Cada processo possui uma quantidade de bilhetes, e quanto mais bilhetes ele tem, maior é a chance de ser escolhido. Como tem sorteio, o resultado pode mudar de uma execução para outra.

O CFS usa o valor de `vruntime` para escolher o processo. O processo com menor `vruntime` é escolhido. No nosso código, o último campo do arquivo é usado como peso. Quanto maior o peso, mais devagar o `vruntime` cresce.

## Estrutura dos arquivos

O projeto foi dividido em alguns arquivos para deixar o código mais organizado:

```txt
SEP/
│
├── main.c
├── processo.c
├── processo.h
├── escalonador.c
├── escalonador.h
│
└── entradas/
    ├── entrada_cfs.txt
    ├── entrada_cpu_ociosa.txt
    ├── entrada_desempata_prioridade.txt
    ├── entrada_desempate_cfs.txt
    ├── entrada_loteria.txt
    ├── entrada_mista_grande.txt
    ├── entrada_processos_longos.txt
    ├── entrada_rr.txt
    └── entrada_prioridade.txt
```

- O arquivo main.c é o arquivo principal. Ele recebe o nome do arquivo de entrada, chama a leitura dos processos e depois executa o escalonador.
- O arquivo processo.h possui a estrutura Processo e as declarações das funções ligadas aos processos.
- O arquivo processo.c possui funções para ler o arquivo, validar dados, verificar se ainda existem processos e mostrar os resultados finais.
- O arquivo escalonador.h possui as declarações das funções dos algoritmos de escalonamento.
- O arquivo escalonador.c possui a implementação dos algoritmos Round Robin, Prioridade, Loteria e CFS.

## Formato do arquivo de entrada

A primeira linha do arquivo indica o algoritmo e a fatia de CPU:

```txt
algoritmo|fatiaDeCPU
```

Depois, cada linha representa um processo:


```txt
momentoDeCriacao|PID|tempoDeExecucao|prioridadeOuBilhetes
```

Exemplo:

```txt
RR|2
0|1|5|1
0|2|3|1
2|3|4|1
4|4|2|1
```

Neste exemplo, o algoritmo usado é o Round Robin e a fatia de CPU é 2.

O processo de PID 1 é criado no tempo 0, precisa de 5 unidades de tempo para terminar e possui valor 1 no último campo.

## Algoritmos aceitos

O programa aceita os seguintes nomes na primeira linha do arquivo:

- RR
- ALTERNANCIA
- ALTERNANCIA_CIRCULAR
- PRIORIDADE
- LOTERIA
- CFS

## Como compilar

Para compilar o programa, entre na pasta do projeto e use:

```bash
gcc main.c processo.c escalonador.c -o escalonador
```

## Como executar

Para executar, use:

```bash
./escalonador caminho_do_arquivo

exemplo....
./escalonador ./entradas/entrada_rr.txt
```

## Modo silencioso

Também foi criado um modo silencioso. Esse modo não mostra toda a execução passo a passo. Ele mostra apenas o resultado final.

Para usar:

```bash
./escalonador caminho_do_arquivo silencioso

exemplo....
./escalonador ./entradas/entrada_loteria.txt silencioso
```

Esse modo foi feito porque a saída completa pode ficar muito grande dependendo da quantidade de processos.

## Exemplos de execução

- Para testar Round Robin:

```bash
./escalonador ./entradas/entrada_rr.txt
```

- Para testar prioridade:

```bash
./escalonador ./entradas/entrada_prioridade.txt
```

- Para testar loteria:

```bash
./escalonador ./entradas/entrada_loteria.txt
```

- Para testar CFS:

```bash
./escalonador ./entradas/entrada_cfs.txt
```

- Para testar CPU ociosa:

```bash
./escalonador ./entradas/entrada_cpu_ociosa.txt
```

- Para testar desempate na prioridade:

```bash
./escalonador ./entradas/entrada_desempata_prioridade.txt
```

- Para testar desempate no CFS:

```bash
./escalonador ./entradas/entrada_desempate_cfs.txt
```

- Para testar processos mais longos:

```bash
./escalonador ./entradas/entrada_processos_longos.txt
```

- Para testar uma entrada maior:

```bash
./escalonador ./entradas/entrada_mista_grande.txt
```

- Também é possível usar o modo silencioso em qualquer entrada:

```bash
./escalonador ./entradas/entrada_mista_grande.txt silencioso
```

## O que aparece na saída

Durante a execução normal, o programa mostra quando um processo entra na CPU, quanto tempo ele executa, quanto falta para terminar e quando ele sai da CPU.

Exemplo de saída:

```txt
Tempo 0: PID 1 entrou na CPU
Tempo 0 -> 1: PID 1 executando | falta 4
Tempo 1 -> 2: PID 1 executando | falta 3
Tempo 2: PID 1 saiu da CPU
```

Quando um processo termina, aparece:

>>> PID 1 terminou no tempo 14

No final, o programa mostra uma tabela com os resultados.

## Tempos calculados

O programa calcula três tempos principais.

O primeiro é o tempo total. Esse tempo vai desde o momento em que o processo foi criado até o momento em que ele terminou.

> tempo total = tempo de conclusão - momento de criação

O segundo é o tempo pronto. Esse é o tempo que o processo ficou esperando para usar a CPU.

O terceiro é o tempo de resposta. Esse é o tempo entre o momento em que o processo foi criado e a primeira vez que ele entrou na CPU.

> tempo de resposta = primeira execução - momento de criação
