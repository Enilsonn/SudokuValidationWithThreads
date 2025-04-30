#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "header.h"

/* 
    a ideia é, dada a configuracao de um jogo de sudoku (completo), criar um programa
    que valide se o jogo esta correto ou nao. Para isso, o programa deve verificar
    se cada linha, coluna e quadrante 3x3 possui os numeros de 1 a 9 sem repeticoes.
    O programa deve ser multithread, onde cada thread ira validar uma linha, coluna
    ou quadrante do sudoku
*/

/*
    para que o jogo seja verificado, todas as threads precisam retornar true
    (1) ou false (0). Para isso, cada thread ira retornar um valor inteiro
    que sera armazenado em um vetor de inteiros. No final, o programa ira
    verificar se todas as threads retornaram 1. Se sim, o jogo esta correto
*/

// e por fim, veremos se o quadrante eh valido
/*
    A o o A o o A o o 
    o o o o o o o o o 
    o o o o o o o o o 
    A o o A o o A o o 
    o o o o o o o o o 
    o o o o o o o o o 
    A o o A o o A o o 
    o o o o o o o o o 
    o o o o o o o o o 

    os pontos definidos por A serao os parametros que iremos passar:
        (0, 0) -> 0 + 0/3 = 0
        (0, 3) -> 0 + 3/3 = 1
        (0, 6) -> 0 + 6/3 = 2
        (3, 0) -> 3 + 0/3 = 3
        (3, 3) ...
        (3, 6) ...
        (6, 0) ...
        (6, 3) -> 6 + 3/3 = 7
        (6, 6) -> 6 + 6/3 = 8
*/ 

int results[NUM_RES] = {0}; //Vetor para armazenar os resultados das threads

#include <stdio.h>

#define SIZE 9

void verificaSequencialComErros(int** sudoku) { //execução sequencial da verificação do sudoku
    int erro = 0;
    int erros[SIZE][SIZE] = {0}; //marca 1 onde houver erro

    //Verifica linhas
    for (int i = 0; i < SIZE; i++) {
        int aparece[SIZE] = {0};
        for (int j = 0; j < SIZE; j++) {
            int val = sudoku[i][j];
            if (val < 1 || val > 9) {
                erros[i][j] = 1;
                erro = 1;
            } else if (aparece[val - 1]) {
                for (int k = 0; k < SIZE; k++) {
                    if (sudoku[i][k] == val)
                        erros[i][k] = 1;
                }
                erro = 1;
            } else {
                aparece[val - 1] = 1;
            }
        }
    }

    //Verifica colunas
    for (int j = 0; j < SIZE; j++) {
        int aparece[SIZE] = {0};
        for (int i = 0; i < SIZE; i++) {
            int val = sudoku[i][j];
            if (val < 1 || val > 9) {
                erros[i][j] = 1;
                erro = 1;
            } else if (aparece[val - 1]) {
                for (int k = 0; k < SIZE; k++) {
                    if (sudoku[k][j] == val)
                        erros[k][j] = 1;
                }
                erro = 1;
            } else {
                aparece[val - 1] = 1;
            }
        }
    }

    //Verifica quadrantes
    for (int bloco_i = 0; bloco_i < SIZE; bloco_i += 3) {
        for (int bloco_j = 0; bloco_j < SIZE; bloco_j += 3) {
            int aparece[SIZE] = {0};
            //int posicoes[SIZE][2]; //guarda posições duplicadas
            int dup = 0;

            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    int linha = bloco_i + i;
                    int coluna = bloco_j + j;
                    int val = sudoku[linha][coluna];

                    if (val < 1 || val > 9) {
                        erros[linha][coluna] = 1;
                        erro = 1;
                    } else if (aparece[val - 1]) {
                        //marca todo mundo com esse valor nesse bloco
                        for (int a = 0; a < 3; a++) {
                            for (int b = 0; b < 3; b++) {
                                int li = bloco_i + a;
                                int co = bloco_j + b;
                                if (sudoku[li][co] == val) {
                                    erros[li][co] = 1;
                                }
                            }
                        }
                        erro = 1;
                    } else {
                        aparece[val - 1] = 1;
                    }
                }
            }
        }
    }

    if(!erro) printf("\nExecucao 9: Essa configuracao do tabuleiro eh valida!\n");
    else printf("\nExecucao 9: Essa configuracao do tabuleiro eh invalida!\n");

    // Imprime o Sudoku com destaque
    printf("\nSudoku analisado (X marca erros):\n\n");
    for (int i = 0; i < SIZE; i++) {
        if (i % 3 == 0 && i != 0)
            printf("---------------------\n");

        for (int j = 0; j < SIZE; j++) {
            if (j % 3 == 0 && j != 0)
                printf("| ");

            if (erros[i][j])
                printf("X ");
            else
                printf("%d ", sudoku[i][j]);
        }
        printf("\n");
    }

    if (erro)
        printf("\nErros identificados no Sudoku (marcados com X).\n");
}


