/**
 * @file    arvoreB.c
 * @brief   Arquivo responsável pela implementação da árvore B e de suas funções de criação, acesso, manipulação, impressão e liberação.
 * @author  Daniel Corona de Aguiar (daniel.aguiar@edu.ufes.br/2023101578)
 * @author  João Pedro Pereira Loss (joao.loss@edu.ufes.br/2023102068)
 * @author  Raphael Correia Dornelas (raphael.dornelas@edu.ufes.br/2023100595)
 */

#include <stdio.h>
#include <stdlib.h>

#include "arvoreB.h"
#include "fila.h"

#define NOME_ARQ_BIN "arvB.bin"
#define POSICAO_RAIZ 0
#define TRUE 1
#define FALSE 0

/// @brief Estrutura do nó da árvore B.
typedef struct _node Node;
struct _node {
    int numChavesArmazenadas; 
    // Internos e folhas: mínimo ⌈(t/2)⌉−1 e máximo t−1. Raíz: mínimo 1 e máximo t−1

    char ehFolha; 
    // 1: folha | 0: não folha

    char ehSuperNode; // node com número de chaves igual à ordem
    // 1: super node | 0: nao super node

    char ehMiniNode; // node deficiente com nChaves < minimo
    // 1: mini node | 0: nao mini node

    int posicaoArqBin;
    // Deslocamento em bytes para acessar o nó dentro do arq. bin.

    int* chaves;
    int* registros;
    
    int* filhos; 
    // sempre igual ao número de chaves armazenadas + 1
    // indica o offset (deslocamento) necessário para encontrar os filhos no arq. bin.
};

struct _arvB {
    int ordem;
    int numNos;
    int nodeSizeBytes;
    int offsetAcumulado;
    FILE* arqBin;
};

// --- FUNÇÕES DE INTERFACE
ArvB* criaArvB(int ordem);
void insereChaveValor(ArvB* arv, int chave, int registro);
int buscaChave(ArvB* arv, int chave, int* registroBuscado);
void imprimeArvB(ArvB* arv, FILE* saida);
void removeChaveValor(ArvB* arv, int chave);
void liberaArvB(ArvB* arv);
// ---

// --- FUNÇÕES INTERNAS
static Node* criaNode(int ordem, char ehFolha, int posicaoArqBin);
static void liberaNode(Node* n);

static int arvBVazia(ArvB* arv);
static int cheio(Node* n, int ordem);
static int buscaBinaria(int c, int* chaves, int inicio, int fim);
static Node* leNodeArqBin(int offset, ArvB* arv);
static void escreveNodeArqBin(ArvB* arv, Node* n);
static int buscaChaveNode(ArvB* arv, int posNode, int chave, int* registroBuscado);
static void insereChaveValorRec(ArvB* arv, Node* n, int chave, int registro);
static void splitNodeFilho(ArvB* arv, Node* pai, Node* filho, int idxFilho);
static int minChaves(int ordem);
static void redistribuiDaEsquerda(ArvB* arv, Node* pai, int idxFilho, Node* filho, Node* irmaoEsq);
static void redistribuiDaDireita(ArvB* arv, Node* pai, int idxFilho, Node* filho, Node* irmaoDir);
static void concatenaComIrmaoEsquerdo(ArvB* arv, Node* pai, int idxFilho, Node* filho, Node* irmaoEsq);
static void removeFolha(ArvB *arv, Node *n, int idxChave);
static void rebalanceia(ArvB* arv, Node* pai, Node* filho, int idxFilho);
static void removeChaveValorRec(ArvB* arv, Node* n, int chave);
static int trocaChaveComPredecessor(ArvB* arv, Node* n, Node* filho, int idxChave);
// ---

// --- IMPLEMENTAÇÕES
ArvB* criaArvB(int ordem) {
    ArvB* arv = malloc(sizeof(ArvB));
    arv->ordem = ordem;
    arv->numNos = 0;
    arv->offsetAcumulado = 0;
    arv->nodeSizeBytes = sizeof(int)*2 + sizeof(char) + sizeof(int)*ordem*3;
    arv->arqBin = NULL;

    return arv;
}

