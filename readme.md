# Simulador de Escalonamento de Processos

Este projeto foi desenvolvido para a disciplina de Sistemas Operacionais.

O objetivo do programa é simular o módulo de gerenciamento de processos de um pequeno sistema operacional. O sistema lê um arquivo de entrada, cria os processos e executa o algoritmo de escalonamento informado na primeira linha do arquivo.

Durante a execução, o programa mostra:

* o processo que está utilizando a CPU;
* o momento atual da simulação;
* quanto tempo falta para o processo terminar;
* quando um processo entra e sai da CPU;
* quando um processo termina sua execução.

Ao final, o programa apresenta os tempos calculados para cada processo e também as médias da execução.

## Algoritmos implementados

O programa possui os seguintes algoritmos de escalonamento:

* Alternância Circular / Round Robin;
* Prioridade;
* Loteria;
* CFS, de forma simplificada.

Todos os algoritmos utilizam uma fatia de CPU informada no arquivo de entrada.

Um processo permanece na CPU até:

* terminar sua fatia de CPU; ou
* concluir sua execução.

Nesta versão do projeto ainda não existem operações de entrada e saída.

## Estruturas de dados utilizadas

A primeira versão do projeto utilizava um vetor com tamanho fixo para guardar os processos.

Essa estrutura foi substituída por árvores rubro-negras, pois percorrer um vetor inteiro várias vezes poderia causar um gargalo caso existisse uma quantidade muito grande de processos.

A árvore rubro-negra mantém suas operações principais com custo aproximado de `O(log n)`, como:

* inserção;
* remoção;
* busca;
* obtenção do menor processo.

Os processos são criados dinamicamente com `malloc`, portanto o programa não possui mais o antigo limite fixo de 100 processos.

O mesmo processo pode aparecer em mais de uma árvore, mas todas as árvores guardam apenas ponteiros para a mesma estrutura `Processo`.

## Árvores utilizadas

O programa utiliza árvores rubro-negras com diferentes formas de ordenação.

### Árvore por PID

Mantém todos os processos organizados pelo PID.

Ela é utilizada para:

* verificar se existe PID repetido;
* manter uma referência de todos os processos;
* mostrar o resultado final em ordem de PID;
* liberar os processos no final do programa.

### Árvore de processos futuros

Mantém os processos ordenados por:

1. momento de criação;
2. PID.

O menor elemento da árvore representa o próximo processo que deve chegar ao sistema.

Quando o momento de criação do processo é atingido, ele é removido da árvore de futuros e colocado entre os processos prontos.

### Árvore de processos prontos

Mantém apenas os processos que já chegaram ao sistema e estão esperando a CPU.

A ordenação dessa árvore depende do algoritmo utilizado:

* Prioridade: organizada pela prioridade;
* CFS: organizada pelo `vruntime`;
* Loteria: organizada pelo PID e com a soma dos bilhetes nas subárvores;
* Round Robin: organizada pelo PID, com uma fila auxiliar para controlar a ordem circular.

## Alternância Circular / Round Robin

O Round Robin executa os processos em ordem circular.

Quando um processo termina sua fatia de CPU e ainda possui tempo restante, ele volta para o final da fila.

O programa utiliza:

* uma árvore rubro-negra para manter os processos prontos;
* uma fila encadeada para manter a ordem correta do Round Robin.

Quando um processo é escolhido, ele é retirado da fila e da árvore de processos prontos.

Caso ainda não tenha terminado depois da fatia, ele é inserido novamente nas duas estruturas.

## Prioridade

O algoritmo por prioridade escolhe o processo pronto com maior prioridade.

Neste projeto, quanto menor o número informado, maior é a prioridade.

Por exemplo:

```txt
prioridade 1 = maior prioridade
prioridade 2 = prioridade menor
prioridade 3 = prioridade ainda menor
```

A árvore de processos prontos é organizada pelos seguintes critérios:

1. prioridade;
2. momento de criação;
3. PID.

Dessa forma, o processo escolhido é o menor elemento da árvore.

Um processo novo não interrompe outro no meio da fatia. A troca acontece somente quando a fatia atual termina, conforme a especificação desta etapa do trabalho.

## Loteria

O algoritmo de loteria escolhe o próximo processo por meio de um sorteio.

Cada processo possui uma quantidade de bilhetes. Quanto maior a quantidade de bilhetes, maior é sua chance de ser escolhido.

Cada nó da árvore guarda a soma dos bilhetes presentes em sua subárvore.

Isso permite localizar o processo vencedor sem precisar percorrer todos os processos prontos.

Como o algoritmo utiliza sorteio, os resultados podem mudar entre diferentes execuções do programa.

A soma total dos tempos de execução não muda, mas a ordem, o tempo de resposta, o tempo pronto e o momento de conclusão de cada processo podem mudar.

## CFS

O CFS foi implementado de forma simplificada.

Cada processo possui um valor chamado `vruntime`.

O processo pronto com menor `vruntime` é escolhido para executar.