void *linhaValida(void* parametros){ //função para verificar as linhas
    
    params *parameters = (params *)parametros;

    int numeroExecucoes; //se for uma thread so, executa 9x, caso sejam 9 threads, executa 1 vez so
    if(parameters->unique == 1 ){
        numeroExecucoes = 9;
    }else{
        numeroExecucoes = 1;
    }

    for(int ex = 0; ex < numeroExecucoes; ex++){
        if(parameters->column != 0 || parameters->row > 8){ //verifica se os parametros estão validos
            pthread_exit(NULL);
        }
        
        int apareceApenasUmaVezNaLinha[SIZE] = {0}; //variavel para verificar se os numeros se repetem

        if(parameters->unique) parameters->row = ex;  //se for uma thread unica, a linha segue o numero da execução

        for(int j = 0; j < SIZE; j++){ //itera por cada linha


            int numeroEmVerificacao = parameters->sudoku[parameters->row][j];

            if(numeroEmVerificacao < 1 || numeroEmVerificacao > 9 || apareceApenasUmaVezNaLinha[numeroEmVerificacao - 1] == 1 ){ //verifica se tem algum erro
                pthread_exit(NULL); //se tiver, a thread termina

            }else{
                apareceApenasUmaVezNaLinha[numeroEmVerificacao - 1] = 1;
            }
        }

        results[ inicio_linhas + parameters->row ] = 1; //se nao houve nenhum erro a linha é valida e seu resultado na lista results é 1
        //para cada linha, coluna e quadrante há um indice na lista results, então cada thread altera o indice que está sendo visto no momento, sem interferir o indice de outra thread
    }

    free(parameters);
    pthread_exit(NULL); //fim do processo
}

void *colunaValida(void* parametros){ //função para verificar as colunas

    params *parameters = (params *)parametros;
    
    int numeroExecucoes = parameters->unique == 1 ? 9:1; // se for uma thread so, executo 9x, caso sejam varias, executa 1 vez so

    for(int ex = 0; ex < numeroExecucoes; ex++){
        if(parameters->row != 0 || parameters->column > 8){ //verifica se os parametros estão validos
            pthread_exit(NULL);
        }

        int apareceApenasUmaVezNaColuna[SIZE] = {0};

        if(parameters->unique) parameters->column = ex;

        for(int i = 0; i < SIZE; i++){

            int numeroEmVerificacao = parameters->sudoku[i][parameters->column];

            if(numeroEmVerificacao < 1 || numeroEmVerificacao > 9 || apareceApenasUmaVezNaColuna[numeroEmVerificacao - 1] == 1){
                pthread_exit(NULL);
                
            } else{
                apareceApenasUmaVezNaColuna[numeroEmVerificacao - 1] = 1;
            }
        }

        
        results[inicio_colunas + parameters->column] = 1;

    }

    free(parameters);
    pthread_exit(NULL); // fim da thread
}

void *quadranteValido(void* parametros){ //função para verificar os quadrantes

    params *parameters = (params *)parametros;

    int numeroExecucoes = parameters->unique == 1 ? 9:1; // se for uma thread so, executo 9x, caso sejam varias, executa 1 vez so

    // diferentemente de colunas e linhas, em quadrantes é necessário que se percorra blocos 3x3, por isso dois for aninhados para as execuções
    // e dois for aninhados para a verificação de cada quadrante
    // de resto, tudo bem parecido com as outras duas
    for(int exi = 0; exi < numeroExecucoes; exi+=3){
        for(int exj = 0; exj < numeroExecucoes; exj+=3){
            if(parameters->row > 6 || parameters->column > 6 || parameters->row%3 != 0 || parameters->column%3 != 0){
                pthread_exit(NULL);
            }
            
            int apareceApenasUmaVezNoQuadrante[SIZE] = {0};
            if(parameters->unique){
                parameters->row = exi;
                parameters->column = exj;
            }
            
            for(int i = parameters->row; i < parameters->row + 3; i++){
                for(int j = parameters->column; j < parameters->column + 3; j++){

                    int numeroEmVerificacao = parameters->sudoku[i][j];

                    if(numeroEmVerificacao < 1 || numeroEmVerificacao > 9 || apareceApenasUmaVezNoQuadrante[numeroEmVerificacao - 1] == 1){
                        pthread_exit(NULL);

                    }else{
                        apareceApenasUmaVezNoQuadrante[numeroEmVerificacao - 1] = 1;
                    }
                }
            }
            
            results[parameters->row + (parameters->column)/3] = 1;

        
        }
    }

    free(parameters);
    pthread_exit(NULL); // fim do processo
}