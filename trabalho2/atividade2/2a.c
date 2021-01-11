# include <stdio.h>
# include <stdlib.h>
# include <omp.h>

# define SRAND_VALUE 1985
# define GERACOES 2000
# define TAMANHO_DO_TABULEIRO 2048
# define NUMERO_DE_THREADS 4

int** alocaTabuleiro();
int liberaTabuleiro(int** tabuleiro);
int posicaoInicial(int** tabuleiro);
int atualizaBordas(int** tabuleiro);
int getNeighbors(int** tabuleiro, int i, int j);
int somaCelulasVivas(int** tabuleiro);
int defineEstado(int** tabuleiro, int i, int j);

int somaQuantidadeCelulasVivas = 0;

int** tabuleiro; int** novo_tabuleiro;

int main(int argc, char* argv[]) {

    tabuleiro = alocaTabuleiro();
    novo_tabuleiro = alocaTabuleiro();
    posicaoInicial(tabuleiro);
    printf("Número de células inicialmente vivas: %d\n", somaCelulasVivas(tabuleiro));

    int i, j;
    double inicio, fim;
    int geracao_n = 1; int** aux;

    inicio = omp_get_wtime();

    omp_set_num_threads(NUMERO_DE_THREADS);

    do {
        atualizaBordas(tabuleiro);
        # pragma omp parallel private(j)
        {
            # pragma omp for
            for (i = 1; i <= TAMANHO_DO_TABULEIRO; i++)
                for (j = 1; j <= TAMANHO_DO_TABULEIRO; j++)
                    novo_tabuleiro[i][j] = defineEstado(tabuleiro, i, j);
        }
        aux = tabuleiro;
        tabuleiro = novo_tabuleiro;
        novo_tabuleiro = aux;
        geracao_n++;
    } while (geracao_n <= GERACOES);
    fim = omp_get_wtime();
    printf("Leva %f segundos.\n", fim-inicio);

    printf("Células vivas ao final de %d gerações: %d\n", GERACOES, somaCelulasVivas(tabuleiro));

    liberaTabuleiro(tabuleiro);
    liberaTabuleiro(novo_tabuleiro);
    return 0;
}

int** alocaTabuleiro() {
    int i;
    int** tabuleiro = malloc((TAMANHO_DO_TABULEIRO+2)*sizeof(int*));
    for (i = 0; i <= TAMANHO_DO_TABULEIRO+1; i++)
        tabuleiro[i] = malloc((TAMANHO_DO_TABULEIRO+2)*sizeof(int));
    return tabuleiro;
}

int liberaTabuleiro(int** tabuleiro) {
    int i;
    for (i = 0; i <= TAMANHO_DO_TABULEIRO+1; i++)
        free(tabuleiro[i]);
    free(tabuleiro);
    return 0;
}

int posicaoInicial(int** tabuleiro) {
    int i, j;
    srand(SRAND_VALUE);
    for(i = 1; i <= TAMANHO_DO_TABULEIRO; i++)
        for(j = 1; j <= TAMANHO_DO_TABULEIRO; j++)
            tabuleiro[i][j] = (int) rand() % 2;
    return 0;
}


int atualizaBordas(int** tabuleiro) {
    int i;
    
    for (i = 1; i <= TAMANHO_DO_TABULEIRO; i++) {
        tabuleiro[i][0] = tabuleiro[i][TAMANHO_DO_TABULEIRO];
        tabuleiro[i][TAMANHO_DO_TABULEIRO+1] = tabuleiro[i][1];
    }
  
    for (i = 1; i <= TAMANHO_DO_TABULEIRO; i++) {
        tabuleiro[0][i] = tabuleiro[TAMANHO_DO_TABULEIRO][i];
        tabuleiro[TAMANHO_DO_TABULEIRO+1][i] = tabuleiro[1][i];
    }
    
    tabuleiro[0][0] = tabuleiro[TAMANHO_DO_TABULEIRO][TAMANHO_DO_TABULEIRO];
    tabuleiro[0][TAMANHO_DO_TABULEIRO+1] = tabuleiro[TAMANHO_DO_TABULEIRO][1];
    tabuleiro[TAMANHO_DO_TABULEIRO+1][0] = tabuleiro[1][TAMANHO_DO_TABULEIRO];
    tabuleiro[TAMANHO_DO_TABULEIRO+1][TAMANHO_DO_TABULEIRO+1] = tabuleiro[1][1];
    return 0;
}

int getNeighbors(int** tabuleiro, int i, int j) {
    int k; int soma = 0;
    for(k = i-1; k <= i+1; k++)
        soma += tabuleiro[k][j-1] + tabuleiro[k][j] + tabuleiro[k][j+1];
    return tabuleiro[i][j] ? soma-1 : soma;
}

int defineEstado(int** tabuleiro, int i, int j) {
    int n_neighbors = getNeighbors(tabuleiro, i, j);
    if (tabuleiro[i][j]) {
        if (n_neighbors == 2 || n_neighbors == 3)
            return 1;
    }
    else {
        if (n_neighbors == 3)
            return 1;
    }
    return 0;
}

int somaCelulasVivas(int** tabuleiro){
    int j, i, soma = 0;
     #pragma omp parallel private(j)
     {
        #pragma omp for  
        for(i = 1; i <= TAMANHO_DO_TABULEIRO; i++){
            for(j = 1; j <= TAMANHO_DO_TABULEIRO; j++){
                #pragma omp critical
                soma += tabuleiro[i][j];
            }
        }
     }
    
    somaQuantidadeCelulasVivas = soma;
    
    return somaQuantidadeCelulasVivas;
}