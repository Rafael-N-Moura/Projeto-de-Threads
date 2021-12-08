#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <math.h>


/*

8. Um exemplo muito comum de controle de processo é o Problema da Montanha Russa.
Nesse problema, existem 3 tipos de threads:
○ a montanha russa;
○ os passageiros;
○ o(s) carrinho(s).
Para o nosso problema, vamos supor que temos 20 passageiros ao todo e 1 carrinho.
Os passageiros precisam esperar na fila até que consigam o carrinho para andar na montanha
russa. O carrinho suporta no máximo 10 passageiros por viagem, e ele só sai da estação
quando estiver cheio. Depois do passeio, os passageiros, um pouco enjoados, passeiam pelo
parque antes de retornarem à fila para andar novamente na montanha russa. Porém, por
motivos de segurança, o carrinho só pode fazer 10 voltas por dia.
Supondo que o carro e cada passageiro sejam representados por uma thread diferente,
escreva um programa, usando pthreads, que simule o sistema descrito. Após as 10 voltas, o
programa deve ser encerrado

*/

#define CARRO_THREADS_NUMBER 1
#define PASSAGEIROS_THREADS_NUMBER 20

#define MAX_VOLTAS 10
#define MAX_PASSAGEIROS 10

#define AGUARDANDO 0    // ESSES SÃO OS TRES ESTADOS DO CARRO
#define CORRENDO 1
#define DESEMBARCANDO 2

#define DESCANSANDO 0   // ESSES SÃO OS TRES ESTADOS DOS PASSAGEIROS
#define NA_FILA 1
#define NO_CARRO 2


pthread_mutex_t carro_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t passageiro_mutex = PTHREAD_MUTEX_INITIALIZER;



typedef struct{      // ESSA ESTRUTURA DEFINE O ALGUMAS CARACTERISTICAS DOS PASSAGEIROS
	int id;
	int status;       // O STATUS É O ESTADO DO ATUAL DO PASSAGEIRO
	int lugar_na_fila;
	int n_voltas;
}Passageiro; 


// ALGUMAS VARIAVEIS
int voltas=0;  // VOLTAS QUE O CARRO DÁ
int estado_carro[CARRO_THREADS_NUMBER];
int ultimo_na_fila=0;   //ESSA VARIAVEL CONTROLA O TAMANHO DA FILA

Passageiro passageiro[PASSAGEIROS_THREADS_NUMBER];

void *vida_passageiros(void *tid){  // ESSA FUNÇÃO INICIALIZA E ADMINISTRA O ESTADO DO PASSAGEIRO
	int threadId =  (*(int *)tid);

	srand((long int) &threadId);

	pthread_mutex_lock(&passageiro_mutex);          // AQUI É DEFINIDO O ESTADO INICIAL DO PASSAGEIRO   
	passageiro[threadId].id = threadId;  
	passageiro[threadId].status = rand()%2;    		// O ESTADO É DEFINIDO PELA FUNÇÃO RAND()
	passageiro[threadId].lugar_na_fila  = -1;
	pthread_mutex_unlock(&passageiro_mutex);

	if(passageiro[threadId].status == NA_FILA){    		   // SE ESTADO INICIAL FOR NA_FILA ...
		pthread_mutex_lock(&carro_mutex); 
		passageiro[threadId].lugar_na_fila=ultimo_na_fila; // ... ELE É COLOCADO NO ULTIMO LUGAR DA FILA
		ultimo_na_fila++;								   // AO FINAL O ULTIMO LUGAR É INCREMENTADO
		pthread_mutex_unlock(&carro_mutex);
	}

	int this = threadId ;
	int i,j,k;
	
	while(voltas < MAX_VOLTAS){

		if(passageiro[this].status==DESCANSANDO){   // APENAS UM ESTADO É DESCRITO POIS OS OUTROS SÃO
													// ADMINISTRADOS NA FUNÇÃO "corrida()"
			
			// PRIMEIRA ELE  ESPERA NESSA ROTINA DE TEMPO ONDE O PASSAGEIRO "PASSEIA"
			int espera;
			srand((long int )&espera);
			espera = rand()%300;
			for(i=0;i < 250 + espera ;i++){	
				for(j=0;j < 100 + espera  ; j++){
				}
			}

			//DEPOIS ELE ENTRA NA FILA
			pthread_mutex_lock(&passageiro_mutex);
			pthread_mutex_lock(&carro_mutex);
			passageiro[this].status=NA_FILA;                 
			passageiro[this].lugar_na_fila = ultimo_na_fila;  
			ultimo_na_fila++;
			pthread_mutex_unlock(&passageiro_mutex); 
			pthread_mutex_unlock(&carro_mutex);  
		}


	}

}

