// sudoku_checker.h

#ifndef SUDOKU_VALIDATION_H
#define SUDOKU_VALIDATION_H

#include <stdbool.h>

#define SIZE 9 // tamanho do tabuleiro
#define NUM_THREADS 27 // Total de threads (9 linhas + 9 colunas + 9 quadrantes)
/*
    [0 - 8] serao os qudrantes
    [9 - 17] serao as linha
    [18 - 26] serao as colunas
*/
#define inicio_quadrantes 0
#define inicio_linhas 9
#define inicio_colunas 18

extern int sudoku[SIZE][SIZE];
extern int results[NUM_THREADS]; // Vetor para armazenar os resultados das threads


/*
    o código verificara cada linha, coluna e 3x3. Entao havera um funcao
    para cada tipo de verificacao. Que recebera como paramentro a linha, a coluna 
    ou o quadrado 3x3
*/
typedef struct {
    int row;
    int column;
    int **sudoku;
    bool unique;
} params;

void* linhaValida(void* parametros);
void* colunaValida(void* parametros);
void* quadranteValido(void* parametros);

#endif
