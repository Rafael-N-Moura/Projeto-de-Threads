#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
int n_readers,n_writers;                   //quantidade de leitores & escritores
int n,m;
int *buffer=NULL;
int buffer_size=0;
int value; //valor que vai ser escrito no array
int write_random,read_random; // randomico gerado
int counter=0;
//controle de regiao critica
pthread_mutex_t array_buffer=PTHREAD_MUTEX_INITIALIZER;         //controla o acesso ao banco de dados
pthread_mutex_t mutex= PTHREAD_MUTEX_INITIALIZER;     //controla o acesso a variável n_readers
void sleep_escrita(){
	printf("Processo de escrita em andamento\n");
	sleep(4);
}
void sleep_leitura(){
	printf("...Procurando informacoes no banco de dados...\n");
	sleep(5);
}
void Writedb(int id){ //essa funcao eh responsavel por adicionar itens no banco de dados 
	buffer=(int*)realloc(buffer,(buffer_size+1)*sizeof(int));
		if(buffer!=NULL){
			value=rand () % 100;
			buffer_size++;
		    write_random = rand() % buffer_size;
  			printf("...Thread Escritora: %d escrevendo no banco de dados...\n",id);
  			n_writers++;
  			printf("...O array ja foi escrito %d vezes...\n",n_writers);
  			buffer[write_random]=value; //escrevendo algo aleatorio no array...
		}else printf("...A thread nao conseguiu escrever no banco de dados...");
	sleep(5);
}	
void writer(void *tid) {
	int id= (*(int *)tid);
while(1){
    	sleep_escrita();            //so pra mostrar execucao da thread de forma mais bonita (funcao sem muita utilizade)
        pthread_mutex_lock(&array_buffer);    // garantia de que cada thread escreva  no banco de dados de cada vez
        Writedb(id);                   	//escreve novas informacoes na base de dados
		pthread_mutex_unlock(&array_buffer);  //libera o banco de dados p proxima thread
    }
}

 //os sleeps foram adicionados para ver melhor as threads funcionando.
void Readdb(int id){
	//output do processo de leitura
	printf(">> Thread %d: lendo banco de dados. Leitor %d > na posicao: %d\n",id,n_readers,read_random); //remover aki
	sleep(3);
}

void reader(void *tid) {  // a variavel n_readers controla o numero de leitores. O numero de leitores é dado pelo num de threads.
//a  n_readers serve p contar o numero de leitores que ta lendo o buffer no momento.
  int id= (*(int *)tid);
	while (1) {  
		if(buffer_size>0){ // so vai ler algo se o buffer tiver pelo menos 1 elemento
  			pthread_mutex_lock(&mutex);   //down(&mutex); garante acesso exclusivo a variaveln_readersn_readers=n_readers+1;                              //um novo leitor
			read_random=rand()%buffer_size;
			n_readers++;  
      		if(n_readers==1) pthread_mutex_lock(&array_buffer);   //caso este seja o primeiro leitor...
      		pthread_mutex_unlock(&mutex);         //up(&mutex); libera o acesso a variavel n_readers
      		Readdb(id);             //le banco de dados
      		pthread_mutex_lock(&mutex);           // garante acesso exclusivo a variavel n_readers
      		n_readers=n_readers-1;                              //um leitor a menos...
      		if(n_readers==0)pthread_mutex_unlock(&array_buffer);  //caso este seja o ultimo leitor...
      		pthread_mutex_unlock(&mutex);         // libera o acesso a variavel n_readers
      		sleep_leitura();	                      //isso eh usado apenas p melhorar  a visualizaçao da execução
     	}
	}
}

main(){
	int i;
	printf("\nSeja bem vindo ao banco de dados!\n");
	printf("\nDigite o numero de threads escritoras:");
	scanf("%d",&n);
	printf("\nDigite o numero de threads leitoras: ");
	scanf("%d",&m);
	pthread_t escrita_th[n];
	pthread_t leitura_th[m]; 
	int *ID_escrita[n]; 
	int *ID_leitura[m]; 
	
    for(i=0;i<n;i++){
    	ID_escrita[i]=(int *) malloc(sizeof(int)); 
		*ID_escrita[i]=i;
    	pthread_create( &escrita_th[i], NULL,(void *) writer, (void *)ID_escrita[i]);
}

    for(i=0;i<m;i++){
    	ID_leitura[i]=(int *) malloc(sizeof(int)); 
		*ID_leitura[i]=i;
    	pthread_create( &leitura_th[i], NULL,(void *) reader, (void *)ID_leitura[i]);
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
