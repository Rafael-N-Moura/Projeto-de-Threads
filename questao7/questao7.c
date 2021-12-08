#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define N 2

int globalID = 0;

pthread_mutex_t mutexBuffer;
pthread_mutex_t mutexResults;
pthread_cond_t isEmpty;
pthread_cond_t isEmptyResults;

pthread_t thread[N];
pthread_t despac; //Thread despachante

struct param
{
    int a;
    int b;
};
typedef struct param Param;
//Parametros das funexecs

struct result
{
    int id;
    int result;
    int isDone;
};
typedef struct result Result;
//Elemento do buffer de resultados

//Todo o código para implementar o buffer de Resultados, que foi implementado como uma Lista pois era necessário remover de uma posição arbitrária
typedef struct resultElement *ResultList;

struct resultElement
{
    struct result data;
    struct resultElement *next;
};
typedef struct resultElement ResultElement;

ResultList *results;

ResultList *createRL()
{
    ResultList *rl = (ResultList *)malloc(sizeof(ResultList));
    if (rl != NULL)
        *rl = NULL;

    return rl;
}

void destroyRL(ResultList *rl)
{
    if (rl != NULL)
    {
        ResultElement *node;
        while ((*rl) != NULL)
        {
            node = *rl;
            *rl = (*rl)->next;
            free(node);
        }
        free(rl);
    }
}

int sizeRL(ResultList *rl)
{
    if (rl == NULL)
        return 0;
    int count = 0;
    ResultElement *node = *rl;
    while (node != NULL)
    {
        count++;
        node = node->next;
    }
    return count;
}

int isEmptyRL(ResultList *rl)
{
    if (rl == NULL)
        return 1;
    if (*rl == NULL)
        return 1;

    return 0;
}

int insertRL(ResultList *rl, Result res)
{
    if (rl == NULL)
        return 0;
    ResultElement *node = (ResultElement *)malloc(sizeof(ResultElement));
    if (node == NULL)
        return 0;
    node->data = res;
    node->next = NULL;
    if ((*rl) == NULL)
    {
        *rl = node;
    }
    else
    {
        ResultElement *aux = *rl;
        while (aux->next != NULL)
        {
            aux = aux->next;
        }
        aux->next = node;
    }
    return 1;
}

int removeRL(ResultList *rl, int id)
{
    if (rl == NULL)
        return 0;
    ResultElement *pre;
    ResultElement *node = *rl;
    while (node != NULL && node->data.id != id)
    {
        pre = node;
        node = node->next;
    }
    if (node == NULL)
        return 0;
    if (node == *rl)
        *rl = node->next;
    else
        pre->next = node->next;

    free(node);

    return 1;
}

int getElementRL(ResultList *rl, int id, Result *res)
{
    if (rl == NULL)
        return 0;
    ResultElement *node = *rl;
    while (node != NULL && node->data.id != id)
    {
        node = node->next;
    }
    if (node == NULL)
        return 0;
    else
    {
        *res = node->data;
        return 1;
    }
}

int updateElementRL(ResultList *rl, int id, int isDone, int result)
{
    if (rl == NULL)
        return 0;
    ResultElement *node = *rl;
    while (node != NULL && node->data.id != id)
    {
        node = node->next;
    }
    if (node == NULL)
        return 0;
    else
    {
        node->data.isDone = isDone;
        node->data.result = result;
        return 1;
    }
}
//-----------------------------------------------------------------------------------------------------------------------------------

//Todo o código para implementar o buffer de Resultados, que foi implementado como uma Fila
struct requisicao
{
    void *(*funexec)(void *);
    struct param parametros;
    int id;
};
typedef struct requisicao Requisicao;

typedef struct bufferQueue BufferQueue;

struct bufferQueue
{
    struct bufferElement *start;
    struct bufferElement *end;
};

struct bufferElement
{
    struct requisicao data;
    struct bufferELement *next;
};
typedef struct bufferElement BufferElement;

BufferQueue *buffer;

BufferQueue *createBufferQueue()
{
    BufferQueue *bf = (BufferQueue *)malloc(sizeof(BufferQueue));
    if (bf != NULL)
    {
        bf->end = NULL;
        bf->start = NULL;
    }
    return bf;
}

void destroyBufferQueue(BufferQueue *bf)
{
    if (bf != NULL)
    {
        BufferElement *node;
        while (bf->start != NULL)
        {
            node = bf->start;
            bf->start = bf->start->next;
            free(node);
        }
        free(bf);
    }
}

int sizeBufferQueue(BufferQueue *bf)
{
    if (bf == NULL)
        return 0;
    int count = 0;
    BufferElement *node = bf->start;
    while (node != NULL)
    {
        count++;
        node = node->next;
    }
    return count;
}

