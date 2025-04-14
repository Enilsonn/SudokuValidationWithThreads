#include<stdio.h>

int main() {
    int i, j;

    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
            // Checa apenas uma vez se QUALQUER das 3 condições é verdadeira
            if (i == 0 || j == 0 || (i % 3 == 0 && j % 3 == 0)) {
                printf("x ");
            } else {
                printf("o ");
            }
        }
        printf("\n");
    }

    return 0;
}