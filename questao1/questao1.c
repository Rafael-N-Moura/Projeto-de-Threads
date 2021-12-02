#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
long contador = 0;
void *inc(void *threadid)
{
    int i = 0;
    for (; i < 1000000; i++)
    {
        contador++;
        if (contador == 1000000)
        {
            printf("Cheguei\n");
            exit(0);
        }
    }
}
void *dec(void *threadid)
{
    int i = 0;
    for (; i < 9000000; i++)
    {
        contador--;
    }
}
int main(int argc, char *argv[])
{
    pthread_t thread1, thread2, thread3, thread4, thread5, thread6;
    pthread_create(&thread1, NULL, inc, NULL);
    pthread_create(&thread2, NULL, inc, NULL);
    pthread_create(&thread3, NULL, inc, NULL);
    pthread_create(&thread4, NULL, inc, NULL);
    pthread_create(&thread5, NULL, inc, NULL);
    pthread_create(&thread6, NULL, inc, NULL);
    pthread_exit(NULL);
}