int isEmptyBQ(BufferQueue *bf)
{
    if (bf == NULL)
        return 1;
    if (bf->start == NULL)
        return 1;

    return 0;
}

int insertBQ(BufferQueue *bf, Requisicao req)
{
    if (bf == NULL)
        return 0;
    BufferElement *node = (BufferElement *)malloc(sizeof(BufferElement));
    if (node == NULL)
        return 0;
    node->data = req;
    node->next = NULL;
    if (bf->end == NULL)
        bf->start = node;
    else
        bf->end->next = node;

    bf->end = node;

    return 1;
}

int removeBQ(BufferQueue *bf)
{
    if (bf == NULL)
        return 0;
    if (bf->start == NULL)
        return 0;

    BufferElement *node = bf->start;
    bf->start = bf->start->next;
    if (bf->start == NULL)
        bf->end = NULL;

    free(node);

    return 1;
}

int getElementBQ(BufferQueue *bf, Requisicao *req)
{
    if (bf == NULL)
        return 0;
    if (bf->start == NULL)
        return 0;

    *req = bf->start->data;

    return 1;
}
//---------------------------------------------------------------------------------------------------------------------------------------

//Funções Funexecs
void *somar(void *args)
{
    Param param = *(Param *)args;
    int *functionResult = (int *)malloc(sizeof(int));
    *functionResult = param.a + param.b;
    free(args);
    return (void *)functionResult;
}

void *subtrair(void *args)
{
    Param param = *(Param *)args;
    int *functionResult = (int *)malloc(sizeof(int));
    *functionResult = param.a - param.b;
    free(args);
    return (void *)functionResult;
}

void *multiplicar(void *args)
{
    Param param = *(Param *)args;
    int *functionResult = (int *)malloc(sizeof(int));
    *functionResult = param.a * param.b;
    free(args);
    return (void *)functionResult;
}

void *dividir(void *args)
{
    Param param = *(Param *)args;
    int *functionResult = (int *)malloc(sizeof(int));
    *functionResult = param.a / param.b;
    free(args);
    return (void *)functionResult;
}

//----------------------------------------------------------------------------------------------------------------------------------------

int agendarExecucao(void *(*funexec)(void *), Param parametros)
{
    //Garantindo exclusão mutua já que sera preciso adicionar elementos em ambos os buffers
    pthread_mutex_lock(&mutexBuffer);
    pthread_mutex_lock(&mutexResults);

    //Crio uma requisição para guardar o function pointer, os parametros e o ID
    Requisicao req;
    req.funexec = funexec;
    req.parametros = parametros;
    req.id = globalID;
    //Insiro na Fila de requisições
    insertBQ(buffer, req);
    //Considerado que cada requisição terá um resultado, o resultado já é alocado no buffer de resultado mas com o campo
    //isDone igual a 0, o que quer dizer que não está pronto
    Result res;
    res.id = globalID;
    res.isDone = 0;
    insertRL(results, res);
    globalID++;

    pthread_cond_signal(&isEmpty); //Dou um sinal que um elemento foi adiciona no buffer de requisições
    pthread_mutex_unlock(&mutexBuffer);
    pthread_mutex_unlock(&mutexResults);

    return globalID - 1;
}

void *despachante()
{

    while (1)
    {

        pthread_mutex_lock(&mutexBuffer);
        //Não é necessário travar o mutex do buffer de Resultados por que só seria feito o update de um elemento já criado por isso não tem possibilidade de conflito com a função agendarExecução. Já com a função pegarResultadoExecução, caso ela tente pegar um elemento que está sendo modificado e receba uma versão antiga ela simplesmente pegará o valor correto na próxima iteração do while
        int ids[N]; //Nesse buffer eu guardo os ids das requisições que foram mandadas para a execução dessa forma eu consigo saber
        //onde colocar o resultado no buffer de resultados quando a thread retornar da execução

        while (isEmptyBQ(buffer))
        {
            pthread_cond_wait(&isEmpty, &mutexBuffer);
        }                                //Caso o buffer de requisições esteja vazio a thread despachante dorme
        if (sizeBufferQueue(buffer) < N) //Caso tenha menos elementos no buffer do que a quantidade de threads que estão disponíveis para executar, mando para executar somente a quantidade de threads necessárias
        {
            int x = sizeBufferQueue(buffer);
            for (int i = 0; i < x; i++)
            {
                Requisicao req;
                getElementBQ(buffer, &req); //Pego a requisição do buffer, a mais antiga
                ids[i] = req.id;
                if (pthread_create(&thread[i], NULL, req.funexec, &req.parametros))
                {
                    printf("Erro na criacao de uma thread");
                    exit(0);
                }
                removeBQ(buffer); //Removo a mesma requisição do buffer depois de mandar executar
            }
            for (int i = 0; i < x; i++)
            {
                int *res = malloc(sizeof(int));
                pthread_join(thread[i], (void **)&res);
                updateElementRL(results, ids[i], 1, *res);
                //Quando recebo o resulto mudo o campo isDone no buffer de Resultado para 1 e também coloco o resultado lá
            }
        }
        else
        { //Mesma coisa só que caso todas as threads forem necessárias
            for (int i = 0; i < N; i++)
            {
                Requisicao req;
                getElementBQ(buffer, &req);
                ids[i] = req.id;
                if (pthread_create(&thread[i], NULL, req.funexec, &req.parametros))
                {
                    printf("Erro na criacao de uma thread");
                    exit(0);
                }
                removeBQ(buffer);
            }
            for (int i = 0; i < N; i++)
            {
                int *res = malloc(sizeof(int));
                pthread_join(thread[i], (void **)&res);
                updateElementRL(results, ids[i], 1, *res);
                pthread_cond_signal(&isEmptyResults); //Dou um sinal que um resultado foi adicionado no buffer de resultados
                free(res);
            }
        }
        pthread_mutex_unlock(&mutexBuffer);
    }
}

