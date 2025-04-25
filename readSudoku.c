#include <stdio.h>
#include <stdlib.h>

int** carregarSudoku(const char *nomeArquivo) {
    
    FILE *arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo");
        return 0;
    }

    int **sudoku = (int**)malloc(9 * sizeof(int*));
    if(!sudoku){
        printf("Erro na alocação\n");
    }
    for (int i = 0; i < 9; i++){
        sudoku[i] = (int*)malloc(9 * sizeof(int));
        if(!sudoku[i]){
            printf("Erro na alocação\n");
        }
    }

    // Lê os 81 inteiros do arquivo
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (fscanf(arquivo, "%d", &sudoku[i][j]) != 1) {
                fprintf(stderr, "Erro ao ler o valor [%d][%d] do arquivo.\n", i, j);
                fclose(arquivo);
                return 0;
            }
        }
    }

    fclose(arquivo);
    return sudoku;
}