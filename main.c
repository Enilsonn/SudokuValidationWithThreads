#include <string.h>
#include <time.h>
#include "create.c"
#include "utils.c"

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

/*
        000
        001
        010
        011
        100
        101
        110
        111
*/

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