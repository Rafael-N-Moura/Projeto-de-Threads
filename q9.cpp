#include <stdio.h>
#include <pthread.h>
#include <stdlib.h> //cria uma variavel que pega o ultimo primo, essa variavel eh a regiao critica e usa o mutex... (é essa parte q eu nao to sabendo fazer)
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int *arr_bool = NULL;
int tam_arr, qtd_thread;
int flag;
int primes_found = 0;
int *arr_final = NULL;
int lastPrime = 2;

void *crivo(void *tid)
{
	while (1)
	{
		pthread_mutex_lock(&mutex);
		while (arr_final[lastPrime] != 1)
		{
			if (lastPrime >= tam_arr - 1)
			{
				return 0;
			}
			lastPrime++;
		}

		int a = tam_arr / lastPrime;
		for (int j = 2; j < a; j++)
		{
			arr_final[lastPrime * j] = 0;
		}
		lastPrime++;
		pthread_mutex_unlock(&mutex);
	}

	// int i, j;
	// int id_thread = (*(int *)tid);
	// arr_bool = (int *)malloc(tam_arr * sizeof(int));
	// arr_bool[0] = 0; //ctz que nao é primo, entao leva o valor de 0.
	// arr_bool[1] = 0;
	// for (i = 2; i < tam_arr; i++)
	// 	arr_bool[i] = 1; //colocando todas as posicoes como true a partir de 2
	// for (i = 2; i < tam_arr; i++)
	// {
	// 	if (arr_bool[i] == 1)
	// 	{																			 //primo achado, agora tem que eliminar os multiplos dele...
	// 		arr_final = (int *)realloc(arr_final, (primes_found + 1) * sizeof(int)); //isso aqui é apenas para guardar os primos achados e fazer o output no final.
	// 		arr_final[primes_found] = i;											 //coloca o  primo no array que vai fazer o output
	// 		primes_found++;															 //qtd de primos
	// 		for (j = i; j < tam_arr; j += i)
	// 		{
	// 			arr_bool[j] = 0; //eliminando os multiplos de um determinado primo
	// 		}
	// 	}
	// }
}

main()
{
	int u, p; //variaveis e inicialização do codigo
	printf("Digite o tamanho do array: ");
	scanf("%d", &tam_arr);
	printf("Digite a quantidade de threads: ");
	scanf("%d", &qtd_thread);
	pthread_t thread[qtd_thread];
	int *ID[qtd_thread];
	arr_final = (int *)malloc(sizeof(int) * tam_arr);
	arr_final[0] = 0;
	arr_final[1] = 0;
	for (int i = 2; i < tam_arr; i++)
	{
		arr_final[i] = 1;
	}

	for (p = 0; p < qtd_thread; p++)
	{ //criacao das threads
		// ID[p] = (int *)malloc(sizeof(int));
		// *ID[p] = p;
		pthread_create(&thread[p], NULL, &crivo, NULL);
	}
	for (u = 0; u < qtd_thread; u++)
	{
		pthread_join(thread[u], NULL);
	}
	// int z;
	// for (z = 0; z < primes_found; z++)
	// { //output dos numeros primos
	// 	printf("[%d] ", arr_final[z]);
	// }
	for (int i = 0; i < tam_arr; i++)
	{
		if (arr_final[i] == 1)
			printf("[%d]", i);
	}

	pthread_exit(NULL);
	free(arr_bool); //desalocando o espaço usado na memoria
	free(arr_final);
}
