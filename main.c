/**
 * @file    main.c
 * @brief   Arquivo cliente responsável por manipular a árvore B a partir de comandos vindos de um arquivo de entrada.
 * Além disso, as saídas geradas são registradas em um arquivo de saída.
 * @author  Daniel Corona de Aguiar (daniel.aguiar@edu.ufes.br/2023101578)
 * @author  João Pedro Pereira Loss (joao.loss@edu.ufes.br/2023102068)
 * @author  Raphael Correia Dornelas (raphael.dornelas@edu.ufes.br/2023100595)
 */

#include <stdio.h>
#include <stdlib.h>

#include "arvoreB.h"

#define MSG_REGISTRO_ENCONTRADO "O REGISTRO ESTA NA ARVORE!\n"
#define MSG_REGISTRO_NAO_ENCONTRADO "O REGISTRO NAO ESTA NA ARVORE!\n"

int main(int argc, char const *argv[]) {
    if(argc != 3) {
        printf("Chamada incorreta.\n");
        printf("Formato esperado: <nome_executavel> <nome_arquivo_entrada> <nome_arquivo_saida>\n");
        return 1;
    }

    // --- ABERTURA DE ARQUIVOS
    FILE* arqEntrada = fopen(argv[1], "r");
    if(arqEntrada == NULL) {
        printf("Falha na abertura do arquivo de entrada '%s'.\n", argv[1]);
        return 1;
    }

    FILE* arqSaida = fopen(argv[2], "w");
    if(arqSaida == NULL) {
        printf("Falha na abertura do arquivo de saída '%s'.\n", argv[2]);
        fclose(arqEntrada);
        return 1;
    }
    // ---

    int ordemArvB = 0, numOperacoes = 0;
    fscanf(arqEntrada, "%d%d", &ordemArvB, &numOperacoes);
    fscanf(arqEntrada, "%*[^\n]"); fscanf(arqEntrada, "%*c");

    if(ordemArvB < 3) ordemArvB = 3;

    ArvB* arvB = criaArvB(ordemArvB);

    char operacao = 0;
    int chave = 0, registro = 0, flagBusca = 0;
    for(int i = 0; i < numOperacoes; i++) {
        fscanf(arqEntrada, "%c", &operacao);

        switch (operacao) {
        case 'I':
            fscanf(arqEntrada, "%d, %d", &chave,  &registro);
            insereChaveValor(arvB, chave, registro);
            break;
        
        case 'R':
            fscanf(arqEntrada, "%d", &chave);
            removeChaveValor(arvB, chave);
            break;
        
        case 'B':
            flagBusca = 1;
            fscanf(arqEntrada, "%d", &chave);
            int registro = 0;
            if(buscaChave(arvB, chave, &registro)) {
                fprintf(arqSaida, MSG_REGISTRO_ENCONTRADO);
            } else {
                fprintf(arqSaida, MSG_REGISTRO_NAO_ENCONTRADO);
            }
            break;
        
        default:
            break;
        }

        fscanf(arqEntrada, "%*[^\n]"); fscanf(arqEntrada, "%*c");
    }

    if(flagBusca) fprintf(arqSaida, "\n");
    imprimeArvB(arvB, arqSaida);

    // --- LIBERAÇÃO DE MEMÓRIA
    liberaArvB(arvB);   
    fclose(arqEntrada);
    fclose(arqSaida);
    // ---

    return 0;
}