void imprimeArvB(ArvB* arv, FILE* saida) {
    if(arv == NULL || arvBVazia(arv)) return;
    
    fprintf(saida, "-- ARVORE B\n");
    
    Fila* fila = criaFila();
    insereFila(fila, POSICAO_RAIZ);
    Node* nAtual = NULL;
    int numNodesNivelAtual = 0;
    while(!filaVazia(fila)) {
        numNodesNivelAtual = getTamFila(fila);
        for(int i = 0; i < numNodesNivelAtual; i++) {
            int posNodeAtual = removeFila(fila);
            nAtual = leNodeArqBin(posNodeAtual, arv);
    
            fprintf(saida, "[");
            for(int c = 0; c < nAtual->numChavesArmazenadas; c++) {
                fprintf(saida, "key: %d(%d), ", nAtual->chaves[c], nAtual->registros[c]);
            }
            fprintf(saida, "] ");
    
            if(!nAtual->ehFolha) {
                for(int i = 0; i < nAtual->numChavesArmazenadas+1; i++) {
                    insereFila(fila, nAtual->filhos[i]);
                }
            }
    
            liberaNode(nAtual);
        }

        fprintf(saida, "\n");
    }

    liberaFila(fila);
}

void liberaArvB(ArvB* arv) {
    if(arv == NULL) return;
    if(arv->arqBin) fclose(arv->arqBin);
    if(arv) free(arv);
    remove(NOME_ARQ_BIN);
}

void insereChaveValor(ArvB* arv, int chave, int registro) {
    if(arv == NULL || chave < 0) return;
    
    if(arv->arqBin == NULL) { // arquivo binário ainda não existe
        arv->arqBin = fopen(NOME_ARQ_BIN, "wb+");
    }

    Node* raiz = NULL;
    if(arvBVazia(arv)) {
        raiz = criaNode(arv->ordem, TRUE, 0);
        arv->numNos++;
        arv->offsetAcumulado++;
    } else { // se a raíz já existir ela é recuperada do arq. bin.
        raiz = leNodeArqBin(POSICAO_RAIZ, arv);
    }

    insereChaveValorRec(arv, raiz, chave, registro);
    if(raiz->ehSuperNode) { // se a raíz virou super node, ela é splitada e uma nova raíz é criada
        Node* novaRaiz = criaNode(arv->ordem, FALSE, POSICAO_RAIZ);
        novaRaiz->filhos[0] = raiz->posicaoArqBin;
        raiz->posicaoArqBin = arv->offsetAcumulado; // antiga raíz vai para a o final do arq. bin.
        arv->numNos++;
        arv->offsetAcumulado++;
        
        splitNodeFilho(arv, novaRaiz, raiz, 0);
        liberaNode(novaRaiz);
    }
    liberaNode(raiz);
}

int buscaChave(ArvB* arv, int chave, int* registroBuscado) {
    if(arv == NULL || arvBVazia(arv) || chave < 0) return 0;

    Node* raiz = leNodeArqBin(POSICAO_RAIZ, arv);
    int chaveEncontrada = 0;
    int idx = buscaBinaria(chave, raiz->chaves, 0, raiz->numChavesArmazenadas-1);
    if(idx < raiz->numChavesArmazenadas && raiz->chaves[idx] == chave) {
        if(registroBuscado != NULL) *registroBuscado = raiz->registros[idx];
        chaveEncontrada = 1;
    } else if(!raiz->ehFolha) {
        chaveEncontrada = buscaChaveNode(arv, raiz->filhos[idx], chave, registroBuscado);
    }

    liberaNode(raiz);
    return chaveEncontrada;
}

void removeChaveValor(ArvB* arv, int chave) {
    if (arv == NULL || arvBVazia(arv)) return;

    Node* raiz = leNodeArqBin(POSICAO_RAIZ, arv);
    removeChaveValorRec(arv, raiz, chave);
    liberaNode(raiz);
}

