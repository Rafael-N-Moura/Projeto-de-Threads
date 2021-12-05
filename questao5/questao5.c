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
		      {1,1,0,1,1,0,1,1,0,1},
		      {0,0,0,1,0,0,0,0,0,0},
		      {1,1,0,0,0,0,1,1,1,0},
		      {0,1,1,0,0,0,1,0,1,0},
		      {1,1,1,1,0,0,1,1,1,0},
		      {1,0,0,0,0,0,0,0,0,0},
		      {1,0,0,0,1,0,0,1,1,1},
		      {1,0,0,1,1,1,0,1,0,0},
		      {0,0,0,1,1,1,0,1,0,1},};

//***************************************************************************************************************************************************





int size_lines=TAM;  
int size_columns=TAM;
int sombra[TAM][TAM][3];


int count=0;
int equal_count=0;

pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;

struct head{
	int x,y;
};

struct equal { 
    struct head h1;
    struct head h2;
};

struct head heads[TAM*TAM];
struct equal equal_head[TAM*TAM];


void *find_terra(void *tid){
	
	int i,j,k;
	
	int  threadId = (*(int *)tid);
	int inicio = threadId*(size_lines/(NUM_THREADS));
	int fim = (threadId+1)*(size_lines/(NUM_THREADS)+1);
	fim++;
	
	for(i=inicio;i<fim;i++){
		for(j=0;j < size_columns;j++){

			if(sombra[i][j][0] ){

				int ii,jj;
				for(ii=(i-1) ; ii<= (i+1) ; ii++){
					for(jj=(j-1) ; jj<= (j+1) ; jj++){
		
						if(ii==i && jj==j  )continue;
						if(ii<0 || ii>=TAM )continue;
						if(jj<0 || jj>=TAM )continue;

						if(sombra[ii][jj][0]    && sombra[ii][jj][1] > -1 &&
						   sombra[ii][jj][2]>-1 && sombra[i][j][1] == -1  &&
						   sombra[i][j][2]==-1 ){
	
							pthread_mutex_lock(&mymutex); 
							sombra[i][j][1]= sombra[ii][jj][1];
							sombra[i][j][2]= sombra[ii][jj][2];
							pthread_mutex_unlock(&mymutex); 	
						}

						if(sombra[ii][jj][2] > -1 && sombra[ii][jj][1]>-1 &&
						   sombra[i][j][1]>-1 && sombra[i][j][2]>-1 && 
						   (sombra[i][j][2]!=sombra[ii][jj][2] || sombra[i][j][1]!=sombra[ii][jj][1])){
							
							int k;
							int saida=1;
							for(k=0;k<=equal_count;k++){
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
									saida=0;
								}
							}
								if(saida){
									pthread_mutex_lock(&mymutex); 
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
				
				if(sombra[i][j][1]==-1 && sombra[i][j][2]==-1){
					pthread_mutex_lock(&mymutex); 					
					sombra[i][j][1]= i;
					sombra[i][j][2]= j;
			
					heads[count].x=i;
					heads[count].y=j;
					count++;
					pthread_mutex_unlock(&mymutex); 
				}
			}
		}
	}

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
	if(threadId == (NUM_THREADS-1))
		printf("\n*********************\nnumero de ilhas: %d\n", count-equal_count );
	
}



int main (){
	
	pthread_t threads[NUM_THREADS]; 
  	int *taskids[NUM_THREADS];
	int i,j,u; int t;   

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

  	 
	printf("*********************\nmapa final desenhado:\n\n");
	for(i=0;i<size_lines;i++){
		for(j=0;j < size_columns;j++){
			if(sombra[i][j][1]!=-1){
				int k;
				for(k=0;k<equal_count;k++){
					
					if(sombra[i][j][1]==equal_head[k].h1.x &&
					   sombra[i][j][2]==equal_head[k].h1.y){
						if((equal_head[k].h1.x+equal_head[k].h1.y)<=(equal_head[k].h2.x+equal_head[k].h2.y)){
							sombra[i][j][1]=equal_head[k].h1.x;
							sombra[i][j][2]=equal_head[k].h1.y;

						}if((equal_head[k].h1.x+equal_head[k].h1.y)>(equal_head[k].h2.x+equal_head[k].h2.y)){
							sombra[i][j][1]=equal_head[k].h2.x;
							sombra[i][j][2]=equal_head[k].h2.y;
						}			
					}					

											
				}
				printf("|%d,%d| ",sombra[i][j][1],sombra[i][j][2]);
			}else{
				printf("|_,_| ");
			}	
		}	
		printf("\n\n");
	}

	pthread_exit(NULL);
	return 0;	

}




























