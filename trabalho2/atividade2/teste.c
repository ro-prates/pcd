#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define NUMERO_DE_ITERACOES 2097152

int *v1, *v2; 
int somaQuantidadeCelulasVivas = 0; 

int main(){

    int i;

    v1 = malloc(NUMERO_DE_ITERACOES*sizeof(int));
    v2 = malloc(NUMERO_DE_ITERACOES*sizeof(int));

    srand(1985);

    for(i = 0; i < NUMERO_DE_ITERACOES; i++){
        v1[i] = rand()%2;
        v2[i] = rand()%2;
    }

    double inicio = omp_get_wtime();

    for(i = 0; i < NUMERO_DE_ITERACOES; i++){
        somaQuantidadeCelulasVivas += v1[i]+v2[i];
    }
    
    printf("O somatório das celulas vivas é: %d\n",somaQuantidadeCelulasVivas);

    double fim = omp_get_wtime();
    printf("Tempo de execucao: %.3f\n",(fim-inicio));

    return 0;
}