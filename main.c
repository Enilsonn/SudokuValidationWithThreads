#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "sudoku-validation.c"
#include "readSudoku.c"

// se desse para receber o "tabuleiro" pelos parametros da main seria show
int main(){
    char nomeArquivo[100];
    printf("Digite o nome do arquivo com a instância do Sudoku: ");
    scanf("%s", nomeArquivo);
    char nomeCompleto[120] = "Solutions/";
    strcat(nomeCompleto, nomeArquivo);
    int** sudoku = carregarSudoku(nomeCompleto);

    pthread_t threads[NUM_THREADS];

    int threadIdx = 0;

    // agora vamos escolher os pontos usandos
    /*
        x x x x x x x x x 
        x o o o o o o o o 
        x o o o o o o o o 
        x o o x o o x o o 
        x o o o o o o o o 
        x o o o o o o o o 
        x o o x o o x o o 
        x o o o o o o o o 
        x o o o o o o o o 
    */
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            // primeiros vamos pegar os pontos dos quadrantes e endereca-los as threads
            if(!(i%3) && !(j%3)){
                params *pontoValido = (params *)malloc(sizeof(params));
                pontoValido->row = i;
                pontoValido->column = j;
                pontoValido->sudoku = sudoku;
                pthread_create(&threads[threadIdx++], NULL, quadranteValido, pontoValido);
            }

            // depois veremos todas as linhas que sao validas
            if(!j){
                params *pontoValido = (params *)malloc(sizeof(params));
                pontoValido->row = i;
                pontoValido->column = j;
                pontoValido->sudoku = sudoku;
                pthread_create(&threads[threadIdx++], NULL, linhaValida, pontoValido);
            }

            // por fim veremos todas as colunas que sao validas
            if(!i){
                params *pontoValido = (params *)malloc(sizeof(params));
                pontoValido->row = i;
                pontoValido->column = j;
                pontoValido->sudoku = sudoku;
                pthread_create(&threads[threadIdx++], NULL, colunaValida, pontoValido);
            }
        }
    }

    // agora que todas as threads estao alocadas no vetor vamos junta-las todas
    // num unico processo e executá-las (vao sendo executas a medida que vao sendo
    // juntadas num processo)
    for(int i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);

    
    // Processamento terminado, agora vamos verificar o vetor de results se as linhas,
    // colunas e quadrantes sao validos
    for(int i = 0; i < NUM_THREADS; i++){
        if(!results[i]){
            printf("Essa configuracao do tabuleiro eh invalida!\n");
            return 0;
        }
    }

    printf("Essa configuracao do tabuleiro eh valida!\n");
    return 0;
}