static Node* criaNode(int ordem, char ehFolha, int posicaoArqBin) {
    Node* novoNode = malloc(sizeof(Node));

    novoNode->ehFolha = ehFolha;
    novoNode->ehSuperNode = FALSE;
    novoNode->ehMiniNode = FALSE;
    novoNode->numChavesArmazenadas = 0;
    novoNode->posicaoArqBin = posicaoArqBin;

    novoNode->chaves = calloc(ordem, sizeof(int));
    novoNode->registros = calloc(ordem, sizeof(int));
    novoNode->filhos = calloc(ordem + 1, sizeof(int));

    return novoNode;
}

static void liberaNode(Node* n) {
    if(n == NULL) return;

    if(n->chaves) free(n->chaves);
    if(n->registros) free(n->registros);
    if(n->filhos) free(n->filhos);

    free(n);
}

static int arvBVazia(ArvB* arv) {
    return arv->numNos == 0;
}

static int cheio(Node* n, int ordem) {
    return n->numChavesArmazenadas == (ordem-1);
}

// Retorna o índice de 'c' em 'chaves' ou o índice da chave imediatamente superior (se a chave for maior que 
// a maior chave do vetor, é retornado uma posição fora do vetor correspondente ao tamanho desse).
static int buscaBinaria(int c, int* chaves, int inicio, int fim) {
    if(inicio > fim) return inicio;

    int idxMed = inicio + (fim-inicio)/2;
    if(c < chaves[idxMed]) return buscaBinaria(c, chaves, inicio, idxMed-1);
    else if(c > chaves[idxMed]) return buscaBinaria(c, chaves, idxMed+1, fim);
    else return idxMed;
}

static Node* leNodeArqBin(int offset, ArvB* arv) {
    FILE* arqBin = arv->arqBin;
    int ordem = arv->ordem;
    fseek(arqBin, offset*arv->nodeSizeBytes, SEEK_SET);
    
    int numChaves, posicaoBin;
    char ehFolha;
    fread(&numChaves, sizeof(int), 1, arqBin);
    fread(&ehFolha, sizeof(char), 1, arqBin);
    fread(&posicaoBin, sizeof(int), 1, arqBin);

    Node* n = criaNode(ordem, ehFolha, posicaoBin);
    n->numChavesArmazenadas = numChaves;
    fread(n->chaves, sizeof(int), ordem-1, arqBin);
    fread(n->registros, sizeof(int), ordem-1, arqBin);
    fread(n->filhos, sizeof(int), ordem, arqBin);

    return n;
}

static void escreveNodeArqBin(ArvB* arv, Node* n) {
    FILE* arqBin = arv->arqBin;
    int ordem = arv->ordem;
    fseek(arqBin, n->posicaoArqBin*arv->nodeSizeBytes, SEEK_SET);

    fwrite(&n->numChavesArmazenadas, sizeof(int), 1, arqBin);
    fwrite(&n->ehFolha, sizeof(char), 1, arqBin);
    fwrite(&n->posicaoArqBin, sizeof(int), 1, arqBin);
    fwrite(n->chaves, sizeof(int), (ordem-1), arqBin);
    fwrite(n->registros, sizeof(int), (ordem-1), arqBin);
    fwrite(n->filhos, sizeof(int), ordem, arqBin);

    fflush(arqBin);
}

static int buscaChaveNode(ArvB* arv, int posNode, int chave, int* registroBuscado) {
    Node* n = leNodeArqBin(posNode, arv);
    int idx = buscaBinaria(chave, n->chaves, 0, n->numChavesArmazenadas-1);
    
    int chaveEncontrada = 0;
    if(idx < n->numChavesArmazenadas && n->chaves[idx] == chave) {
        if(registroBuscado != NULL) *registroBuscado = n->registros[idx];
        chaveEncontrada = 1;
    } else if(!n->ehFolha) {
        chaveEncontrada = buscaChaveNode(arv, n->filhos[idx], chave, registroBuscado);
    }

    liberaNode(n);
    return chaveEncontrada;
}

