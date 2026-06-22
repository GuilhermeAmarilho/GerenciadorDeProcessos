# Gerenciador de Processos e Memória

Este trabalho simula o escalonamento de processos e também compara algoritmos de substituição de páginas.

O programa recebe um arquivo de entrada com as configurações do sistema, os processos e a sequência de páginas acessadas por cada processo.

Durante a execução, cada ciclo de CPU faz um acesso à memória.

No final, o programa mostra a quantidade de trocas de páginas dos algoritmos:

* FIFO;
* LRU;
* NUF;
* Ótimo.

Também mostra qual algoritmo ficou mais próximo do resultado do algoritmo Ótimo.

---

## Arquivos do projeto

```text
GerenciadorDeProcessos/
├── entradas/
│   └── entradaMemoria.txt
├── main.c
├── configuracao.h
├── entrada.c
├── entrada.h
├── processo.c
├── processo.h
├── escalonador.c
├── escalonador.h
├── memoria.c
├── memoria.h
├── arvore_rubro_negra.c
├── arvore_rubro_negra.h
├── readme.md
└── uso_de_IA.md
```

---

## Compilação

Entre na pasta do projeto pelo terminal e execute:

```bash
gcc -std=c11 -Wall -Wextra -pedantic *.c -o escalonador
```

Se não aparecer nenhum erro, o programa foi compilado corretamente.

---

## Execução

Para executar mostrando todos os detalhes:

```bash
./escalonador entradas/entradaMemoria.txt
```

Para executar sem mostrar os detalhes de cada ciclo:

```bash
./escalonador entradas/entradaMemoria.txt silencioso
```

Mesmo no modo silencioso, o programa ainda mostra o resultado dos processos e a linha final dos algoritmos de memória.

---

## Formato do arquivo de entrada

A primeira linha possui as configurações do sistema:

```text
algoritmoDeEscalonamento|fraçãoDeCPU|políticaMemória|tamanhoMemória|tamanhoPáginasMolduras|percentualAlocação
```

As próximas linhas possuem os processos:

```text
tempoCriacaoProcesso|PID|tempoDeExecução|prioridadeOuBilhetes|qtdeMemoria|sequênciaAcessoPaginasProcesso
```

---

## Exemplo de entrada

```text
alternancia|10|local|65536|512|50
0|1|20|59|4096|1 2 2 2 3 4 3 4 5 5 6 1 5 3 2 6 7 7 7 8
0|2|24|32|2048|1 2 2 2 3 4 3 4 4 4 2 3 2 1 3 2 1 2 2 3 4 3 2 2
0|3|32|13|4096|1 2 3 4 5 6 7 8 4 3 2 1 1 6 7 5 6 8 3 2 2 1 2 2 4 4 5 3 2 1 7 8
```

---

## Configuração do sistema

### Algoritmo de escalonamento

Os valores aceitos são:

```text
alternancia
rr
alternancia_circular
prioridade
loteria
cfs
```

O programa converte o nome para letras maiúsculas durante a leitura.

### Fração de CPU

Define quantos ciclos um processo pode executar antes de voltar para a fila de processos prontos.

Exemplo:

```text
10
```

O processo poderá executar até 10 ciclos por vez.

### Política de memória

Os valores aceitos são:

```text
local
global
```

Na política local, um processo só pode substituir páginas dele mesmo.

Na política global, um processo pode substituir páginas de outros processos quando a memória estiver cheia.

### Tamanho da memória

Informa o tamanho total da memória principal em bytes.

Exemplo:

```text
65536
```

### Tamanho das páginas e molduras

Informa o tamanho de cada página e de cada moldura em bytes.

Exemplo:

```text
512
```

A quantidade total de molduras é calculada assim:

```text
quantidade de molduras = tamanho da memória / tamanho da moldura
```

Exemplo:

```text
65536 / 512 = 128 molduras
```

### Percentual de alocação

Define o percentual máximo das páginas de um processo que pode permanecer na memória principal.

Exemplo:

```text
50
```

Um processo com 8 páginas poderá ocupar no máximo 4 molduras.

Quando o resultado do percentual não é inteiro, o programa usa a parte inteira.

O programa sempre permite pelo menos uma moldura para cada processo.

---

## Informações dos processos

### Tempo de criação

Indica o momento em que o processo entra no sistema.

### PID

É o identificador do processo.

Não podem existir dois processos com o mesmo PID.

### Tempo de execução

Indica quantos ciclos de CPU o processo precisa executar.

### Prioridade ou bilhetes

Esse campo depende do algoritmo de escalonamento.

No algoritmo de prioridade, o menor valor possui maior prioridade.

No algoritmo de loteria, o valor representa a quantidade de bilhetes.

