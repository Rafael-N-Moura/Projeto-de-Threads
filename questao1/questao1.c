#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS 700

long contador = 0;
pthread_mutex_t mutex;

void *contar(void *threadid)
{
    int tid = *((int *)threadid);

    for (int i = 0; i < 1000000; i++)
    {
        //Exclusão mútua é necessária já que o contador é uma região crítica e se o mutex não estivesse presente
        //haveria um conflito e a contagem seria feita de forma errada.
        pthread_mutex_lock(&mutex);
        contador++;
        if (contador == 1000000)
        {
            printf("Cheguei, sou a thread #%d\n", tid);
            free(threadid);
            exit(0);
        }
        pthread_mutex_unlock(&mutex);
    }
}
int main(int argc, char *argv[])
{
    pthread_mutex_init(&mutex, NULL); // Iniciando mutex
    pthread_t threads[NUM_THREADS];

    int t;
    for (t = 0; t < NUM_THREADS; t++)
    {
        int *a = (int *)malloc(sizeof(int));
        *a = t;
        //Se passasse diretamente o endereço de t, o resultado não seria o esperado pois t mudaria continuamente
        if (pthread_create(&threads[t], NULL, &contar, a))
        {
            printf("ERRO");
            exit(-1);
        }
    }
    for (t = 0; t < NUM_THREADS; t++)
    {

        if (pthread_join(threads[t], NULL))
        {
            printf("ERRO");
            exit(-1);
        }
    }

    pthread_mutex_destroy(&mutex); //Destruindo mutex

    pthread_exit(NULL);
}