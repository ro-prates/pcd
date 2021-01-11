# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <omp.h>

# define NUMERO_DE_THREADS 4
# define NUMERO_DE_ITERACOES 5 // numero de iterações por thread

long int sum (long int* arr) {
    long int i; long int s = 0;
    for (i = 0; i < NUMERO_DE_THREADS; i++)
        s += arr[i];
    return s;
}

int geraNumeroAleatorio (int inferior, int superior) {
    return (rand() % (superior-inferior+1)) + inferior;
}

int main (int argc, char* argv[]) {

    srand(0);

	int requisicao = 0; int resposta = 0; int soma = 0;
	long int* clientes = calloc(NUMERO_DE_THREADS, sizeof(long int));
	long int condicao_de_parada = NUMERO_DE_THREADS*NUMERO_DE_ITERACOES;

	# pragma omp parallel num_threads(NUMERO_DE_THREADS+1)
	{
		int tid = omp_get_thread_num();
		if (tid == 0) {
			while(1) { // servidor
				while (requisicao == 0);
				resposta = requisicao;
				printf("O Servidor garantiu acesso a seção crítica ao Cliente %d\n", resposta);
				while (resposta != 0);
				requisicao = 0;

				if (sum(clientes) == condicao_de_parada) break; // verifica se todas as threads foram terminadas
			}
		}
		else {
			while(1) { // cliente
				while (resposta != tid) // seção não crítica
					requisicao = tid;

				
				int local = soma; // seção crítica
				usleep(geraNumeroAleatorio(1, 100)*1000);    // 1 até 100 milisegundos
				clientes[tid-1]++;
				soma = local + 1;
                printf("Cliente %d entrou na seção crítica [%ld]\n", tid, clientes[tid-1]); 
				resposta = 0;

                
				if (clientes[tid-1] >= NUMERO_DE_ITERACOES) break; // a threads sai da seção crítica após NUMERO_DE_ITERACOES vezes
			}	
		}
	}
    long int i;
	for (i = 0; i < NUMERO_DE_THREADS; i++)
		printf("T[%li]: %li ", i+1, clientes[i]);
	printf("\n");
	printf("SOMA: %d\n", soma);
    free(clientes);
	return 0;
}
