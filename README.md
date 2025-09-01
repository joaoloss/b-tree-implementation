# Implementação da estrutura Árvore B

## Descrição

Este trabalho tem como objetivo implementar a estrutura de dados Árvore B, simulando a indexação de registros em armazenamento secundário por meio de um arquivo binário.

## Problemática

A **indexação** em bancos de dados é fundamental para otimizar buscas e recuperar registros de forma eficiente em armazenamento secundário, cujo acesso é muito mais lento que o da memória principal. Sem índices adequados, consultas podem exigir leituras sequenciais e exaustivas de grandes volumes de dados irrelevantes, aumentando o tempo de resposta.

Os principais desafios da indexação nesse contexto incluem:

- **Tempo de acesso**: minimizar leituras no disco devido à alta latência, mesmo em SSDs.

- **Tamanho dos dados**: lidar com grandes volumes que não cabem na RAM, exigindo algoritmos e estruturas eficientes.

- **Estruturas de dados**: escolher mecanismos adequados (como árvores B) para garantir buscas rápidas, atualização eficiente e reorganização de índices ao longo do tempo.

Assim, a indexação é essencial em sistemas de gerenciamento de bancos de dados e arquivos para manter desempenho e eficiência no acesso a grandes volumes de dados, tornando viável o uso desses em grande escala.

## Árvore B

As árvores B são estruturas de dados **balanceadas**, **autoajustáveis** e **ordenadas**, amplamente utilizadas em sistemas de gerenciamento de bancos de dados e sistemas de arquivos para otimizar a busca, a inserção e a exclusão de dados em grandes volumes. Elas são uma generalização das árvores binárias de busca, adaptadas para permitir que múltiplos elementos (ou chaves) sejam armazenados em cada nó, o que reduz o número de acessos ao disco e melhora o desempenho em sistemas de armazenamento secundário.

### Propriedades

- **Balanceamento:**  
  A **árvore B** é intrinsecamente balanceada, ou seja, todas as folhas estão no mesmo nível, garantindo que o tempo de acesso aos dados seja `O(log n)`, onde n é o número total de elementos. Diferente da degeneração de balanceamento que pode ocorrer em árvores binárias de busca tradicionais, que podem crescer de forma desproporcional, o desempenho em árvores B é garantido, pois a estrutura se mantém sempre balanceada.

- **Nó com múltiplas chaves:**  
  Generalizando as árvores binárias, as árvores B permitem armazenar múltiplas chaves dentro de um único nó. Essa característica reduz o número de acessos ao disco, o que é especialmente importante, já que acessos a disco são mais lentos do que acessos à memória principal.

- **Estrutura de árvore t-ária:**  
  As árvores B possuem uma ordem, definida como `t`, que estabelece os números máximo e mínimo de filhos para cada nó da estrutura. Assim, cada nó pode ter até `t` filhos, no máximo `t-1` e, no mínimo, `⌈t/2⌉ - 1` chaves (exceto a raiz, que pode ter até `t` filhos, no máximo `t-1` chaves e no mínimo 1 chave).  

  A árvore garante o balanceamento durante as operações de inserção e remoção de dados por meio de redistribuições e concatenações de registros, além da criação de novos nós elegíveis a folhas, internos ou nova raiz. Os nós internos contêm chaves que atuam como delimitadores, dividindo o intervalo de chaves armazenadas nos seus filhos.

### Operações implementadas

- **Busca:**  
  Realizada de forma eficiente, a busca explora as chaves dentro de cada nó para decidir em qual subárvore continuar. Percorre-se a árvore de forma similar a uma árvore binária, mas como cada nó pode conter múltiplas chaves, a profundidade da árvore (e, consequentemente, os acessos) é reduzida, diminuindo também o número de comparações.

- **Inserção:**  
  A inserção inicia-se pela raiz e desce pela árvore até encontrar um nó folha onde a chave pode ser inserida.  
  - Se o nó folha tiver menos que `t-1` chaves, a chave é inserida diretamente.  
  - Caso contrário, realiza-se uma **divisão do nó**, onde a chave do meio é promovida para o nó pai, e o nó é dividido em dois.  
  - Essa divisão pode se propagar para os nós pais se eles também estiverem cheios.  
  - Todas as inserções acontecem em nós folha.

- **Remoção:**  
  A remoção é realizada de forma semelhante à inserção, mas pode envolver **fusões de nós** ou **redistribuição de chaves** entre nós irmãos para manter a árvore balanceada.  
  - Se a chave a ser removida estiver em um nó folha e o nó contiver chaves suficientes (pelo menos `⌈t/2⌉ − 1`, exceto a raiz), basta removê-la diretamente.  
  - Caso contrário, o nó pode:  
    - Fundir-se com um nó irmão;  
    - Pegar uma chave emprestada de um nó irmão;  
    - Pegar uma chave de um dos filhos.  
  Isso garante que as propriedades da árvore B sejam mantidas após a remoção.

## Entrada

Todas as informações devem ser dadas em um arquivo texto:  

1. A ordem da árvore B.  
2. O número de operações na estrutura, que incluem:  
   - Inserções (**I**) e as chaves/dados a serem inseridos;  
   - Remoções (**R**) e as chaves dos dados a serem removidos;  
   - Buscas (**B**) e as chaves buscadas na árvore resultante.
3. As operações.

Exemplo:
```
4
15
I 20, 20
I 75, 75
I 77, 77
I 78, 78
I 55, 55
I 62, 62
I 51, 51
I 40, 40
I 60, 60
I 45, 45
R 78
B 15
B 40
B 25
B 78
```

## Saída

A saída do trabalho é salva em um arquivo de texto contendo os resultados das buscas e o estado final da árvore B, impressa em largura (em cada linha são impressos os nós e chaves referentes aquele nível).

Exemplo:

```
O REGISTRO NAO ESTA NA ARVORE!
O REGISTRO ESTA NA ARVORE!
O REGISTRO ESTA NA ARVORE!
O REGISTRO NAO ESTA NA ARVORE!
-- ARVORE B
[key: 51, key: 75, ]
[key: 20, key: 40, key: 45, ][key: 55, key: 60, key: 62, ][key: 77, ]
```

## Execução

Para compilar:

```bash
make
```

O comando irá gerar o executável `prog`.

Em seguidam, execute:

```bash
./prog <nome_arquivo_entrada> <nome_arquivo_saida>
```

## Conceitos abrangidos
- Modularização com TADs opacos
- Implementação de fila com lista encadeada
- Implementação da estrutura Árvore B com suas principais operações
- Alocação dinâmica de memória
- Makefile