// Implementa a inserção recursiva pela árvore a partir do nó de entrada.
static void insereChaveValorRec(ArvB* arv, Node* n, int chave, int registro) {
    int idx = n->numChavesArmazenadas - 1;

    if(n->ehFolha) {
        int i = buscaBinaria(chave, n->chaves, 0, n->numChavesArmazenadas-1);
        if(i < n->numChavesArmazenadas && n->chaves[i] == chave) { // atualiza o registro caso a chave já esteja presente
            n->registros[i] = registro;
        } else {
            if(cheio(n, arv->ordem)) {
                n->ehSuperNode = TRUE;
            }
            n->numChavesArmazenadas++;
            while(idx >= 0 && chave < n->chaves[idx]) {
                n->chaves[idx+1] = n->chaves[idx];
                n->registros[idx+1] = n->registros[idx];
                idx--;
            }
    
            int idxNovaChave = idx + 1;
            n->chaves[idxNovaChave] = chave;
            n->registros[idxNovaChave] = registro;
        }

        // se o nó não é super node, ele não será splitado pelo pai, logo pode ser atualizado no arq. bin..
        // se ele fosse super node, não seria necessário passá-lo para o arq. uma vez que o split já fará isso
        if(!n->ehSuperNode) 
            escreveNodeArqBin(arv, n);
    } else {
        idx = buscaBinaria(chave, n->chaves, 0, n->numChavesArmazenadas-1);

        if(idx < n->numChavesArmazenadas && n->chaves[idx] == chave) { // atualiza o registro caso a chave já esteja presente
            n->registros[idx] = registro;
            escreveNodeArqBin(arv, n);
        } else {
            Node* nodeFilho = leNodeArqBin(n->filhos[idx], arv);
            insereChaveValorRec(arv, nodeFilho, chave, registro);
    
            if(nodeFilho->ehSuperNode) {
                splitNodeFilho(arv, n, nodeFilho, idx);
            }
            liberaNode(nodeFilho);  
        }

    }
}

// Os nós 'pai' e 'filho' não são retirados da memória principal após o split, apenas o novo nó criado é liberado.
static void splitNodeFilho(ArvB* arv, Node* pai, Node* filho, int idxFilho) {
    int posSegundoFilho = arv->offsetAcumulado; // a inserção será no final do arq. bin.
    arv->numNos++;
    arv->offsetAcumulado++;

    Node* segundoFilho = criaNode(arv->ordem, filho->ehFolha, posSegundoFilho);

    int idxMediana = filho->numChavesArmazenadas / 2; // índice da mediana das chaves de 'filho'
    
    // atualização dos tamanhos após o split
    segundoFilho->numChavesArmazenadas = (filho->numChavesArmazenadas - 1) - idxMediana;
    filho->numChavesArmazenadas = idxMediana;

    int offsetFilhoOriginal = idxMediana + 1;

    // Transfere as chaves e registros após a mediana de 'filho' para o novo filho ('segundoFilho')
    for(int i = 0; i < segundoFilho->numChavesArmazenadas; i++) {
        segundoFilho->chaves[i] = filho->chaves[offsetFilhoOriginal + i];
        segundoFilho->registros[i] = filho->registros[offsetFilhoOriginal + i];
    }

    // Transfere as referências para os filhos caso o nó splitado ('filho') seja folha
    if(!filho->ehFolha) {
        for(int i = 0; i <= segundoFilho->numChavesArmazenadas; i++) {
            segundoFilho->filhos[i] = filho->filhos[offsetFilhoOriginal + i];
        }
    }

    // abre espaço para inserir a mediana do nó splitado no pai
    int numChavesPai = pai->numChavesArmazenadas;
    for(int i = numChavesPai - 1; i >= idxFilho; i--) {
        pai->chaves[i+1] = pai->chaves[i];
        pai->registros[i+1] = pai->registros[i];
    }

    // abre espaço para inserir as novas referências
    for(int i = numChavesPai; i >= idxFilho+1; i--) {
        pai->filhos[i+1] = pai->filhos[i];
    }

    // faz as inserções nos espaços corretos do nó pai
    pai->chaves[idxFilho] = filho->chaves[idxMediana];
    pai->registros[idxFilho] = filho->registros[idxMediana];
    pai->filhos[idxFilho] = filho->posicaoArqBin;
    pai->filhos[idxFilho + 1] = segundoFilho->posicaoArqBin;

    pai->numChavesArmazenadas++;

    if(pai->numChavesArmazenadas == arv->ordem) {
        pai->ehSuperNode = TRUE;
    }
    filho->ehSuperNode = FALSE;
    segundoFilho->ehSuperNode = FALSE;

    escreveNodeArqBin(arv, pai);
    escreveNodeArqBin(arv, filho);
    escreveNodeArqBin(arv, segundoFilho);

    liberaNode(segundoFilho);
}

