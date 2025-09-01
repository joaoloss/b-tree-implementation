/**
 * @file    fila.c
 * @brief   Arquivo responsável pela implementação de uma fila de inteiros positivos e de suas funções de criação, manipulação e liberação.
 * @author  Daniel Corona de Aguiar (daniel.aguiar@edu.ufes.br/2023101578)
 * @author  João Pedro Pereira Loss (joao.loss@edu.ufes.br/2023102068)
 * @author  Raphael Correia Dornelas (raphael.dornelas@edu.ufes.br/2023100595)
 */

#include <stdio.h>
#include <stdlib.h>

#include "fila.h"

typedef struct _node Node;

struct _node {
    int v;
    Node* prox;
};


struct _fila {
    int tam;
    Node* prim;
    Node* ult;
};

Fila* criaFila() {
    Fila* f = malloc(sizeof(Fila));
    f->prim = f->ult = NULL;
    f->tam = 0;
    return f;
}

int filaVazia(Fila* f) {
    if(f == NULL) return -1;
    return f->tam == 0;
}

int getTamFila(Fila* f) {
    if(f == NULL) return -1;
    return f->tam;
}

void insereFila(Fila* f, int v) {
    if(f == NULL || v < 0) return;

    Node* n = malloc(sizeof(Node));
    n->prox = NULL;
    n->v = v;
    
    if(filaVazia(f)) {
        f->prim = f->ult = n;
    } else {
        f->ult->prox = n;
        f->ult = n;
    }
    f->tam++;
}

int removeFila(Fila* f) {
    if(filaVazia(f) || f == NULL) return -1;

    Node* n = f->prim;
    int v = n->v;
    f->prim = f->prim->prox;    
    free(n);
    f->tam--;

    return v;
}

void liberaFila(Fila* f) {
    if(f == NULL) return;
    
    Node* n = f->prim;
    while (n != NULL) {
        f->prim = f->prim->prox;
        free(n);
        n = f->prim;
    }

    free(f);    
} 

