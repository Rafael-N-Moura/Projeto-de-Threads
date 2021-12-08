#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <math.h>


/*
5 - A marinha recebeu um mapa em formato de matriz com 0 e 1, de tal forma que:
0: água
1: terra
Sua equipe deverá implementar um programa em pthreads para identificar a quantidade de
ilhas no mapa. De forma geral, se duas posições de terra são adjacentes na vertical, na
horizontal, ou na diagonal, elas são da mesma ilha.
*/

//****************************************************************************************************************************************************
// MUDE AS VARIAVEIS AQUI 

#define TAM 10
#define NUM_THREADS 4
int mapa[TAM][TAM] = {{1,1,0,1,0,0,0,1,0,1},
		      {1,1,0,0,1,0,1,1,0,1},
		      {0,0,0,1,0,0,0,0,0,0},
		      {1,1,0,0,0,0,1,1,1,0},
		      {0,0,1,0,0,0,1,0,1,0},
		      {1,0,1,1,0,0,1,1,1,0},
		      {0,0,0,0,0,0,0,0,0,0},
		      {1,0,0,0,1,0,0,1,0,1},
		      {1,0,0,1,1,1,0,1,0,1},
		      {0,0,0,1,1,1,0,1,0,1},};

//***************************************************************************************************************************************************





int size_lines=TAM;   // DEFINE AS DIMENSÕES DO MAPA
int size_columns=TAM;
int sombra[TAM][TAM][3]; // SOMBRA É A MATRIX QUE CONTEM OS PONTOS DO MAPA MAIS A INFORMAÇÃO
			 //  A QUAL CONJUNTO O PONTO PERTENCE


int count=0;       // ESSA VARIAVEL CONTA O NUMERO DE CONJUNTOS
int equal_count=0; // ESSA CONTEM O NUMERO DE CONJUNTOS REPETIDOS

pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;

struct head{       //	CHAMEI DE HEAD / CABEÇA OS INDICES DO PONTO QUE REPRESENTA UM CONJUNTO
	int x,y;
};

struct equal {     // ESSE É PARA COMPARAR QUANDO CONJUNTOS (OU CABEÇAS ) SÃO IGUAIS
    struct head h1;
    struct head h2;
};

struct head heads[TAM*TAM];
struct equal equal_head[TAM*TAM];


