#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
// output: Espera as n threads escreverem e faz a leitura sequencial do buffer 
pthread_mutex_t array_buffer;         //controla o acesso ao banco de dados
pthread_mutex_t mutex;      //controla o acesso a variável n_readers
int n_readers;                   //quantidade de leitores lendo
int *buffer=NULL;
int buffer_size=0;
int indice=0;

void sleep_escrita(){
	printf("Processo de escrita em andamento\n");
	sleep(4);
}
void sleep_leitura(){
	printf("\n...Procurando informacoes no banco de dados...\n");
	sleep(5);
}
void Writedb(int id){ //essa funcao eh responsavel por adicionar itens no banco de dados 

	int value = rand() % 100;/*valor que vai ser escrito no array*/
	buffer=(int*)realloc(buffer,(buffer_size+1)*sizeof(int));
		if(buffer!=NULL){
  			printf("...Thread Escritora: %d escrevendo no banco de dados...\n",id);
  			buffer[buffer_size]=value; //escrevendo algo aleatorio no array...
  			buffer_size++;
		}else printf("...A thread nao conseguiu escrever no banco de dados...");
	sleep(5);
}	
void writer(void *tid) {
	int id= (*(int *)tid);
while(1)            
    {
    sleep_escrita();            //pensa em informações para adicionar a base de dados
        pthread_mutex_lock(&array_buffer);    //down(&buffer); garante acesso exclusivo a base de dados
        Writedb(id);          //escreve novas informacoes na base de dados

        pthread_mutex_unlock(&array_buffer);  //up(&array_buffer); libera o acesso a base de dados
    }
}

 //os sleeps foram adicionados para ver melhor as threads funcionando.
void Readdb(int id){
	//quantidade de leitores lendo atualmente
	printf("Thread %d: lendo banco de dados. Total de %d leitores lendo agora. A thread leu a posicao: %d\n",id,n_readers,indice); //remover aki
	sleep(3);
}

void reader(void *tid) {  // a variavel n_readers controla o numero de leitores. O numero de leitores é dado pelo num de threads.
//a variavel n_readers serve p cntar o numero de leitores que ta lendo o buffer no momento.
  int id= (*(int *)tid);//
while (1) {  
	if(buffer_size>0){ // so vai ler algo se o buffer tiver pelo menos 1 elemento
  		pthread_mutex_lock(&mutex);           //down(&mutex); garante acesso exclusivo a variaveln_readers
      	n_readers=n_readers+1;                              //um novo leitor
	  	indice++;		
      	if(n_readers==1) pthread_mutex_lock(&array_buffer);   //caso este seja o primeiro leitor...
      	pthread_mutex_unlock(&mutex);         //up(&mutex); libera o acesso a variavel n_readers
	  
      	Readdb(id); //                    //le banco de dados
      	pthread_mutex_lock(&mutex);           //down(&mutex); garante acesso exclusivo a variavel n_readers
      	n_readers=n_readers-1;                              //um leitor a menos...

      	if(n_readers==0) pthread_mutex_unlock(&array_buffer);  //caso este seja o ultimo leitor...
      	pthread_mutex_unlock(&mutex);         //up(&mutex); libera o acesso a variavel n_readers

      	sleep_leitura();                      //isso eh usado apenas p melhorar  a visualizaçao da execução
     	}
	}
}

main(){
	int n,m,i;
	printf("\nSeja bem vindo ao banco de dados!\n");
	printf("\nDigite o numero de threads escritoras:");
	scanf("%d",&n);
	printf("\nDigite o numero de threads leitoras: ");
	scanf("%d",&m);
	pthread_t escrita_th[n];
	pthread_t leitura_th[m]; 
	int *ID_escrita[n]; //
	int *ID_leitura[m]; //

//inicializacao dos semaforos...
	pthread_mutex_init(&array_buffer, NULL);
	pthread_mutex_init(&mutex, NULL);


//criação das threads  de escritores...
    for(i=0;i<n;i++){//
    	ID_escrita[i]=(int *) malloc(sizeof(int)); //
		*ID_escrita[i]=i;//
    	pthread_create( &escrita_th[i], NULL,(void *) writer, (void *)ID_escrita[i]);//
}

//criação das threads  de leitores...
    for(i=0;i<m;i++){
    	ID_leitura[i]=(int *) malloc(sizeof(int)); //
		*ID_leitura[i]=i;//
    	pthread_create( &leitura_th[i], NULL,(void *) reader, (void *)ID_leitura[i]);//
	}


	for(i=0;i<n;i++){
    	pthread_join(escrita_th[i], NULL);
	}		

	for(i=0;i<m;i++){
    pthread_join(leitura_th[i], NULL);
	}
	pthread_exit(NULL);
	free(buffer);
	return 0;
}
