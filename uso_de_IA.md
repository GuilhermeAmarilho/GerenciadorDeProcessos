# Uso de IA e Referências

Este documento foi criado para atender à observação do enunciado do trabalho sobre o uso de ferramentas de inteligência artificial.

## Uso de IA

Durante o desenvolvimento do trabalho, foi utilizada a ferramenta ChatGPT como apoio e projetos da web.

A ferramenta foi usada principalmente para ajudar na organização do projeto e explicação de conceitos, além da criação de entradas de teste.

A IA não foi usada apenas para copiar uma solução pronta. O código foi sendo construído, testado, ajustado e explicado durante o desenvolvimento.

## Referências externas

- Escalonamento Round Robin
    - GeeksforGeeks. Round Robin Scheduling in Operating System. Disponível em: https://www.geeksforgeeks.org/operating-systems/round-robin-scheduling-in-operating-system/
    - Essa referência foi usada para revisar a ideia do Round Robin, em que os processos recebem uma fatia fixa de tempo e são executados em ordem circular.
- Escalonamento por Prioridade
    - GeeksforGeeks. Priority Scheduling in Operating System. Disponível em: https://www.geeksforgeeks.org/operating-systems/priority-scheduling-in-operating-system/
    - Essa referência foi usada para revisar a ideia de escolher processos com base em um valor de prioridade.
- Escalonamento por Loteria
    - OSTEP. Scheduling: Proportional Share. Disponível em: https://pages.cs.wisc.edu/~remzi/OSTEP/cpu-sched-lottery.pdf
    - Essa referência foi usada para entender a ideia de escalonamento proporcional, em que os processos recebem uma parte da CPU de acordo com uma distribuição.
    - WALDSPURGER, Carl A.; WEIHL, William E. Lottery Scheduling: Flexible Proportional-Share Resource Management. Disponível em: https://www.usenix.org/publications/library/proceedings/osdi/full_papers/waldspurger.pdf
    - Essa referência foi usada como base teórica sobre o algoritmo de loteria, que usa bilhetes e sorteio para decidir qual processo será executado.
- CFS
    - Linux Kernel Documentation. CFS Scheduler. Disponível em: https://docs.kernel.org/scheduler/sched-design-CFS.html
    - Essa referência foi usada para entender a ideia geral do CFS, especialmente o uso de uma lógica voltada à distribuição justa da CPU.

## Prompts usados

Durante o desenvolvimento do trabalho, foram utilizados prompts com diferentes objetivos, como geração inicial do código, melhoria da lógica, organização dos arquivos, criação de testes e documentação. Abaixo estão alguns exemplos reescritos de forma mais clara e objetiva.

### Primeira versão do código

- "Explique como posso criar uma versão inicial, em linguagem C, de um simulador de escalonamento de processos, considerando leitura de arquivo de entrada e execução dos algoritmos solicitados no enunciado."

### Melhorias na lógica do escalonador

- "Aprimore o algoritmo de loteria para exibir o total de bilhetes, o bilhete sorteado e o processo escolhido durante a execução. Também implementando um modo silencioso, no qual o programa mostre apenas os resultados finais, sem imprimir todo o passo a passo da execução."
- "Melhore os critérios de desempate dos algoritmos de prioridade e CFS, considerando momento de criação e PID."
- "Não implemente envelhecimento no algoritmo de prioridade, mantendo a lógica baseada apenas na prioridade informada no arquivo."

### Organização dos arquivos

- "Me ajude a definir uma lógica correta para reorganizar o projeto em múltiplos arquivos `.c` e `.h`, separando a lógica dos processos da lógica do escalonador."
<!-- Aqui houve varios prompts pois nós não conheciamos arquivos cabeçalhos em C -->
- "Explique a função do arquivo `escalonador.h` dentro do projeto e sua relação com o arquivo `escalonador.c`."
- "Explique de forma simples o papel dos arquivos de cabeçalho `.h` em programas escritos em C."
- "Esclareça por que as funções precisam ser declaradas em arquivos `.h` mesmo estando implementadas em arquivos `.c`."

### Entradas de teste

- "Crie entradas de teste mais robustas, incluindo processos com diferentes tempos de criação, CPU ociosa, desempates e processos longos."

### README e documentação

- "Elabore um README explicando o funcionamento do projeto, como compilar, como executar e como interpretar a saída do programa."
- "Inclua no README a estrutura dos arquivos, o formato da entrada, os algoritmos implementados e exemplos de execução."

### Agrupando os prompts 

- "Vamos compactar todos os meus prompts em um read.me novo, para a entrega final, para isso, preciso que você me envie todos eles revisados na ordem que foram feitos"

---

## 2ª Atividade

Na segunda parte do trabalho, a ferramenta também foi utilizada como apoio para adaptar o projeto de escalonamento já existente para incluir o gerenciamento de memória e os algoritmos de substituição de páginas.

### Planejamento dos algoritmos de memória

- “Me ajude a planejar como integrar os algoritmos FIFO, LRU, NUF e Ótimo ao escalonador já desenvolvido na primeira parte do trabalho.”

### Criação de entradas de teste

- "Crie versões de arquivos de entrada que permitam testar: situações sem troca de páginas; diferenças entre FIFO, LRU, NUF e Ótimo; empate entre os algoritmos; políticas de memória local e global; desempate do NUF pela página de menor ID; percentual de alocação com resultado não inteiro; processos criados em momentos diferentes; funcionamento dos escalonadores por prioridade, loteria e CFS junto com a memória; política de memória inválida; e PIDs repetidos."

### Revisão de erros de compilação

- “Analise as mensagens de erro e avisos apresentados pelo compilador e ajudar a encontrar qual parte do código que precisa ser corrigidas.”

### Revisão de memória dinâmica

- “Verifique onde os vetores e estruturas alocados com malloc precisam ser liberados para evitar vazamentos de memória.”

### Explicação das políticas de memória

- "Explique a diferença entre política local e política global e mostre como cada uma afeta a escolha das páginas que podem ser substituídas."

### Interpretação dos resultados

- "Me ajude a exmplicar e interpretar a linha final produzida pelo programa e como identificar qual algoritmo ficou mais próximo do algoritmo Ótimo."