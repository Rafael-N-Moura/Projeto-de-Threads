#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int **resMatrix;
typedef struct
{
    int red;
    int blue;
    int green;
    int line;
} Pixel;

void *routine(void *p)
{
    //Exclusão mútua não é necessária pois cada thread cuida de um pixel diferente
    Pixel pixel = *(Pixel *)p;
    int grey = pixel.red * 0.30 + pixel.green * 0.59 + pixel.blue * 0.11; //Realizando a conta
    for (int i = 0; i < 3; i++)
    {
        resMatrix[pixel.line][i] = grey; //Colocando o valor na respectiva linha
    }
    free(p);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int l, c;
    FILE *ofile; //Arquivo de origem
    FILE *dfile; //Arquivo destino
    int limite;

    ofile = fopen("teste1.txt", "r"); //Mude o nome de acordo com o arquvio testa que está testando
    dfile = fopen("destination1.txt", "w");

    //Verificação dos arquivos
    if (ofile == NULL)
    {
        printf("Falha na abertura do arquivo\n");
        exit(1);
    }
    if (dfile == NULL)
    {
        printf("Falha na abertura do arquivo\n");
        exit(1);
    }

    fseek(ofile, sizeof(char) * 3, SEEK_CUR); //Acredito que a primeira informação não seja útil nesse caso já que o arquivo serdo tipo P3
    //já é uma premissa da questão, então o cursor foi movido para a próxima linha
    fscanf(ofile, "%d %d", &c, &l); //Obtendo as dimensões

    l = l * c; //As dimensões obtidas consideram os pixels como um elemento mas a matriz real passada no arquivo trata cada pixel como uma
    //linha, então a quantidade de linhas é na verdade linhas * colunas

    int **matrix = malloc(sizeof(int *) * l);           //Alocando a matrix para a leitura do arquivo
    resMatrix = malloc(sizeof(int *) * l);              //Alocando a matrix de resultados que será escrita no arquivo destino
    pthread_t *threads = malloc(sizeof(pthread_t) * l); //Alocando as threads, considerando que teremos uma thread para cada pixel

    for (int i = 0; i < l; i++)
    {
        matrix[i] = malloc(sizeof(int) * c);
        resMatrix[i] = malloc(sizeof(int *) * c);
    }

    fscanf(ofile, "\n");

    fscanf(ofile, "%d", &limite);
    fscanf(ofile, "\n");

    //Lendo a matriz do arquivo
    for (int i = 0; i < l; i++)
    {
        for (int j = 0; j < c; j++)
        {
            fscanf(ofile, "%d", &matrix[i][j]);
            //Caso o valor seja maior que o limite
            if (matrix[i][j] > limite)
                matrix[i][j] = limite;
        }
        fscanf(ofile, "\n");
    }

    //Pegando cada linha, que notadamente é um pixel, colocando suas informações dentro de um elemento Pixel e criando uma thread
    //que irá fazer os calculos para esse elemento
    for (int i = 0; i < l; i++)
    {
        //Cada pixel é alocado na memória pois caso foi declarado estaticamente seus valores mudariam a cada iteração do for
        //e haveria um conflito na execução
        Pixel *pixel = malloc(sizeof(Pixel));
        (*pixel).red = matrix[i][0];
        pixel->green = matrix[i][1];
        pixel->blue = matrix[i][2];
        pixel->line = i;
        //A linha é guardada para que seja possível realizar a escrita na posição certa posteriormente
        pthread_create(&threads[i], NULL, &routine, pixel);
    }

    for (int i = 0; i < l; i++)
    {
        pthread_join(threads[i], NULL);
        //Esperando cada thread finalizar sua execução
    }

    for (int i = 0; i < l; i++)
    {
        for (int j = 0; j < c; j++)
        {
            fprintf(dfile, "%d ", resMatrix[i][j]);
            //Escrevendo no arquivo destino
        }
        fprintf(dfile, "\n");
    }

    for (int i = 0; i < l; i++)
    {
        free(matrix[i]);
        free(resMatrix[i]);
    }

    free(matrix);
    free(resMatrix);
    free(threads);

    fclose(ofile);
    fclose(dfile);

    return 0;
}