// Retorna o mínimo de chaves permitido para um nó interno
static int minChaves(int ordem) {
    // o incremento arredonda a divisão para cima
    return ((ordem + 1) / 2) - 1;
}

static void removeFolha(ArvB *arv, Node *n, int idxChave) {
    if(n->numChavesArmazenadas == minChaves(arv->ordem)){
        n->ehMiniNode = TRUE;
    }

    // tirando independentemente doq for acontecer dps
    // assumindo que mini nodes serao tratados pela
    // rebalanceia de forma externa
            
    // Remove a chave deslocando os elementos seguintes
    for (int i = idxChave; i < n->numChavesArmazenadas - 1; i++) {
        n->chaves[i] = n->chaves[i + 1];
        n->registros[i] = n->registros[i + 1];
    }
    
    // nao ha transferencia de filhos pois 
    // esse no eh folha

    n->numChavesArmazenadas--;
    escreveNodeArqBin(arv, n); 
}

// Realiza o procedimento de concatenação/redistribuição
static void rebalanceia(ArvB* arv, Node* pai, Node* filho, int idxFilho){
    
    // Inicia-se tentando realizar o procedimento de redistribuição
    if(idxFilho != 0) { // nó filho tem irmão à esquerda
        Node* irmao = leNodeArqBin(pai->filhos[idxFilho-1], arv); // lê irmão adjacente à esquerda
        if(irmao->numChavesArmazenadas > minChaves(arv->ordem)) { // verifica se a redistribuição é possível
            redistribuiDaEsquerda(arv, pai, idxFilho, filho, irmao);
            liberaNode(irmao);
            return;
        }
        liberaNode(irmao);
    } 
    if (idxFilho < pai->numChavesArmazenadas) { // nó filho tem irmão à direita
        Node* irmao = leNodeArqBin(pai->filhos[idxFilho+1], arv); // lê irmão adjacente à direita
        if(irmao->numChavesArmazenadas > minChaves(arv->ordem)) { // verifica se a redistribuição é possível
            redistribuiDaDireita(arv, pai, idxFilho, filho, irmao);
            liberaNode(irmao);
            return; 
        }
        liberaNode(irmao);
    }

    // Se não foi possível realizar a redistribuição com nenhum irmão adjacente, realiza-se o procedimento de concatenação
    if(idxFilho != 0) {
        
        Node* irmao = leNodeArqBin(pai->filhos[idxFilho-1], arv);
        concatenaComIrmaoEsquerdo(arv, pai, idxFilho, filho, irmao);
        if(pai->numChavesArmazenadas < minChaves(arv->ordem)) pai->ehMiniNode = TRUE;
        
        if (pai->posicaoArqBin == POSICAO_RAIZ && pai->numChavesArmazenadas == 0) { // se o pai era a raíz e ficou vazio, o irmão vira a nova raíz
            arv->numNos--;
            irmao->posicaoArqBin = POSICAO_RAIZ;
            escreveNodeArqBin(arv, irmao);
        }

        liberaNode(irmao);
        return;
    } else {
        Node* irmao = leNodeArqBin(pai->filhos[idxFilho+1], arv);
        concatenaComIrmaoEsquerdo(arv, pai, idxFilho+1, irmao, filho);
        if(pai->numChavesArmazenadas < minChaves(arv->ordem)) pai->ehMiniNode = TRUE;

        if (pai->posicaoArqBin == POSICAO_RAIZ && pai->numChavesArmazenadas == 0) {
            arv->numNos--;
            filho->posicaoArqBin = POSICAO_RAIZ;
            escreveNodeArqBin(arv, filho);
        }

        liberaNode(irmao);
        return;
    }
}