No CFS, o valor é usado como peso do processo.

### Quantidade de memória

Indica a quantidade de memória virtual do processo em bytes.

A quantidade de páginas é calculada usando:

```text
quantidade de páginas = quantidade de memória / tamanho da página
```

Quando a divisão não é exata, é criada uma página adicional.

### Sequência de páginas

Informa a ordem das páginas acessadas pelo processo.

Exemplo:

```text
1 2 3 2 1
```

Como cada ciclo de CPU faz um acesso à memória, a quantidade de páginas da sequência deve ser igual ao tempo de execução do processo.

As páginas começam no número 1.

---

## Escalonadores implementados

### Alternância circular

Os processos são executados em ordem de fila.

Quando a fatia de CPU termina, o processo volta para o final da fila, caso ainda não tenha terminado.

### Prioridade

O processo com o menor valor de prioridade é escolhido primeiro.

### Loteria

Cada processo recebe uma quantidade de bilhetes.

Um bilhete é sorteado e o processo dono desse bilhete usa a CPU.

### CFS

O processo com menor tempo virtual é escolhido.

O tempo virtual aumenta de acordo com o peso do processo.

---

## Algoritmos de memória

### FIFO

Remove a página que entrou primeiro na memória.

### LRU

Remove a página que ficou mais tempo sem ser acessada.

### NUF

Remove a página que foi acessada menos vezes.

Quando duas páginas possuem a mesma frequência, é removida a página com menor ID.

### Ótimo

Remove a página que será acessada mais longe no futuro.

Quando uma página não será mais acessada, ela é escolhida para sair.

Esse algoritmo é usado como referência para comparar os outros algoritmos.

---

## Contagem de trocas

Uma troca só é contada quando uma página precisa sair da memória para outra entrar.

Exemplo com uma moldura vazia:

```text
[1] [2] [vazia]
```

Ao colocar a página 3:

```text
[1] [2] [3]
```

Não é contada uma troca, porque nenhuma página precisou sair.

Agora, com a memória cheia:

```text
[1] [2] [3]
```

Ao colocar a página 4:

```text
[4] [2] [3]
```

Uma troca é contada, porque a página 1 precisou sair.

As páginas de um processo são liberadas quando ele termina. Essa liberação não conta como troca.

---

## Histórico de acessos

Durante o escalonamento, o programa guarda todos os acessos na ordem em que aconteceram.

Cada acesso possui:

```text
tempo
PID
página
```

Depois que todos os processos terminam, o mesmo histórico é usado para executar FIFO, LRU, NUF e Ótimo.

Isso garante que todos os algoritmos recebam exatamente a mesma sequência de acessos.

---

## Saída do programa

Durante a execução detalhada, o programa mostra:

* Processos entrando no estado pronto;
* Processo entrando na CPU;
* Página acessada;
* Tempo restante;
* Processo saindo da CPU;
* Processo terminando;
* Resultado final dos processos;
* Média dos tempos;
* Quantidade total de acessos;
* Resultado dos algoritmos de memória.

A última linha possui o formato:

```text
FIFO|LRU|NUF|ÓTIMO|MELHOR
```

Exemplo:

```text
38|40|35|27|NUF
```

Esse resultado significa:

```text
FIFO: 38 trocas
LRU: 40 trocas
NUF: 35 trocas
Ótimo: 27 trocas
Algoritmo mais próximo do Ótimo: NUF
```

Quando dois ou mais algoritmos ficam igualmente próximos do Ótimo, o programa imprime:

```text
empate
```

Exemplo:

```text
1|1|1|1|empate
```

---

## Entrada fornecida

O arquivo usado para executar o programa é:

```text
entradas/entradaMemoria.txt
```

Execução normal:

```bash
./escalonador entradas/entradaMemoria.txt
```

Execução silenciosa:

```bash
./escalonador entradas/entradaMemoria.txt silencioso
```

Para a entrada fornecida, a linha final produzida é:

```text
38|40|35|27|NUF
```

---

## Limpeza do executável

Para remover o executável depois do teste:

```bash
rm -f escalonador
```

O executável não precisa ser colocado dentro do arquivo compactado da entrega.

---

## Observações

O programa valida vários problemas no arquivo de entrada, como:

* Política de memória inválida;
* Algoritmo desconhecido;
* PID repetido;
* Campo vazio;
* Valor numérico inválido;
* Página que não pertence ao processo;
* Sequência com acessos faltando;
* Sequência com acessos sobrando;
* Percentual fora do intervalo de 1 a 100;
* Memória ou página com tamanho inválido.

Quando encontra um erro, o programa mostra uma mensagem e encerra a execução.