void *corrida(void *tid){  // ESSA FUNÇÃO ADMINISTRA OS TRES ESTADOS DO CARRINHO ATE ATINGIR O LIMITE DE RODADAS
	int i,j,k;
	int threadId =  (*(int *)tid);

	estado_carro[threadId]=AGUARDANDO;

	while( voltas < MAX_VOLTAS ){
	
		if(estado_carro[threadId] == AGUARDANDO){   // NESSE PRIMEIRO ESTADO ELE APENAS CONFERE SE
			pthread_mutex_lock(&carro_mutex);   	// HÁ PESSOAS O SUFICIENTE NA FILA
			if(ultimo_na_fila >= MAX_PASSAGEIROS ){ estado_carro[threadId] = CORRENDO; }  //SE SIM ELE PULA PRO PROXIMO
			pthread_mutex_unlock(&carro_mutex);
		}
	
		if(estado_carro[threadId] == CORRENDO){

			// OS PRIMEIROS PASSAGEIROS ENTRAO NO CARRO
			pthread_mutex_lock(&passageiro_mutex);
			for(i=0; i < PASSAGEIROS_THREADS_NUMBER ;i++){ // O TESTE DA LINHA ABAIXO CONFERE QUEM SÃO OS PRIMEIROS PASSAGEIROS DA FILA
				if(passageiro[i].lugar_na_fila < MAX_PASSAGEIROS && passageiro[i].status == NA_FILA ){
					passageiro[i].status = NO_CARRO;      // O STATUS DE NA_FILA É TROCADO PARA NO_CARRO
				}
			}
			pthread_mutex_unlock(&passageiro_mutex);

			// ABAIXO É PRINTADO O MENO PRINCIPAL

			printf("\n*************************************************************\n");
			printf("            o carro esta correndo a %2d° volta\n\n", voltas);
			printf("     passageiro |              status | lugar na fila/carro |\n");
			printf("                |                     |                     |\n");
				for(i=0;i < PASSAGEIROS_THREADS_NUMBER;i++){
					printf("             %2d |",passageiro[i].id);
					switch(passageiro[i].status){
						case NA_FILA:
							printf("         %11s |                  %2d |\n","NA_FILA", passageiro[i].lugar_na_fila);
						break;
						case NO_CARRO:
							printf("         %11s |                  %2d |\n","NO_CARRO", passageiro[i].lugar_na_fila);
						break;
						case DESCANSANDO:
							printf("         %11s |              NENHUM | \n","DESCANSANDO");
						break;
	
					}
				}

			// PEQUENA ROTINA DE TEMPO SIMULANDO O ANDAR DO CARRO
			for(i=0;i < 100;i++){
				for(j=0;j < 100 ;j++){
				}
			}
			// PULA PARA O PROXIMO ESTADO
			estado_carro[threadId] = DESEMBARCANDO;

		}

		if(estado_carro[threadId] == DESEMBARCANDO){  // ESSE É O ULTIMO ESTADO DO CICLO
			
			pthread_mutex_lock(&passageiro_mutex);
			pthread_mutex_lock(&carro_mutex);
			ultimo_na_fila-=MAX_PASSAGEIROS; 					//PASSAGEIROS DESEMBARCAM

			for(i=0; i < PASSAGEIROS_THREADS_NUMBER ;i++){
				passageiro[i].lugar_na_fila-=MAX_PASSAGEIROS;   // A FILA "ANDA"		
				if(passageiro[i].status == NO_CARRO){           // OS PASSAGEIROS QUE ESTAVAM NO CARRO
					passageiro[i].status = DESCANSANDO;			// DESCANSAM
					passageiro[i].n_voltas++;					// ACRESCENTAM O NUMERO DE VOLTAS QUE DERAM
					passageiro[i].lugar_na_fila=-1;				// E SAEM DA FILA
				}
			}
			pthread_mutex_unlock(&passageiro_mutex);
			pthread_mutex_unlock(&carro_mutex);
			voltas++;	//VOLTAS SÃO INCREMENTADAS
			estado_carro[threadId] = AGUARDANDO; // VOLTA PARA O INICIO
		}
	}
}


int main (){
	
	pthread_t carro_threads[CARRO_THREADS_NUMBER]; 
	pthread_t passageiros_threads[PASSAGEIROS_THREADS_NUMBER]; 
 
  	int *carro_taskids[CARRO_THREADS_NUMBER];
	int *passageiros_taskids[PASSAGEIROS_THREADS_NUMBER];
	int i,j,u; int t;   
	
	pthread_mutex_init(&carro_mutex,NULL);
	pthread_mutex_init(&passageiro_mutex,NULL);

  	for(t=0; t<PASSAGEIROS_THREADS_NUMBER; t++){
    		passageiros_taskids[t] = (int *) malloc(sizeof(int)); *passageiros_taskids[t] = t;
	 	pthread_create(&passageiros_threads[t],NULL, vida_passageiros , (void *)passageiros_taskids[t]);         
	}

  	for(t=0; t<CARRO_THREADS_NUMBER; t++){
    		carro_taskids[t] = (int *) malloc(sizeof(int)); *carro_taskids[t] = t + PASSAGEIROS_THREADS_NUMBER;
	 	pthread_create(&carro_threads[t],NULL, corrida , (void *)carro_taskids[t]);         
	}

	for(u=0; u<CARRO_THREADS_NUMBER;u++) {
  	  	pthread_join(carro_threads[u], NULL);
  	}
	for(u=0; u<PASSAGEIROS_THREADS_NUMBER;u++) {
  	  	pthread_join(passageiros_threads[u], NULL);
  	}

	printf("\nas voltas acabaram\n\n");

	for(i=0;i < PASSAGEIROS_THREADS_NUMBER;i++){
		printf("passageiro: %2d - voltas dadas: %2d\n", passageiro[i].id ,passageiro[i].n_voltas);
	}
	

	pthread_exit(NULL);

	return 0;	

}












