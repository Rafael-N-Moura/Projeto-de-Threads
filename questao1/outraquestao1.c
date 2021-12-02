#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS 700
long contador = 0;
pthread_mutex_t mutex;

void *PrintHello(void *threadid)
{
    int tid = *((int *)threadid);
    int i = 0;
    for (; i < 1000000; i++)
    {
        pthread_mutex_lock(&mutex);
        contador++;
        if (contador == 1000000)
        {
            printf("Cheguei, sou a thread #%d\n", tid);
            exit(0);
        }
        pthread_mutex_unlock(&mutex);
    }
}
int main(int argc, char *argv[])
{
    pthread_mutex_init(&mutex, NULL);
    pthread_t threads[NUM_THREADS];
    int *taskids[NUM_THREADS];

    int rc;
    int t;
    for (t = 0; t < NUM_THREADS; t++)
    {
        taskids[t] = (int *)malloc(sizeof(int));
        *taskids[t] = t;
        //printf("No main: criando thread %d\n", t);
        rc = pthread_create(&threads[t], NULL, PrintHello, (void *)taskids[t]);
        if (rc)
        {
            printf("ERRO; código de retorno é %d\n", rc);
            exit(-1);
        }
    }
    for (t = 0; t < NUM_THREADS; t++)
    {
        pthread_join(threads[t], NULL);
    }

    pthread_mutex_destroy(&mutex);

    pthread_exit(NULL);
}