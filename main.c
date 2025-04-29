#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "sudoku-validation.c"
#include "read-sudoku.c"

void retornaNomeCompleto(char* destino) {
    char nomeArquivo[100];
    printf("Digite o nome do arquivo com a instância do Sudoku: ");
    scanf("%s", nomeArquivo);
    strcpy(destino, "Solutions/");
    strcat(destino, nomeArquivo);
}

void createThreadsLinhas(pthread_t* threads, int* threadIdx, bool threadUnicaLinha, int** sudoku){

    if(threadUnicaLinha == 0){
        for(int i = 0; i < SIZE; i++){
            params* pontoValido = (params*)malloc(sizeof(params)); // Deve ser alocado um pontoValido para cada thread para evitar condições de corrida e caso
            pontoValido->row = i;                                  // ele fosse compartilhado o paralelismo da utilização das threads seria desperdiçado, então
            pontoValido->column = 0;                               // no final das contas acontece um trade-off entre tempo de execução e uso de memória
            pontoValido->sudoku = sudoku;                          // Nesse caso, cada thread tem um pontoValido em diferentes espaços de memória

            pthread_create(&threads[*threadIdx], NULL, linhaValida, pontoValido);
            (*threadIdx)++;
        }
    }else{
        params *pontoValido = (params *)malloc(sizeof(params));
        pontoValido->sudoku = sudoku;
    }
}

void createThreadsColunas(pthread_t* threads, int* threadIdx, bool threadUnicaColuna, int** sudoku){

    if(threadUnicaColuna == 0){
        for(int i = 0; i < SIZE; i++){
            params* pontoValido = (params*)malloc(sizeof(params));
            pontoValido->row = 0;
            pontoValido->column = i;
            pontoValido->sudoku = (int**)sudoku;

            pthread_create(&threads[*threadIdx], NULL, colunaValida, pontoValido);
            (*threadIdx)++;
        }
    }else{
        params *pontoValido = (params *)malloc(sizeof(params));
        pontoValido->sudoku = sudoku;
    }
}

void createThreadsQuadrantes(pthread_t* threads, int* threadIdx, bool threadUnicaQuadrante, int** sudoku){

    if(threadUnicaQuadrante == 0){
        for(int i = 0; i < SIZE; i+=3){
            for(int j = 0; j < SIZE; j+=3){
                params* pontoValido = (params*)malloc(sizeof(params));
                pontoValido->row = i;
                pontoValido->column = j;
                pontoValido->sudoku = (int**)sudoku;

                pthread_create(&threads[*threadIdx], NULL, quadranteValido, pontoValido);
                (*threadIdx)++;
            }
        }
    }else{
        params *pontoValido = (params *)malloc(sizeof(params));
        pontoValido->sudoku = sudoku;
    }
}


int main(){
    char nomeCompleto[120];
    retornaNomeCompleto(nomeCompleto);
    int** sudoku = carregarSudoku(nomeCompleto);

    int numThreads = 3;
    bool threadUnicaLinha = 0, threadUnicaColuna = 0, threadUnicaQuadrante = 0;
    if(!threadUnicaLinha){
        numThreads += 8;
    } if(threadUnicaColuna){
        numThreads += 8;
    } if(threadUnicaQuadrante){
        numThreads += 8;
    }

    pthread_t threads[NUM_THREADS];
    int threadIdx = 0;

    createThreadsLinhas(threads, &threadIdx, threadUnicaLinha, sudoku);
    createThreadsColunas(threads, &threadIdx, threadUnicaColuna, sudoku);
    createThreadsQuadrantes(threads, &threadIdx, threadUnicaQuadrante, sudoku);

    for(int i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);

    for(int i = 0; i < NUM_THREADS; i++){
        if(!results[i]){
            printf("Essa configuracao do tabuleiro eh invalida!\n");
            return 0;
        }
    }

    printf("Essa configuracao do tabuleiro eh valida!\n");
    return 0;
}

    //while para testar todas as possibilidades:
    /*
            linha   coluna  quadrante   threadUnicaLinha   threadUnicaColuna    threadUnicaQuadrante    total de threads
              0        0        0               -                  -                     -                     0
              1        1        1               1                  1                     1                     3
              1        1        9               1                  1                     0                     11
              1        9        1               1                  0                     1                     11   
              9        1        1               0                  1                     1                     11
              1        9        9               1                  0                     0                     19   
              9        1        9               0                  1                     0                     19
              9        9        1               0                  0                     1                     19    
              9        9        9               0                  0                     0                     27 
    */