// Realiza a troca do par chave/registro de índice 'idxChave' no nó pai com o predecessor imediato.
// Obs.: assume-se que o nó filho é o sucessor do pai no índice 'idxChave'.
static int trocaChaveComPredecessor(ArvB* arv, Node* pai, Node* filho, int idxChave) {

    // busca o node mais a direita da subárvore enraizada em ant
    Node* predecessor = filho;
    while (!predecessor->ehFolha)
        predecessor = leNodeArqBin(predecessor->filhos[predecessor->numChavesArmazenadas], arv);
            
    int novaChave = predecessor->chaves[predecessor->numChavesArmazenadas - 1];
    int novoRegistro = predecessor->registros[predecessor->numChavesArmazenadas - 1];
    
    // substituição de dados
    pai->chaves[idxChave] =  novaChave;
    pai->registros[idxChave] = novoRegistro;

    escreveNodeArqBin(arv, pai);

    return novaChave;
}

// Implementa a remoção recursiva pela árvore a partir do nó de entrada.
static void removeChaveValorRec(ArvB* arv, Node* n, int chave) {
    int idx = buscaBinaria(chave, n->chaves, 0, n->numChavesArmazenadas - 1);

    if(idx == n->numChavesArmazenadas || n->chaves[idx] != chave) { // verifica se a chave a ser removida foi encontrada
        
        if(n->ehFolha) return; // chave não está na árvore

        Node* filho = leNodeArqBin(n->filhos[idx], arv);
        removeChaveValorRec(arv, filho, chave);
        
        if(filho->ehMiniNode) { // verifica se o filho se tornou mini node (possui menos chaves que o permitido)
            rebalanceia(arv, n, filho, idx);
        }
        liberaNode(filho);
    } else { // chave encontrada no nó atual
        if(n->ehFolha) {
            removeFolha(arv, n, idx);
        } else {
            Node* filho = leNodeArqBin(n->filhos[idx], arv);            

            int chavePred = trocaChaveComPredecessor(arv, n, filho, idx);
            removeChaveValorRec(arv, filho, chavePred);

            if(filho->ehMiniNode) { // verifica se o filho se tornou mini node (possui menos chaves que o permitido)
                rebalanceia(arv, n, filho, idx);
            }
            liberaNode(filho);
        }
    }

}

static void redistribuiDaEsquerda(ArvB* arv, Node* pai, int idxFilho, Node* filho, Node* irmaoEsq) {
    
    // Desloca as chaves e registros do filho para a direita
    for(int i = filho->numChavesArmazenadas-1; i >= 0; i--) {
        filho->chaves[i+1] = filho->chaves[i];
        filho->registros[i+1] = filho->registros[i];
    }

    // Desloca filhos se não for folha
    if(!filho->ehFolha){
        for(int i = filho->numChavesArmazenadas; i >= 0; i--){
            filho->filhos[i+1] = filho->filhos[i];
        }
    }

    // Move a chave do pai para o filho
    filho->chaves[0] = pai->chaves[idxFilho - 1];
    filho->registros[0] = pai->registros[idxFilho - 1];

    // Move último filho do irmão para o primeiro do filho
    if(!filho->ehFolha) {
        filho->filhos[0] = irmaoEsq->filhos[irmaoEsq->numChavesArmazenadas];
    }

    // Atualiza a chave do pai com a última chave do irmão esquerdo
    pai->chaves[idxFilho - 1] = irmaoEsq->chaves[irmaoEsq->numChavesArmazenadas - 1];
    pai->registros[idxFilho - 1] = irmaoEsq->registros[irmaoEsq->numChavesArmazenadas - 1];

    filho->numChavesArmazenadas++;
    irmaoEsq->numChavesArmazenadas--;

    escreveNodeArqBin(arv, pai);
    escreveNodeArqBin(arv, filho);
    escreveNodeArqBin(arv, irmaoEsq);
}

