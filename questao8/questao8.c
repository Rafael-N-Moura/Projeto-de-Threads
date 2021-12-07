#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <math.h>

#define CARRO_THREADS_NUMBER 1
#define PASSAGEIROS_THREADS_NUMBER 20

#define MAX_VOLTAS 10
#define MAX_PASSAGEIROS 10

#define AGUARDANDO 0
#define CORRENDO 1

#define DESCANSANDO 0
#define NA_FILA 1
#define NO_CARRO 2


pthread_mutex_t carro_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t passageiro_mutex = PTHREAD_MUTEX_INITIALIZER;



typedef struct{
	int id;
	int status;
	int lugar_na_fila;
	int n_voltas;
}Passageiro; 

int voltas=0;

int carro_lugar[MAX_PASSAGEIROS];
int estado_carro[CARRO_THREADS_NUMBER];
int lugar_vazio=0;//[CARRO_THREADS_NUMBER];
int ultimo_na_fila=0;

Passageiro passageiro[PASSAGEIROS_THREADS_NUMBER];

void *vida_passageiros(void *tid){
	int threadId =  (*(int *)tid);

	srand((long int) &threadId);

	pthread_mutex_lock(&passageiro_mutex);
	passageiro[threadId/* - CARRO_THREADS_NUMBER*/].id = threadId;
	passageiro[threadId /*- CARRO_THREADS_NUMBER*/].status = rand()%2;
	passageiro[threadId /*- CARRO_THREADS_NUMBER*/].lugar_na_fila  = -1;//ultimo_na_fila;	
	pthread_mutex_unlock(&passageiro_mutex);

	if(passageiro[threadId].status == NA_FILA){
		pthread_mutex_lock(&carro_mutex); 
		passageiro[threadId].lugar_na_fila=ultimo_na_fila;
		ultimo_na_fila++;
		pthread_mutex_unlock(&carro_mutex);
	}
	int this = threadId ;
	int i,j,k;
	
	//***********************************************************************************************************************************
	//printf("criei passageiro %d\nstatus: %d\nlugar na fila: %d\n\n", threadId, passageiro[this].status, passageiro[this].lugar_na_fila );
	while(voltas < MAX_VOLTAS){

		if(passageiro[this].status==DESCANSANDO){
			
			// UM ROTINA DE ESPERA ONDE O PASSAGEIRO "PASSEIA"

			int espera;
			srand((long int )&espera);
			espera = (MAX_VOLTAS*(rand()%100)*passageiro[this].n_voltas);

			for(i=0;i < 110 + espera ;i++){	
				
				for(j=0;j < 10 + espera  ; j++){
				}
			}

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

void *corrida(void *tid){
	int i,j,k;
	int threadId =  (*(int *)tid);

	estado_carro[threadId]=AGUARDANDO;

	while( voltas < MAX_VOLTAS ){
	
		if(estado_carro[threadId] == AGUARDANDO){
			pthread_mutex_lock(&passageiro_mutex); 
			
			for( i=0;i < PASSAGEIROS_THREADS_NUMBER && estado_carro[threadId] != CORRENDO ;i++ ){    // checa se alguem esta na fila
				pthread_mutex_lock(&carro_mutex);
				if (passageiro[i].status == NA_FILA  && passageiro[i].lugar_na_fila <= MAX_PASSAGEIROS   && passageiro[i].lugar_na_fila >= 0){  // checa se o passageiro esta na fila
					
					passageiro[i].status = NO_CARRO;
					printf("- o passageiro %2d entrou no lugar ocupado %2d\n", passageiro[i].id, lugar_vazio);
					passageiro[i].n_voltas++;
					passageiro[i].lugar_na_fila = -1;
					ultimo_na_fila--;    /// diminui o tamanho da fila
					carro_lugar[lugar_vazio] =passageiro[i].id; 
					lugar_vazio++;

					for(j=0; j < PASSAGEIROS_THREADS_NUMBER ;j++){
						if(passageiro[j].status==NA_FILA){
							--passageiro[j].lugar_na_fila;	
						}
					}

					if(lugar_vazio >= MAX_PASSAGEIROS){
						estado_carro[threadId]=CORRENDO;
					} 		
				}
				pthread_mutex_unlock(&carro_mutex);
			}
			pthread_mutex_unlock(&passageiro_mutex); 
		}//else{
	
		if(estado_carro[threadId] == CORRENDO){

				printf("\n***********************************************************************************\n");
				printf("o carro esta correndo a %2d° volta\n\n", voltas);
				for(i=0;i < MAX_PASSAGEIROS;i++){
					printf("lugar - %2d : passageiro - %2d \n" ,i, carro_lugar[i]);
				}
		
				for(i=0;i < 100;i++){
					for(j=0;j < 100 ;j++){
					}
				}
				
				int cansados=0;
				int esperando=0;

				
				for(i=0;i < PASSAGEIROS_THREADS_NUMBER ;i++){
					pthread_mutex_lock(&passageiro_mutex);
					if(passageiro[i].status==NO_CARRO){
						passageiro[i].lugar_na_fila=-1;    //muda lugar na fial
						passageiro[i].status= DESCANSANDO; // muda status para q ele descanse
					}
					pthread_mutex_unlock(&passageiro_mutex);
					if(passageiro[i].status==DESCANSANDO){
						cansados++;
					}
					if(passageiro[i].status==NA_FILA){
						esperando++;
					}
					
				}
				
				
				printf("\nnº de passageiros cansados %2d\n", cansados);
				printf("nº de passageiros na fila %2d\n\n" ,esperando);
				printf("\n**********************************************************************\n\n");
				
				pthread_mutex_lock(&carro_mutex);
				estado_carro[threadId]=AGUARDANDO;
				lugar_vazio=0;
				voltas++;
				pthread_mutex_unlock(&carro_mutex);
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











