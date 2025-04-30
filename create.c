#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "threads.c"

void createThreadsLinhas(pthread_t* threads, int* threadIdx, bool threadUnicaLinha, int** sudoku){

    if(threadUnicaLinha == 0){
        for(int i = 0; i < SIZE; i++){ //criação de 9 threads caso a threadUnicaLinha seja falso, ou seja, queremos criar uma thread para cada linha
            params* pontoValido = (params*)malloc(sizeof(params)); // Deve ser alocado um pontoValido para cada thread para evitar condições de corrida e caso
            pontoValido->row = i;                                  // ele fosse compartilhado o paralelismo da utilização das threads seria desperdiçado, então
            pontoValido->column = 0;                               // no final das contas acontece um trade-off entre tempo de execução e uso de memória
            pontoValido->sudoku = sudoku;                          // Nesse caso, cada thread tem um pontoValido em diferentes espaços de memória
            pontoValido->unique = threadUnicaLinha;

            pthread_create(&threads[*threadIdx], NULL, linhaValida, pontoValido);
            (*threadIdx)++;
        }

    }else{ //criação de apenas uma thread para verificar todas as linhas
        params *pontoValido = (params *)malloc(sizeof(params));
        pontoValido->row = 0;
        pontoValido->column = 0; 
        pontoValido->sudoku = sudoku;
        pontoValido->unique = threadUnicaLinha;
        pthread_create(&threads[*threadIdx], NULL, linhaValida, pontoValido);
        (*threadIdx)++;
    }
}

void createThreadsColunas(pthread_t* threads, int* threadIdx, bool threadUnicaColuna, int** sudoku){

    if(threadUnicaColuna == 0){
        for(int i = 0; i < SIZE; i++){
            params* pontoValido = (params*)malloc(sizeof(params));
            pontoValido->row = 0;
            pontoValido->column = i;
            pontoValido->sudoku = sudoku;
            pontoValido->unique = threadUnicaColuna;

            pthread_create(&threads[*threadIdx], NULL, colunaValida, pontoValido);
            (*threadIdx)++;
        }
    }else{
        params* pontoValido = (params*)malloc(sizeof(params));
        pontoValido->row = 0;
        pontoValido->column = 0;
        pontoValido->sudoku = sudoku;
        pontoValido->unique = threadUnicaColuna;
        pthread_create(&threads[*threadIdx], NULL, colunaValida, pontoValido);
        (*threadIdx)++;    
    }
}

void createThreadsQuadrantes(pthread_t* threads, int* threadIdx, bool threadUnicaQuadrante, int** sudoku){

    if(threadUnicaQuadrante == 0){
        for(int i = 0; i < SIZE; i+=3){
            for(int j = 0; j < SIZE; j+=3){ 
                params* pontoValido = (params*)malloc(sizeof(params));
                pontoValido->row = i;
                pontoValido->column = j;
                pontoValido->sudoku = sudoku;
                pontoValido->unique = threadUnicaQuadrante;

                pthread_create(&threads[*threadIdx], NULL, quadranteValido, pontoValido);
                (*threadIdx)++;
            }
        }
        
    }else{
        params* pontoValido = (params*)malloc(sizeof(params));
        pontoValido->row = 0;
        pontoValido->column = 0;
        pontoValido->sudoku = sudoku;
        pontoValido->unique = threadUnicaQuadrante;
        pthread_create(&threads[*threadIdx], NULL, quadranteValido, pontoValido);
        (*threadIdx)++;
    }
}
