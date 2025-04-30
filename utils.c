#include <stdio.h>
#include <stdlib.h>

void retornaNomeCompleto(char* destino) {
    char nomeArquivo[100];
    printf("Digite o nome do arquivo com a instância do Sudoku: ");
    scanf("%s", nomeArquivo);
    strcpy(destino, "Solutions/");
    strcat(destino, nomeArquivo);
}

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