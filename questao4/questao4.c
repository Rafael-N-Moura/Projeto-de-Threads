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
    Pixel pixel = *(Pixel *)p;
    int grey = pixel.red * 0.30 + pixel.green * 0.59 + pixel.blue * 0.11;
    for (int i = 0; i < 3; i++)
    {
        resMatrix[pixel.line][i] = grey;
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int l, c;
    FILE *ofile;
    FILE *dfile;
    ofile = fopen("teste1.txt", "r");
    dfile = fopen("destination.txt", "w");
    //Verificação do arquivo
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
    ////////////////////////

    fseek(ofile, sizeof(char) * 3, SEEK_CUR);
    fscanf(ofile, "%d %d", &c, &l);

    l = l * c;

    int **matrix = malloc(sizeof(int *) * l);
    resMatrix = malloc(sizeof(int *) * l);
    pthread_t *threads = malloc(sizeof(pthread_t) * l);

    for (int i = 0; i < l; i++)
    {
        matrix[i] = malloc(sizeof(int) * c);
        resMatrix[i] = malloc(sizeof(int *) * c);
    }

    fscanf(ofile, "\n");

    fseek(ofile, sizeof(int) + sizeof(char), SEEK_CUR);

    for (int i = 0; i < l; i++)
    {
        for (int j = 0; j < c; j++)
        {
            fscanf(ofile, "%d", &matrix[i][j]);
        }
        fscanf(ofile, "\n");
    }

    for (int i = 0; i < l; i++)
    {
        Pixel *pixel = malloc(sizeof(Pixel));
        (*pixel).red = matrix[i][0];
        pixel->green = matrix[i][1];
        pixel->blue = matrix[i][2];
        pixel->line = i;

        pthread_create(&threads[i], NULL, &routine, pixel);
    }

    for (int i = 0; i < l; i++)
    {
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < l; i++)
    {
        for (int j = 0; j < c; j++)
        {
            fprintf(dfile, "%d ", resMatrix[i][j]);
        }
        fprintf(dfile, "\n");
    }

    fclose(ofile);

    fclose(dfile);

    return 0;
}