O último campo do arquivo de entrada funciona como peso. O `vruntime` é atualizado da seguinte forma:

```txt
vruntime = vruntime + 1 / peso
```

Quanto maior o peso, mais lentamente o `vruntime` aumenta.

Depois de executar sua fatia, o processo é inserido novamente na árvore, pois seu `vruntime` foi alterado e sua posição na árvore também pode mudar.

Nesta implementação, um processo novo começa com:

```txt
vruntime = 0
```

Por isso, um processo que chega depois pode receber vantagem em relação aos processos que já estavam executando.

Esse comportamento faz parte da implementação simplificada do trabalho e não representa todos os detalhes do CFS utilizado pelo Linux.

## Estados dos processos

Cada processo pode estar em um dos seguintes estados:

```txt
PROCESSO_FUTURO
PROCESSO_PRONTO
PROCESSO_EXECUTANDO
PROCESSO_FINALIZADO
```

### PROCESSO_FUTURO

O processo foi lido do arquivo, mas seu momento de criação ainda não chegou.

### PROCESSO_PRONTO

O processo já chegou ao sistema e está esperando para utilizar a CPU.

### PROCESSO_EXECUTANDO

O processo está utilizando a CPU.

### PROCESSO_FINALIZADO

O processo terminou toda a sua execução.

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
├── arvore_rubro_negra.c
├── arvore_rubro_negra.h
├── readme.md
├── uso_de_IA.md
└── entradas/
    └── arquivos de entrada fornecidos pelo professor
