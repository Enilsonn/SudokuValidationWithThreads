#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include "threads.c"
#include "read-sudoku.c"

void retornaNomeCompleto(char* destino) {
    char nomeArquivo[100];
    printf("Digite o nome do arquivo com a instância do Sudoku: ");
    scanf("%s", nomeArquivo);
    strcpy(destino, "Solutions/");
    strcat(destino, nomeArquivo);
}

void imprimirTabelaTempos(double* tempos) {
    printf("\n");
    printf("+-----+----------------+------------------+------------------------+---------------------+\n");
    printf("| Nº  | Unica Linha    | Unica Coluna     | Unico Quadrante        | Tempo (segundos)    |\n");
    printf("+-----+----------------+------------------+------------------------+---------------------+\n");

    for (int i = 0; i < 9; i++) {
        if (i == 8) {
            printf("| %3d | %-14s | %-16s | %-22s | %-19.6f |\n",
                i,
                "Sequencial", "Sequencial", "Sequencial",
                tempos[i]);
        } else {
            int unicaLinha = (i >> 2) & 1;
            int unicaColuna = (i >> 1) & 1;
            int unicoQuadrante = i & 1;

            printf("| %3d | %-14s | %-16s | %-22s | %-19.6f |\n",
                i,
                unicaLinha ? "Sim" : "Nao",
                unicaColuna ? "Sim" : "Nao",
                unicoQuadrante ? "Sim" : "Nao",
                tempos[i]);
        }
    }

    printf("+-----+----------------+------------------+------------------------+---------------------+\n");
}


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


int main(){
    char nomeCompleto[120];
    retornaNomeCompleto(nomeCompleto);
    int** sudoku = carregarSudoku(nomeCompleto);
    clock_t inicio, fim;
    double tempoDaExecucao;
    double tempos[9];


    for(int ex = 0; ex < 8; ex++){ //executando todas as possibilidades de threads para linha, coluna e quadrante (1 ou 9 threads)
        bool threadUnicaLinha = (ex >> 2) & 1;
        bool threadUnicaColuna = (ex >> 1) & 1;
        bool threadUnicaQuadrante = ex & 1;
        
        int numThreads = 3;
        if(!threadUnicaLinha){
            numThreads += 8;
        } if(!threadUnicaColuna){
            numThreads += 8;
        } if(!threadUnicaQuadrante){
            numThreads += 8;
        }

        pthread_t threads[numThreads]; //inicialização das threads com o numero fixo de threads
        int threadIdx = 0;

        inicio = clock();

        //chamada de função para a criação das threads
        createThreadsLinhas(threads, &threadIdx, threadUnicaLinha, sudoku);
        createThreadsColunas(threads, &threadIdx, threadUnicaColuna, sudoku);
        createThreadsQuadrantes(threads, &threadIdx, threadUnicaQuadrante, sudoku);

        for(int i = 0; i < numThreads; i++){
            pthread_join(threads[i], NULL);
        }

        fim = clock();
        tempoDaExecucao = ((double)(fim-inicio)) / CLOCKS_PER_SEC;
        tempos[ex] = tempoDaExecucao;

        printf("\nExecucao %d: ", ex+1);

        int flag = 1;
        for(int i = 0; i < NUM_RES; i++){
            if(!results[i]){ //verifica se há algum erro em linha, coluna ou quadrante
                printf("Essa configuracao do tabuleiro eh invalida!\n");
                flag = 0;
                break;
            }
        }
        
        if(flag) printf("Essa configuracao do tabuleiro eh valida!\n");

        memset(results, 0, sizeof(results));
    }
    
    inicio = clock();
    verificaSequencialComErros(sudoku);
    fim = clock();
    tempoDaExecucao = ((double)(fim-inicio)) / CLOCKS_PER_SEC;
    tempos[8] = tempoDaExecucao;


    imprimirTabelaTempos(tempos);

    return 0;
}