static void redistribuiDaDireita(ArvB* arv, Node* pai, int idxFilho, Node* filho, Node* irmaoDir) {
    
    // Move a chave do pai para o filho
    filho->chaves[filho->numChavesArmazenadas] = pai->chaves[idxFilho];
    filho->registros[filho->numChavesArmazenadas] = pai->registros[idxFilho];
    filho->numChavesArmazenadas++;

    // Atualiza a chave do pai com a primeira chave do irmão direito
    pai->chaves[idxFilho] = irmaoDir->chaves[0];
    pai->registros[idxFilho] = irmaoDir->registros[0];

    for(int i = 0; i < irmaoDir->numChavesArmazenadas-1; i++){
        irmaoDir->chaves[i] = irmaoDir->chaves[i+1];
        irmaoDir->registros[i] = irmaoDir->registros[i+1];
    }

    // Move o primeiro filho do irmão direito (se não for folha)
    if (!irmaoDir->ehFolha) {
        filho->filhos[filho->numChavesArmazenadas] = irmaoDir->filhos[0];
        for (int i = 0; i < irmaoDir->numChavesArmazenadas - 1; i++) {
            irmaoDir->filhos[i] = irmaoDir->filhos[i + 1];
        }
    }

    irmaoDir->numChavesArmazenadas--;

    escreveNodeArqBin(arv, pai);
    escreveNodeArqBin(arv, filho);
    escreveNodeArqBin(arv, irmaoDir);
}

static void concatenaComIrmaoEsquerdo(ArvB* arv, Node* pai, int idxFilho, Node* filho, Node* irmaoEsq) {
    // Move a chave do pai para o irmão esquerdo 
    irmaoEsq->chaves[irmaoEsq->numChavesArmazenadas] = pai->chaves[idxFilho - 1];
    irmaoEsq->registros[irmaoEsq->numChavesArmazenadas] = pai->registros[idxFilho - 1];
    irmaoEsq->numChavesArmazenadas++;


    // Copia chaves e filhos do filho para o irmão esquerdo
    for (int i = 0; i < filho->numChavesArmazenadas; i++) {
        irmaoEsq->chaves[irmaoEsq->numChavesArmazenadas + i] = filho->chaves[i];
        irmaoEsq->registros[irmaoEsq->numChavesArmazenadas + i] = filho->registros[i];
    }
 
    if (!irmaoEsq->ehFolha) {
        for (int i = 0; i <= filho->numChavesArmazenadas; i++) {
            irmaoEsq->filhos[irmaoEsq->numChavesArmazenadas + i] = filho->filhos[i];
        }
    }
    irmaoEsq->numChavesArmazenadas += filho->numChavesArmazenadas;

    // Remove a chave e o ponteiro do pai
    for (int i = idxFilho - 1; i < pai->numChavesArmazenadas; i++) {
        pai->chaves[i] = pai->chaves[i + 1];
        pai->registros[i] = pai->registros[i + 1];
    }
    for (int i = idxFilho; i < pai->numChavesArmazenadas; i++) {
        pai->filhos[i] = pai->filhos[i + 1];
    }

    pai->numChavesArmazenadas--;

    if(irmaoEsq->ehMiniNode == TRUE) irmaoEsq->ehMiniNode = FALSE;

    escreveNodeArqBin(arv, pai);
    escreveNodeArqBin(arv, irmaoEsq);

    arv->numNos--;
}
// ---