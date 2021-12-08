#include <stdio.h>
#include <pthread.h> 
#include <stdlib.h>    
//pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
//variaveis utilizadas 
int *arr_bool;
int tam_arr,qtd_thread;
int primes_found=0;
int *arr_final=NULL;//array para colocar os primos achados e fazer o output no final
int *partition=NULL;		
//as particoes garantem que cada thread fique com um primo diferente, para assim eliminar os seus respectivos multiplos		 
void *crivo(void *part){
	int i,j;
	int*particao=(int*) part;
	for(i=particao[1];i<particao[2];i++){
		if(arr_bool[i]==1){ //primo achado, agora tem que eliminar os multiplos dele...
			arr_final = (int*) realloc (arr_final, (primes_found+1)* sizeof(int)); //isso aqui é apenas para guardar os primos achados e fazer o output no final.
		    arr_final[primes_found]=i; //coloca o  primo no array que vai fazer o output
		    primes_found++;  //qtd de primos (so p controlar o array do output e colocar os primos na pos. correta
				for(j=i;j<tam_arr;j+=i){ //thread vai ficar responsavel por isso, p checar pra cada i diferente...
				arr_bool[j]=0; //eliminando os multiplos de um determinado primo		
			} 	
		}
	}
}
main() {
	int u,p,i;                                 //variaveis e inicialização do codigo
	printf("Digite o tamanho do array: ");
	scanf("%d",&tam_arr);
	printf("Digite a quantidade de threads: ");
	scanf("%d",&qtd_thread);	
	arr_bool=(int *)malloc(tam_arr*sizeof(int));
	if(arr_bool==NULL){
		printf("Nao tem espaco suficiente na memoria\n");
		exit(0);
	}
	arr_bool[0]=0;    //ctz que nao é primo, entao leva o valor de 0.
	arr_bool[1]=0;   //ctz que nao é primo, entao leva o valor de 0.
	for(i=2;i<tam_arr;i++)arr_bool[i]=1; //todos do array leva o valor true (assim como foi pedido na questao)
  	pthread_t thread[qtd_thread]; 
  	int ID[qtd_thread];
  	 
	for(p = 0; p < qtd_thread; p++) {  //criacao das threads e divisao das particoes 
		int* partition=(int*)malloc(tam_arr*sizeof(int)); 
		partition[1] = p * (tam_arr / qtd_thread);
        partition[2] = (p+1) * (tam_arr / qtd_thread);
  	    pthread_create(&thread[p],NULL,crivo,(void*)partition);  
  }
  for(u=0; u<qtd_thread;u++) {
    pthread_join(thread[u], NULL);
  }  
  int z;
  for(z=0; z<primes_found;z++) { //output dos numeros primos
    printf("[%d] ",arr_final[z]);
  } 
  free(arr_bool);     //desalocando o espaço usado na memoria
  free(arr_final);  
  free(partition);
  pthread_exit(NULL); 
}
