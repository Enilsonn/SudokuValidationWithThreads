#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
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
            pontoValido->unique = threadUnicaLinha;

            pthread_create(&threads[*threadIdx], NULL, linhaValida, pontoValido);
            (*threadIdx)++;
        }

    }else{
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


int main(){
    char nomeCompleto[120];
    retornaNomeCompleto(nomeCompleto);
    int** sudoku = carregarSudoku(nomeCompleto);
    clock_t inicio, fim;
    double tempoDaExecucao;
    double tempos[8];

    for(int ex = 0; ex < 8; ex++){
        // Pegando os 3 primeiros bits de 0(000) a 7(111), é possível gerar todas as combinações de variáveis
        bool threadUnicaLinha = (ex >> 2) & 1;
        bool threadUnicaColuna = (ex >> 1) & 1;
        bool threadUnicaQuadrante = ex & 1;

        printf("quantidades de threads para:\nLinhas: %d\nColunas: %d\nQuadradrantes: %d\n\n", threadUnicaLinha, threadUnicaColuna, threadUnicaQuadrante);
        
        int numThreads = 3;
        if(!threadUnicaLinha){
            numThreads += 8;
        } if(!threadUnicaColuna){
            numThreads += 8;
        } if(!threadUnicaQuadrante){
            numThreads += 8;
        }

        pthread_t threads[numThreads];
        int threadIdx = 0;

        createThreadsLinhas(threads, &threadIdx, threadUnicaLinha, sudoku);
        createThreadsColunas(threads, &threadIdx, threadUnicaColuna, sudoku);
        createThreadsQuadrantes(threads, &threadIdx, threadUnicaQuadrante, sudoku);

        for(int i = 0; i < numThreads; i++){
            pthread_join(threads[i], NULL);
        }
        printf("\n");

        int flag = 1;
        for(int i = 0; i < NUM_THREADS; i++){
            if(!results[i]){
                printf("Essa configuracao do tabuleiro eh invalida!\n");
                flag = 0;
                printf("Problema em: %d\n", i);
                break; // para mostrar a coluna, esse return poderia ser tirado e mostrar o segundo 0 na lista de resultados
            }
        }
        
        if(flag) printf("Essa configuracao do tabuleiro eh valida!\n");

        printf("results = [ ");
        for(int a = 0; a < NUM_THREADS; a++){
            printf("%d", results[a]);
            if(a != NUM_THREADS-1) printf(", ");
        }
        printf("]\n");

        memset(results, 0, sizeof(results));
    }


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