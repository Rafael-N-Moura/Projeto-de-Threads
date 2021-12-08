#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

/*

2. Considerando um array como entrada e um número N, verifique se ele está ordenado (não
decrescente), usando N threads.



Sugestão: particionar o array em N partes, e cada thread verifica se aquele trecho está
ordenado. Por fim, cada thread checa se o primeiro e último elemento do seu está ordenado
com seu próximo.

*/

struct args {  // a estrutura que serve de varios parametros para a pthread
    int* array;  // enderço do array iterado
    int* status; // enderaco da variavel que falara se a o array esta em ordem
    int inicio;  // inicio do i esimo pedao de tamanho sizeof(array)/n que será iterado
    int fim;     // fim do da iteraça
};

// funçao que fara a checagem
int check_order( struct args* arg ){
	
	int* array = arg->array;
	int inicio = arg->inicio;
	int fim = arg->fim;
	int i;


	for(i=inicio ; i < fim ; i++){
		if(array[i] > array[i+1]){ // testa se o numero atual é maior o proximo
			*arg->status =1;   // escreve o na memmoria de status 1 caso verdadeiro
			return 1;
		}
	}
	
	return 0;
	

}

int main (){
	
	/* array  que será iterado e as n partes */	
	int array[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30};
	int n=5;


	int i;   // variavel do for
	int status=0;// status que será usado para o resultado

	pthread_t id;  

	int size =  sizeof(array) / sizeof(array[0]);
	struct args *arg = (struct args *)malloc(sizeof(struct args)); // criando a struct
	arg->array = array;
	arg->status= &status;

	for(i=0 ; i <n ;i++){
		arg->inicio=(size/n)*i;   // a i-esima parte  de tamanho n que sera iterado por uma unica thread
		arg->fim=(size/n)*(i+1);
		pthread_create(&id,NULL, &check_order , arg);
	}

	pthread_join(id, NULL);

	// teste final
	if(*arg->status){         
		printf("\n\nnao ordenada\n");
	}else{
		printf("\n\nordenada\n");		
	}

	return 0;	

}




























