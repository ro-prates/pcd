# include <stdlib.h>
# include <string.h>
# include <stdio.h>
# include <math.h>
# include <mpi.h>

# define SRAND_VALUE 1985
# define GERACOES 2000
# define TAMANHO_DO_TABULEIRO 2048

int** alocaTabuleiro(int linhas);
void atualizaBordas(int** tabuleiro, int inicio, int final);
int get_neighbors(int** tabuleiro, int i, int j);
int defineEstado(int** tabuleiro, int i, int j);

// ponteiros globais
int** tabuleiro; int** novo_tabuleiro;

int main (int argc, char* argv[]) {

    int** tmp;
    int f_up = 10;
    int f_dw = 11;
    MPI_Status s;
	int numero_processos, processo_atual, proximo_processo, processo_anterior, ierr, ferr;
    int i, j, num_linhas, lote, primeira_posicao, ultima_posicao, tamanho, soma_total;
    int contador = 0;

    srand(SRAND_VALUE);

	ierr = MPI_Init(&argc, &argv); 

		MPI_Comm_size(MPI_COMM_WORLD, &numero_processos);
		MPI_Comm_rank(MPI_COMM_WORLD, &processo_atual);

        // começa a marcar o tempo de execução
        double inicio_tempo = MPI_Wtime();

		// definindo as bordas

        // total de linhas da matriz
        num_linhas = TAMANHO_DO_TABULEIRO + (2 * numero_processos);                 
        // região que será computada por cada processo
        lote = floor((double) TAMANHO_DO_TABULEIRO / numero_processos); 
        // primeira posiçao (=1)
        // numero de elementos até a fronteira do processo atual          
        primeira_posicao = 1 + (processo_atual * lote) + (2 * processo_atual);
        // último processo
        // posição acumulado do index: processo_atual * lote
        if (processo_atual == numero_processos - 1)                        
            tamanho = TAMANHO_DO_TABULEIRO - (processo_atual * lote);
        else
            // distribuição balanceada
            tamanho = lote;                                       
        ultima_posicao = primeira_posicao + tamanho - 1;
        printf("Processo[%d/%d]: Primeiro = %d, tamanho do lote = %d, último = %d\n", processo_atual, numero_processos, primeira_posicao, tamanho, ultima_posicao);
        fflush(stdout);

        MPI_Barrier(MPI_COMM_WORLD);

        // aloca tabuleiro
            tabuleiro = alocaTabuleiro(num_linhas);
            novo_tabuleiro = alocaTabuleiro(num_linhas);
            int p, inicio, final;
            for (p = 0; p < numero_processos; p++) {
                inicio = 1 + (p * lote) + (2 * p);
                if (p == numero_processos - 1) 
                    final = inicio + (TAMANHO_DO_TABULEIRO - (p * lote));
                else
                    final = inicio + lote;
                for (i = inicio; i < final; i++)
                    for (j = 1; j <= TAMANHO_DO_TABULEIRO; j++)
                        tabuleiro[i][j] = (int) rand() % 2;
            }

        MPI_Barrier(MPI_COMM_WORLD);

        // definindo o próximo processo e o anterior 
        proximo_processo = (processo_atual + 1) % numero_processos;
        processo_anterior = ((processo_atual == 0) ? numero_processos : processo_atual) - 1;

        int gen = 1; int soma = 0;
        do {
            atualizaBordas(tabuleiro, primeira_posicao, ultima_posicao);
            MPI_Barrier(MPI_COMM_WORLD);

            // atualizando as bordas
                MPI_Sendrecv(tabuleiro[primeira_posicao], TAMANHO_DO_TABULEIRO+2, MPI_INT, processo_anterior, f_up,
                             tabuleiro[ultima_posicao+1], TAMANHO_DO_TABULEIRO+2, MPI_INT, proximo_processo, f_up,
                             MPI_COMM_WORLD, &s);

                MPI_Sendrecv(tabuleiro[ultima_posicao], TAMANHO_DO_TABULEIRO+2, MPI_INT, proximo_processo, f_dw,
                             tabuleiro[primeira_posicao-1], TAMANHO_DO_TABULEIRO+2, MPI_INT, processo_anterior, f_dw,
                             MPI_COMM_WORLD, &s);
            
            MPI_Barrier(MPI_COMM_WORLD);

            // atualizando o tabuleiro
                for (i = primeira_posicao; i <= ultima_posicao; i++)
                    for (j = 1; j <= TAMANHO_DO_TABULEIRO; j++)
                        novo_tabuleiro[i][j] = defineEstado(tabuleiro, i, j);

            MPI_Barrier(MPI_COMM_WORLD);

            // novo_tabuleiro -> tabuleiro
            tmp = tabuleiro;
            tabuleiro = novo_tabuleiro;
            novo_tabuleiro = tmp;
            gen++;
        } while (gen <= GERACOES);

        // calcula soma de céluas vivas da geração
        for (i = primeira_posicao; i <= ultima_posicao; i++)
            for (j = 1; j <= TAMANHO_DO_TABULEIRO; j++)
                soma += tabuleiro[i][j];
        MPI_Barrier(MPI_COMM_WORLD);

        // faz a redução de todas as variáveis soma, para a variável soma_total
        MPI_Reduce(&soma, &soma_total, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

        if (processo_atual == 0)
            printf("%d células vivas, após %d gerações\n", soma_total, GERACOES);

        // libera todos tabuleiros de todos processos
        for (i = 0; i < num_linhas; i++) {
            free(tabuleiro[i]);
            free(novo_tabuleiro[i]);
        }   
        free(tabuleiro);
        free(novo_tabuleiro);

    // termina de marcar o tempo de execução
    double final_tempo = MPI_Wtime();

	ferr = MPI_Finalize();

    printf("Tempo decorrido: %f\n", (final_tempo - inicio_tempo));

	return 0;
}

int** alocaTabuleiro(int linhas) {
    int i;
    int** tabuleiro = calloc(linhas, sizeof(int*));
    for (i = 0; i < linhas; i++)
        tabuleiro[i] = calloc(TAMANHO_DO_TABULEIRO+2, sizeof(int));
    return tabuleiro;
}

void atualizaBordas(int** tabuleiro, int inicio, int final) {
    int i;
    for (i = inicio; i <= final; i++) {
        tabuleiro[i][0] = tabuleiro[i][TAMANHO_DO_TABULEIRO];
        tabuleiro[i][TAMANHO_DO_TABULEIRO+1] = tabuleiro[i][1];
    }
}

int get_neighbors(int** tabuleiro, int i, int j) {
    int k; int soma = 0;
    for(k = i-1; k <= i+1; k++)
        soma += tabuleiro[k][j-1] + tabuleiro[k][j] + tabuleiro[k][j+1];
    return tabuleiro[i][j] ? soma-1 : soma;
}

int defineEstado(int** tabuleiro, int i, int j) {
    int n_neighbors = get_neighbors(tabuleiro, i, j);
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