```

## Descrição dos arquivos

### `main.c`

Arquivo principal do programa.

Ele é responsável por:

* verificar os argumentos informados;
* inicializar as árvores principais;
* chamar a leitura do arquivo;
* iniciar o escalonador;
* liberar a memória no final.

### `processo.h`

Contém:

* a estrutura `Processo`;
* os possíveis estados de um processo;
* as declarações das funções relacionadas aos processos;
* as declarações da leitura do arquivo;
* as declarações da exibição dos resultados.

### `processo.c`

Contém:

* criação dinâmica de processos;
* liberação de processos;
* validação dos campos do arquivo;
* leitura do arquivo de entrada;
* mudança dos estados dos processos;
* cálculo e exibição dos resultados finais.

### `escalonador.h`

Contém:

* a estrutura principal do escalonador;
* a estrutura da fila do Round Robin;
* as declarações das funções dos algoritmos;
* as declarações das funções de execução da simulação.

### `escalonador.c`

Contém:

* implementação da fila do Round Robin;
* movimentação dos processos futuros para prontos;
* escolha do próximo processo;
* execução das fatias de CPU;
* atualização do `vruntime`;
* sorteio da loteria;
* controle do relógio da simulação;
* finalização dos processos.

### `arvore_rubro_negra.h`

Contém:

* as estruturas da árvore rubro-negra;
* as cores dos nós;
* os tipos de ordenação;
* as declarações das operações da árvore.

### `arvore_rubro_negra.c`

Contém:

* criação da árvore;
* inserção;
* remoção;
* busca;
* rotações;
* correção das cores;
* busca pelo menor elemento;
* busca por PID;
* controle da soma de bilhetes;
* liberação da árvore.

### `entradas/`

Contém os arquivos de entrada fornecidos pelo professor para testar o programa.

Os arquivos utilizados nos testes não foram criados pelo grupo. O programa deve ser executado utilizando os arquivos disponibilizados pelo professor.

### `uso_de_IA.md`

Contém a descrição do uso de ferramentas de inteligência artificial durante o desenvolvimento do projeto.

## Formato do arquivo de entrada

A primeira linha informa o algoritmo e a fatia de CPU:

```txt
algoritmoDeEscalonamento|fracaoDeCPU
```

As próximas linhas representam os processos:

```txt
momentoDeCriacao|PID|tempoDeExecucao|prioridadeOuBilhetes
```

Cada linha deve possuir exatamente quatro campos.

### Campos da primeira linha

* `algoritmoDeEscalonamento`: algoritmo que será utilizado;
* `fracaoDeCPU`: tempo máximo que um processo pode executar por vez.

### Campos dos processos

* `momentoDeCriacao`: momento em que o processo entra no sistema;
* `PID`: identificador único do processo;
* `tempoDeExecucao`: quantidade total de tempo necessária para finalizar;
* `prioridadeOuBilhetes`: prioridade, bilhetes ou peso, dependendo do algoritmo.

## Algoritmos aceitos

O programa aceita os seguintes nomes:

```txt
RR
ALTERNANCIA
ALTERNANCIA_CIRCULAR
PRIORIDADE
LOTERIA
CFS
```

Os nomes podem ser escritos com letras minúsculas ou maiúsculas, pois o programa converte o nome para letras maiúsculas durante a leitura.

## Validações realizadas

Durante a leitura do arquivo, o programa verifica:

* se o arquivo existe;
* se o arquivo está vazio;
* se o algoritmo é válido;
* se a fatia de CPU é maior que zero;
* se cada linha possui quatro campos;
* se o momento de criação é válido;
* se o PID é válido;
* se existe PID repetido;
* se o tempo de execução é maior que zero;
* se o último campo possui um valor válido.

Caso a prioridade, o peso ou os bilhetes sejam menores ou iguais a zero, o programa utiliza o valor `1`.

## Como compilar

Entre na pasta do projeto e execute:

```bash
gcc -Wall *.c -o escalonador
```

O `*.c` faz com que todos os arquivos com extensão `.c` da pasta sejam compilados, incluindo o arquivo da árvore rubro-negra.

Também é possível compilar informando todos os arquivos:

```bash
gcc -Wall main.c processo.c escalonador.c arvore_rubro_negra.c -o escalonador
```

A opção `-Wall` faz o compilador mostrar avisos sobre possíveis problemas no código.

## Como executar

Para executar o programa, informe o caminho do arquivo fornecido pelo professor:

```bash
./escalonador caminho_do_arquivo
```

Exemplo:

```bash
./escalonador ./entradas/entrada_alternancia
```

O nome exato do arquivo pode mudar conforme os arquivos disponibilizados pelo professor.

## Modo silencioso

No modo silencioso, o programa não mostra a execução passo a passo.

Ele mostra apenas:

* algoritmo utilizado;
* fatia de CPU;
* tabela de resultados;
* médias.

Para executar no modo silencioso:

```bash
./escalonador caminho_do_arquivo silencioso
```

Exemplo:

```bash
./escalonador ./entradas/entrada_loteria silencioso
```

## Comandos utilizados nos testes

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

Esses comandos dependem dos nomes utilizados nos arquivos fornecidos pelo professor.

## Saída durante a execução

Quando um processo entra no estado pronto, o programa mostra:

```txt
Tempo 0: PID 1 ficou pronto
```

Quando um processo entra na CPU:

```txt
Tempo 0: PID 1 entrou na CPU
```

Durante a execução:

```txt
Tempo 0 -> 1: PID 1 executando | falta 4
```

Quando termina sua fatia:

```txt
Tempo 2: PID 1 saiu da CPU
```

Quando conclui toda a execução:

```txt
>>> PID 1 terminou no tempo 14
```

No algoritmo de loteria, também são mostrados:

```txt
Total de bilhetes
Bilhete sorteado
PID escolhido
```

## Resultados finais

Ao final da simulação, o programa mostra uma tabela parecida com:

```txt
PID  Criacao  Execucao  Conclusao  Total  Pronto  Resposta
```

## Tempo total

Representa o tempo entre a criação e a conclusão do processo.

```txt
tempo total = tempo de conclusao - momento de criacao
```

## Tempo pronto

Representa o tempo em que o processo ficou aguardando a CPU.

Como ainda não existem operações de entrada e saída, o tempo pronto pode ser calculado por:

```txt
tempo pronto = tempo de conclusao - momento de criacao - tempo de execucao
```

Esse cálculo evita percorrer todos os processos a cada unidade de tempo.

## Tempo de resposta

Representa o tempo entre a criação do processo e sua primeira entrada na CPU.

```txt
tempo de resposta = primeira execucao - momento de criacao
```

## Médias

Ao final, o programa também calcula:

* tempo médio total;
* tempo médio pronto;
* tempo médio de resposta.

## Uso de memória

Os processos são criados dinamicamente.

A árvore por PID é considerada a estrutura responsável pela memória dos processos.

As outras árvores armazenam ponteiros para os mesmos processos e liberam somente seus próprios nós.

No final do programa:

* a árvore de futuros libera apenas seus nós;
* a árvore de prontos libera apenas seus nós;
* a árvore por PID libera os nós e também os processos.

Isso evita que o mesmo processo seja liberado mais de uma vez.

## Situação atual do projeto

Os quatro algoritmos foram executados utilizando as entradas fornecidas pelo professor:

* Alternância Circular;
* Prioridade;
* Loteria;
* CFS.

As execuções apresentaram resultados coerentes com as regras implementadas.

O algoritmo de loteria pode apresentar resultados diferentes em cada execução devido ao sorteio.

## Limitações atuais

Nesta etapa:

* não existem operações de entrada e saída;
* não existem processos bloqueados;
* um processo só sai da CPU ao terminar a fatia ou sua execução;
* o CFS é uma versão simplificada;
* o simulador utiliza uma única CPU;
* a simulação trabalha com valores inteiros de tempo;
* as entradas utilizadas são as fornecidas pelo professor.

## Possíveis melhorias futuras

O programa pode ser expandido para incluir:

* operações de entrada e saída;
* estado bloqueado;
* diferentes dispositivos de entrada e saída;
* múltiplas CPUs;
* prioridades dinâmicas;
* prevenção de starvation;
* cálculo mais próximo do CFS real;
* novos algoritmos de escalonamento;
* testes automatizados;
* comparação automática entre os resultados dos algoritmos.

## Integrantes

* Antonio Olle Ramos
* Fernando da Cruz de Mello
* Guilherme Pereira do Amarilho
* Yuri Oliveira Serra