int pegarResultadoExecucao(int id)
{
    while (1)
    {
        pthread_mutex_lock(&mutexResults);
        while (isEmptyRL(results))
        {
            pthread_cond_wait(&isEmptyResults, &mutexResults);
        } //Caso o buffer de resultados esteja vazio a thread dorme
        Result resul;
        resul.isDone = 0;
        if (!getElementRL(results, id, &resul)) //Pego o elemento no buffer de resultados de acordo com o id, se o elemento não for encontrado quer dizer que o id passado é inválido
        {
            printf("O ID nao foi encontrado, possivelmente o resultado dessa requisicao ja foi requisitado, por favor coloque um ID valido\n");
            return -1;
        }
        if (resul.isDone)
        {
            //Se o elemento foi encotrado e o campo isDone não é 0 quer dizer que o resultado está pronto e é retornado
            //Caso isDone seja 0, o usuário ficará bloqueado até a execução ser terminada, caso queira testar é possível colocar um sleep() na thread despachante
            removeRL(results, id);
            pthread_mutex_unlock(&mutexResults);
            return resul.result;
        }
        pthread_mutex_unlock(&mutexResults);
    }
}

int main(int argc, char *argv[])
{
    int opr;
    buffer = createBufferQueue(); //Criando o buffer de requisicoes
    results = createRL();         //Criando o buffer de resultados

    pthread_mutex_init(&mutexBuffer, NULL);
    pthread_mutex_init(&mutexResults, NULL);
    pthread_cond_init(&isEmpty, NULL);
    pthread_cond_init(&isEmptyResults, NULL);

    pthread_create(&despac, NULL, &despachante, NULL);

    Param param;
    int idNow;
    int loop = 1;

    while (loop)
    {
        printf("\n1 - Agendar Adicao\n2 - Agendar Subtracao\n3 - Agendar Multiplicacao\n4 - Agendar Divisao\n5 - Pegar Resultado\n0 - Sair\n");
        scanf("%d", &opr);
        switch (opr)
        {
        case 0:
            loop = 0;
            break;
        case 1:
            printf("Parametro 1: ");
            scanf("%d", &param.a);
            printf("Parametro 2: ");
            scanf("%d", &param.b);
            printf("Agendado com sucesso, o id e: %d\n", agendarExecucao(&somar, param));
            break;
        case 2:
            printf("Parametro 1: ");
            scanf("%d", &param.a);
            printf("Parametro 2: ");
            scanf("%d", &param.b);
            printf("Agendado com sucesso, o id e: %d\n", agendarExecucao(&subtrair, param));
            break;
        case 3:
            printf("Parametro 1: ");
            scanf("%d", &param.a);
            printf("Parametro 2: ");
            scanf("%d", &param.b);
            printf("Agendado com sucesso, o id e: %d\n", agendarExecucao(&multiplicar, param));
            break;
        case 4:
            printf("Parametro 1: ");
            scanf("%d", &param.a);
            printf("Parametro 2: ");
            scanf("%d", &param.b);
            printf("Agendado com sucesso, o id e: %d\n", agendarExecucao(&dividir, param));
            break;
        case 5:
            printf("Digite o id da requisicao: ");
            scanf("%d", &idNow);
            printf("O resultado da requisicao de id %d e = %d\n", idNow, pegarResultadoExecucao(idNow));
            break;

        default:
            break;
        }
    }

    pthread_mutex_destroy(&mutexBuffer);
    pthread_mutex_destroy(&mutexResults);
    pthread_cond_destroy(&isEmpty);
    pthread_cond_destroy(&isEmptyResults);
    destroyBufferQueue(buffer);
    destroyRL(results);

    return 0;
}