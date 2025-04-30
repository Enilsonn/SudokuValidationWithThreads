#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "sudoku-validation.h"

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

int results[NUM_THREADS] = {0}; // Vetor para armazenar os resultados das threads

void *linhaValida(void* parametros){
    
    params *parameters = (params *)parametros;

    int numeroExecucoes; // se for uma thread so, executo 9x, caso sejam varias, executa 1 vez so
    if(parameters->unique == 1 ){
        numeroExecucoes = 9;
    }else{
        numeroExecucoes = 1;
    }

    for(int ex = 0; ex < numeroExecucoes; ex++){
        if(parameters->column != 0 || parameters->row > 8){ 
            pthread_exit(NULL);
        }
        
        int apareceApenasUmaVezNaLinha[SIZE] = {0};

        if(parameters->unique) parameters->row = ex;

        for(int j = 0; j < SIZE; j++){


            int numeroEmVerificacao = parameters->sudoku[parameters->row][j];

            if(numeroEmVerificacao < 1 || numeroEmVerificacao > 9 || apareceApenasUmaVezNaLinha[numeroEmVerificacao - 1] == 1 ){
                pthread_exit(NULL);

            }else{
                apareceApenasUmaVezNaLinha[numeroEmVerificacao - 1] = 1;
            }
        }

        results[ inicio_linhas + parameters->row ] = 1;
        
        //free(parameters);
    }

    pthread_exit(NULL); // fim do processo
}

void *colunaValida(void* parametros){

    params *parameters = (params *)parametros;
    
    int numeroExecucoes = parameters->unique == 1 ? 9:1; // se for uma thread so, executo 9x, caso sejam varias, executa 1 vez so

    for(int ex = 0; ex < numeroExecucoes; ex++){
        if(parameters->row != 0 || parameters->column > 8){
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

        //free(parameters);
    }

    pthread_exit(NULL); // fim do processo
}

void *quadranteValido(void* parametros){

    params *parameters = (params *)parametros;

    int numeroExecucoes = parameters->unique == 1 ? 9:1; // se for uma thread so, executo 9x, caso sejam varias, executa 1 vez so

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

            //free(parameters);
        
        }
    }

    pthread_exit(NULL); // fim do processo
}