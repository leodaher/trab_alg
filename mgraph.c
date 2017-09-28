#include <stdio.h>
#include <stdlib.h>
#include "mgraph.h"

struct m_graph {
    int dir; // direcionado ou nao
    int n;  // tamanho (num de vertices)
    float** matrix;
};

M_GRAPH* m_graph_create(int n, int dir) {  //Cria o grafo com a matriz recebendo o numero de vertices e dizendo se é ou nao direcionada
    M_GRAPH* g = (M_GRAPH*) malloc(sizeof(M_GRAPH));
    int i, j;

    g->n = n;
    g->dir = dir;
    g->matrix = (float **) malloc(sizeof(float *) * n);
    for (i = 0; i < n; i++)
        g->matrix[i] = (float *) malloc(sizeof(float) * n);

    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            if(i == j)
              g->matrix[i][j] = 0;
            else
              g->matrix[i][j] = -1; // matrix vazia

    return g;
}

void m_graph_insert(M_GRAPH* g, int row, int col, float w) { //Insere uma aresta na matriz
    g->matrix[row][col] = w;
    if (!g->dir) g->matrix[col][row] = w;
}

int m_graph_checkedge(M_GRAPH* g, int row, int col) { //Verifica se existe aresta
    if (g->matrix[row][col] < 0)
         return 0;//Aresta não existe no grafo
    return 1; // Aresta existe no grafo
}

float m_graph_getedge(M_GRAPH* g, int row, int col) {
    return g->matrix[row][col];
}

void m_graph_setedge(M_GRAPH* g, int row, int col, float w) {
    g->matrix[row][col] = w;
}

void m_graph_adj(M_GRAPH* g, int v) { //Mostra os vertices adjacentes do vertice desejado
    int i;

    for (i = 0; i < g->n; i++) {
        if (g->matrix[v][i] >= 0)
            printf("%d ", i); // vertices adjacentes
    }
    printf("\n");
}

void m_graph_remove(M_GRAPH* g, int row, int col) { //Remove uma das arestas do grafo
    g->matrix[row][col] = -1;
    if (!g->dir) g->matrix[col][row] = -1;
}

int m_graph_nvertex(M_GRAPH* g) {
    return g->n;
}

void m_graph_print(M_GRAPH* g) { //Apresenta o Grafo printando para o usuario
    int i, j;
    for (i = 0; i < g->n; i++) {
        for (j = 0; j < g->n; j++) {
            if (g->matrix[i][j] < 0) printf(". ");
            else printf("%f ", g->matrix[i][j]);
        }
        printf("\n");
    }
}

void m_graph_multiply_edges(M_GRAPH* g, int * e) {
  int i, j;
  for(i = 0; i < g->n; i++) {
    for(j = 0; j < g->n; j++) {
      if (g->matrix[i][j] > 0) {
        g->matrix[i][j] *= e[i];
      }
    }
  }
}

M_GRAPH* m_graph_transpose(M_GRAPH* g) {
    M_GRAPH* t = m_graph_create(g->n,g->dir);
    int i, j;

    for (i = 0; i < t->n; i++)
        for (j = 0; j < t->n; j++)
            t->matrix[j][i] = g->matrix[i][j]; // transposicao

    return t;
}

void m_graph_printtranspose(M_GRAPH* g) {
    M_GRAPH* t = m_graph_create(g->n,g->dir);
    int i, j;

    for (i = 0; i < t->n; i++)
        for (j = 0; j < t->n; j++)
            t->matrix[j][i] = g->matrix[i][j]; // transposicao

    m_graph_print(t);
    m_graph_free(t);
}

void m_graph_printloweredge(M_GRAPH* g) {
    int i, j, row, col, lower = MAX;

    for (i = 0; i < g->n; i++) {
        for (j = 0; j < g->n; j++) {
            if (g->matrix[i][j] < lower && g->matrix[i][j] >= 0) {
                lower = g->matrix[i][j];
                row = i; col = j;
            }
        }
    }

    if (!g->dir) { // printa a aresta com menor peso corretamente
        if (row <= col) {
            printf("%d %d\n", row, col);
        } else printf("%d %d\n", col, row);
    } else printf("%d %d\n", row, col);
}

void m_graph_free(M_GRAPH* g) { //Libera o espaço armazenado pelo grafo na memória, vulgo apaga ela
    int i;
    for (i = 0; i < g->n; i++) free(g->matrix[i]);
    free(g->matrix);
    free(g);
}

float ** floyd_warshall (M_GRAPH* graph, int * E){
    float ** M = graph->matrix;
    int n = graph->n;
    int i, j, k;
    for (k = 0; k < n; k++){
        for (i = 0; i < n; i++){
            for (j = 0; j < n; j++){
                if(M[i][k] != -1 && M[k][j] != -1) {
                  if (M[i][k] + M[k][j] < M[i][j] || M[i][j] == -1)
                      M[i][j] = M[i][k] + M[k][j];
                }
            }
        }
    }

    for(i = 0; i < n; i++) {
      for(j = 0; j < n; j++) {
        M[i][j] *= E[i];
      }
    }

    return M;
}

int center_vertex (float **M, int n){
    int i, j;

    float *aux = malloc(sizeof(float));
    for (j = 0; j < n; j++){
        int maior = M[0][j];
        for (i = 0; i < n; i++){
            if (M[i][j] > maior || M[i][j] == -1)
                maior = M[i][j];
        }
        aux[j] = maior;
    }

    int menor = 0;

    for (i = 0; i < n; i++){
        if (aux[i] < aux[menor] || aux[menor] == -1)
            menor = i;

    }
    return menor;
}

int * fill_E(M_GRAPH * graph) {
    int *E = (int*) malloc(sizeof(int)*graph->n);
    for(int i = 0; i < graph->n; i++)
        E[i] = 1;
    return E;
}

float * sum_rows(M_GRAPH * graph, float **matrix) {
    int i, j;
    float *aux = malloc(sizeof(float));
    for (j = 0; j < graph->n; j++) {
    float soma = 0;
    int div = 0;
        for (i = 0; i < graph->n; i++) {
            soma += matrix[i][j];
            if (matrix[i][j] > 0)
                div++;
        }
    aux[j] = soma/div;
  }
  return aux;
}

int encontra_menor(M_GRAPH * graph, float *aux) {
    int i;
    int result = 0;
    for (i = 0; i < graph->n; i++) {
        if (aux[i] < aux[result])
            result = i;
    }
    return result;
}