void *find_terra(void *tid){ // ESSA É A FUNÇÃO DA THREAD
	
	int i,j,k;
	
	int  threadId = (*(int *)tid);
	int inicio = threadId*(size_lines/(NUM_THREADS));   // INICIO E FIM DEFINE QUAL A REGIAO DO MAPA 
	int fim = (threadId+1)*(size_lines/(NUM_THREADS)+1);// A THREAD VAI ITERAR
	fim++;
	
	for(i=inicio;i<fim;i++){
		for(j=0;j < size_columns;j++){

			if(sombra[i][j][0] ){ // PRIMEIRO É CHECADO SE SE O PONTO NO MAPA
					      // É TERRA ( 1 ) CASO SIM...
				int ii,jj;
				for(ii=(i-1) ; ii<= (i+1) ; ii++){ 	    // ...ESSES LOOPS CHECAM OS VIZINHOS DO PONTO i E j 
					for(jj=(j-1) ; jj<= (j+1) ; jj++){
		
						if(ii==i && jj==j  )continue;
						if(ii<0 || ii>=TAM )continue;
						if(jj<0 || jj>=TAM )continue;

						// NESSE IF É VISTO SE UM VIZINHO É UMA TERRA E SE ELE ESTA EM UM CONJUNTO
						if(sombra[ii][jj][0]    && sombra[ii][jj][1] > -1 && 
						   sombra[ii][jj][2]>-1 && sombra[i][j][1] == -1  &&
						   sombra[i][j][2]==-1 ){  
	
							pthread_mutex_lock(&mymutex);        // CASO SIM O PONTO RECEBERA A MESMA CABEÇA QUE ELE
							sombra[i][j][1]= sombra[ii][jj][1];  // ASSIM PERTENCERAM AO MESMO GRUPO
							sombra[i][j][2]= sombra[ii][jj][2];
							pthread_mutex_unlock(&mymutex); 	
						}
						
						// NESSE IF É TESTADO SE O PONTO ATUAL E O VIZINHO ESTÃO EM CONJUNTOS DIFERENTES
						if(sombra[ii][jj][2] > -1 && sombra[ii][jj][1]>-1 &&
						   sombra[i][j][1]>-1 && sombra[i][j][2]>-1 && 
						   (sombra[i][j][2]!=sombra[ii][jj][2] || sombra[i][j][1]!=sombra[ii][jj][1])){
							
							int k;
							int saida=1;
							for(k=0;k<=equal_count;k++){// ESSE TESTE É UMA PERMUTAÇÃO PARA CHECAR SE OS CONJUNTOS
										    // SÃO REALMENTE DIFERENTES
								if(										
									(equal_head[k].h1.x==sombra[ii][jj][1]&&
									equal_head[k].h1.y==sombra[ii][jj][2]&&
									equal_head[k].h2.x==sombra[i][j][1]&&
									equal_head[k].h2.y==sombra[i][j][2])||
									(equal_head[k].h2.x==sombra[ii][jj][1]&&
									equal_head[k].h2.y==sombra[ii][jj][2]&&
									equal_head[k].h1.x==sombra[i][j][1]&&
									equal_head[k].h1.y==sombra[i][j][2])
								){
									saida=0; // SE FOREM IGUAIS A SAIDA=0 CASO CONTRARIO SAIDA=1
								}
							}
								if(saida){//SE OS CONJUNTOS (OU CABEÇAS) FOREM IGAUIS ...
									pthread_mutex_lock(&mymutex); // ELAS SÃO ADICIONADAS AO GRUPO DE CONJUNTOS IGUAIS
									equal_head[equal_count].h1.x=sombra[ii][jj][1];
									equal_head[equal_count].h1.y=sombra[ii][jj][2];
									equal_head[equal_count].h2.x=sombra[i][j][1];
									equal_head[equal_count].h2.y=sombra[i][j][2];
									equal_count++;
									pthread_mutex_unlock(&mymutex); 
								}
						}
					}
				}
				
				// CASO O PONTO NÃO ENCONTRE NENHUM VIZINHO QUE ESTEJAM EM UM CONJUNTO
				if(sombra[i][j][1]==-1 && sombra[i][j][2]==-1){ // ELE CRIA SEU PROPRIO CONJUNTO
					pthread_mutex_lock(&mymutex); 						
					sombra[i][j][1]= i;			// ONDE SEUS INDICES SÃO A ASSINATURA/NOME/ CABEÇA DO CONJUNTO
					sombra[i][j][2]= j;
			
					heads[count].x=i;
					heads[count].y=j;
					count++;
					pthread_mutex_unlock(&mymutex); 
				}
			}
		}
	}


	// ESSES LOOPS BASICAMENTE CHECAM OS GRUPOS IGUAIS E ESCOLHEM 
	// AQUELE COM OS MENORES INDICES
	for(i=inicio;i<fim;i++){
		for(j=0;j < size_columns;j++){
			if(sombra[i][j][1]!=-1){
				int k;

				if(i<0 || i>=TAM )continue;
				if(j<0 || j>=TAM )continue;
				for(k=0;k<equal_count;k++){
					
					if(sombra[i][j][1]==equal_head[k].h1.x &&
					   sombra[i][j][2]==equal_head[k].h1.y){
						if((equal_head[k].h1.x+equal_head[k].h1.y)<=(equal_head[k].h2.x+equal_head[k].h2.y)){
							pthread_mutex_lock(&mymutex); 			
							sombra[i][j][1]=equal_head[k].h1.x;
							sombra[i][j][2]=equal_head[k].h1.y;
							pthread_mutex_unlock(&mymutex); 
						}if((equal_head[k].h1.x+equal_head[k].h1.y)>(equal_head[k].h2.x+equal_head[k].h2.y)){
							pthread_mutex_lock(&mymutex); 
							sombra[i][j][1]=equal_head[k].h2.x;
							sombra[i][j][2]=equal_head[k].h2.y;
							pthread_mutex_unlock(&mymutex); 
						}			
					}																
				}				
			}
		}			
	}
	
}



int main (){

	count=0;
	equal_count=0;
	
	pthread_t threads[NUM_THREADS]; 
  	int *taskids[NUM_THREADS];
	int i,j,u; int t;   

	// AQUI A SOMBRA É FEITA
	// INICIALMENTE TODOS OS PONTOS TEM CABEÇAS NO VALOR (-1,-1)
	for(i=0;i<size_lines;i++){
		for(j=0;j < size_columns;j++){
			sombra[i][j][0]=mapa[i][j];
			sombra[i][j][1]=-1;
			sombra[i][j][2]=-1;
		}
	}
	
  	for(t=0; t<NUM_THREADS; t++){
    		taskids[t] = (int *) malloc(sizeof(int)); *taskids[t] = t;
	 	pthread_create(&threads[t],NULL,find_terra, (void *)taskids[t]);         
	}
	  for(u=0; u<NUM_THREADS;u++) {
 	  	int *res;
  	  	pthread_join(threads[u], NULL);
  	}   

	// AQUI A MATRIZ FINAL É DESENHADA
  	 
	printf("\n*********************\nnumero de ilhas: %d\n", count-equal_count );
	printf("*********************\nmapa final desenhado:\n\n|");
	for(i=0;i<size_lines;i++){
		for(j=0;j < size_columns;j++){
			if(sombra[i][j][1]!=-1){

					if(sombra[i][j][1]<=9){
						printf(" ");
					}
					printf("%d,",sombra[i][j][1]);
					if(sombra[i][j][2]<=9){
						printf(" ");
					}
					printf("%d|",sombra[i][j][2]);

				
			}else{
				printf("     |");
			}	
		}	
		printf("\n|");
	}
	pthread_exit(NULL);
	return 0;	

}











