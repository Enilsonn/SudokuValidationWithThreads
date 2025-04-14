#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/* 
    a ideia é, dada a configuracao de um jogo de sudoku (completo), criar um programa
    que valide se o jogo esta correto ou nao. Para isso, o programa deve verificar
    se cada linha, coluna e quadrante 3x3 possui os numeros de 1 a 9 sem repeticoes.
    O programa deve ser multithread, onde cada thread ira validar uma linha, coluna
    ou quadrante do sudoku
*/

#define SIZE 9 // Tamanho do tabuleiro
#define NUM_THREADS 27 // Total de threads (9 linhas + 9 colunas + 9 quadrantes)


/*
    [0 - 8] serao os qudrantes
    [9 - 17] serao as linha
    [18 - 26] serao as colunas
*/
#define inicio_quadrantes 0
#define inicio_linhas 9
#define inicio_colunas 18

/*
    para que o jogo seja verificado, todas as threads precisam retornar true
    (1) ou false (0). Para isso, cada thread ira retornar um valor inteiro
    que sera armazenado em um vetor de inteiros. No final, o programa ira
    verificar se todas as threads retornaram 1. Se sim, o jogo esta correto
*/
int results[NUM_THREADS] = {0}; // Vetor para armazenar os resultados das threads

/*
    o código verificara cada linha, coluna e 3x3. Entao havera um funcao
    para cada tipo de verificacao. Que recebera como paramentro a linha, a coluna 
    ou o quadrado 3x3
*/
struct params{
    int row; // linha a ser analizada
    int column; // coluna a ser analizada
};

// exemplo 
int sudoku[9][9] = {
    {5, 3, 4, 6, 7, 8, 9, 1, 2},
    {6, 7, 2, 1, 9, 5, 3, 4, 8},
    {1, 9, 8, 3, 4, 2, 5, 6, 7},
    {8, 5, 9, 7, 6, 1, 4, 2, 3},
    {4, 2, 6, 8, 5, 3, 7, 9, 1},
    {7, 1, 3, 9, 2, 4, 8, 5, 6},
    {9, 6, 1, 5, 3, 7, 2, 8, 4},
    {2, 8, 7, 4, 1, 9, 6, 3, 5},
    {3, 4, 5, 2, 8, 6, 1, 7, 9}
};
//talvez fosse bom colocar essa matriz dentro dos params para ficar mais organizado
/*
    struct params{
        int row; // linha a ser analizada
        int column; // coluna a ser analizada
        int** sudoku; // tabuleiro
    };
*/


// todas as funcoes serao ponteiros por causa da bibliioteca pthread
// assim como todos os parametros sao declarados como void

// primeiro vamos analizar se a linha eh valida
void *linhaValida(void* parametros){
    struct params *parameters = (struct params *)parametros;
    if(parameters->column != 0 || parameters->row > 8){
        // simplismente encerra a thread pois o parametro nao eh valido
        pthread_exit(NULL);
    }
    // agora vamos checar se cada numero aparece apenas uma vez nessa linha
    int apareceApenasUmaVezNaLinha[SIZE] = {0};
    for(int i = 0; i < SIZE; i++){
        int numeroEmVerificacao = sudoku[parameters->row][i];

        if(numeroEmVerificacao < 1 || numeroEmVerificacao > 9 ||
        apareceApenasUmaVezNaLinha[numeroEmVerificacao - 1] == 1 ){
            // ou eh um numero invalido ou ja apareceu
            pthread_exit(NULL);
        }else{
            apareceApenasUmaVezNaLinha[numeroEmVerificacao - 1] = 1;
        }
    }

    // agora todos os elementos no vetor sao 1s, logo essa linha eh valida
    results[ inicio_linhas + parameters->row ] = 1;
    pthread_exit(NULL); // fim do processo
}

// agora vamos analizar se cada linha eh valida
void *colunaValida(void* parametros){
    struct params *parameters = (struct params *) parametros;
    if(parameters->row != 0 || parameters->column > 8){
        // simplismente encerra a thread pois o parametro nao eh valido
        pthread_exit(NULL);
    }
    // agora vamos checar se cada numero aparece apenas uma vez nessa coluna
    int apareceApenasUmaVezNaColuna[SIZE] = {0};

    for(int i = 0; i < SIZE; i++){
        int numeroEmVerificacao = sudoku[i][parameters->column];
        if(numeroEmVerificacao < 1 || numeroEmVerificacao > 9 ||
        apareceApenasUmaVezNaColuna[numeroEmVerificacao - 1] == 1){
            // ou eh um numero invalido ou ja apareceu
            pthread_exit(NULL);
        } else{
            apareceApenasUmaVezNaColuna[numeroEmVerificacao - 1] = 1;
        }
    }
    // agora todos os elementos no vetor sao 1s, logo essa coluna eh valida
    results[inicio_colunas + parameters->column] = 1;
    pthread_exit(NULL); // fim do processo
}

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

void *quadranteValdido(void* parametros){
    struct params *parameters = (struct params *) parametros;
    if(parameters->row > 6 || parameters->column > 6 ||
    parameters->row%3 != 0 || parameters->column%3 != 0){
        // simplesmente encerra a thread pois o parametro nao eh valido
        pthread_exit(NULL);
    }
    // agora vamos checar se cada numero aparece apenas uma vez nesse quadrante
    int apareceApenasUmaVezNoQuadrante[SIZE] = {0};
    for(int i = parameters->row; i < parameters->row + 3; i++){
        for(int j = parameters->column; j < parameters->column + 3; j++){
            int numeroEmVerificacao = sudoku[i][j];
            if(numeroEmVerificacao < 1 || numeroEmVerificacao > 9 ||
            apareceApenasUmaVezNoQuadrante[numeroEmVerificacao - 1] == 1){
                // ou eh um numero invalido ou ja apareceu
                pthread_exit(NULL);
            }else{
                apareceApenasUmaVezNoQuadrante[numeroEmVerificacao - 1] = 1;
            }
        }
    }
    // agora todos os elementos no vetor sao 1s, logo esse quadrante eh valida
    results[parameters->row + (parameters->column)/3] = 1;
    pthread_exit(NULL); // fim do processo
}

// se desse para receber o "tabuleiro" pelos parametros da main seria show
int main(){

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
                struct params *pontoValido = (struct params *)malloc(sizeof(struct params));
                pontoValido->row = i;
                pontoValido->column = j;
                pthread_create(&threads[threadIdx++], NULL, quadranteValdido, pontoValido);
            }

            // depois veremos todas as linhas que sao validas
            if(!j){
                struct params *pontoValido = (struct params *)malloc(sizeof(struct params));
                pontoValido->row = i;
                pontoValido->column = j;
                pthread_create(&threads[threadIdx++], NULL, linhaValida, pontoValido);
            }

            // por fim veremos todas as colunas que sao validas
            if(!i){
                struct params *pontoValido = (struct params *)malloc(sizeof(struct params));
                pontoValido->row = i;
                pontoValido->column = j;
                pthread_create(&threads[threadIdx++], NULL, colunaValida, pontoValido);
            }
        }
    }

    // agora que todas as threads estao alocadas no vetor vamos junta-las todas
    // num unico processo e executalas (vao sendo executas a medida que vao sendo
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

