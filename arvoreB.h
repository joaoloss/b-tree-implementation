/**
 * @file    arvoreB.h
 * @brief   Arquivo responsável pela definição da interface com o cliente da árvore B.
 * @author  Daniel Corona de Aguiar (daniel.aguiar@edu.ufes.br/2023101578)
 * @author  João Pedro Pereira Loss (joao.loss@edu.ufes.br/2023102068)
 * @author  Raphael Correia Dornelas (raphael.dornelas@edu.ufes.br/2023100595)
 */

#ifndef ARVB_H
#define ARVB_H

/// @brief TAD opaco resposável pela definição e manipulação de uma árvore B que armazena seus nós em um arquivo binário
/// cuja criação, manipulação e liberação é totalmente feita internamente pela estrutura. Essa árvore só permite valores
/// inteiros positivos de chave.
typedef struct _arvB ArvB;

/// @brief Cria uma árvore vazia.
/// @param ordem Ordem da árvore
/// @return Ponteiro para a estrutura da árvore alocada dinamicamente.
ArvB* criaArvB(int ordem);

/// @brief Insere um par chave/registro na árvore. Se a chave já estiver presente, o registro é atualizado. Se a chave for negativa nada é feito.
/// @param arv Ponteiro para a árvore B
/// @param chave Chave a ser inserida
/// @param registro Registro correspondente à chave
void insereChaveValor(ArvB* arv, int chave, int registro);

/// @brief Verifica se uma chave está na árvore e, se estiver, atribui o registro ao endereço passado como argumento (caso esse seja diferente de NULL).
/// @param arv Ponteiro para a árvore B
/// @param chave Chave a ser buscada
/// @param registroBuscado Ponteiro para o local onde o registro buscado deve ser armazenado
/// @return 1 se a chave for encontrada e 0, caso contrário.
int buscaChave(ArvB* arv, int chave, int* registroBuscado);

/// @brief Retira par chave/valor da árvore com base na chave fornecida. Se a chave não existir nada é feito.
/// @param arv Ponteiro para a árvore B
/// @param chave Chave a ser removida
void removeChaveValor(ArvB* arv, int chave);

/// @brief Imprime a árvore por níveis de profundidade.
/// @param arv Ponteiro para a árvore B
/// @param saida Referência para o local onde a impressão deve ser realizada
void imprimeArvB(ArvB* arv, FILE* saida);

/// @brief Libera toda a memória utilizada pela árvore, incluindo o arquivo binário utilizado.
/// @param arv Ponteiro para a árvore B
void liberaArvB(ArvB* arv);

#endif