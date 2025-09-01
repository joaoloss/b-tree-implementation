/**
 * @file    fila.h
 * @brief   Arquivo responsável pela definição da interface com o cliente da fila de inteiros.
 * @author  Daniel Corona de Aguiar (daniel.aguiar@edu.ufes.br/2023101578)
 * @author  João Pedro Pereira Loss (joao.loss@edu.ufes.br/2023102068)
 * @author  Raphael Correia Dornelas (raphael.dornelas@edu.ufes.br/2023100595)
 */

#ifndef FILA_H
#define FILA_H

/// @brief TAD opaco responsável pela definição e manipulação de uma fila de inteiros positivo utilizando lista simplesmente
/// encadeada com sentinela. 
typedef struct _fila Fila;

/// @brief Cria uma fila vazia.
/// @return Ponteiro para a sentinela da fila alocada dinamicamente.
Fila* criaFila();

/// @brief Verifica se a fila está vazia.
/// @param f Ponteiro para a fila
/// @return 1 se a fila estiver vazia e 0, caso contrário.
int filaVazia(Fila* f);

/// @brief Retorna o tamanho da fila.
/// @param f Ponteiro para a fila
/// @return Tamanho da fila.
int getTamFila(Fila* f);

/// @brief Insere um novo número no final da fila. Se o número for negativo nada é feito.
/// @param f Ponteiro para a fila
/// @param v Novo número a ser inserido
void insereFila(Fila* f, int v);

/// @brief Remove o primeiro número da fila.
/// @param f Ponteiro para a fila
/// @return O valor do primeiro número da fila ou -1 se a fila estiver vazia.
int removeFila(Fila* f);

/// @brief Libera toda memória alocada pela fila, incluindo os nós restantes.
/// @param f Ponteiro para a fila a ser liberada
void liberaFila(Fila* f);

#endif