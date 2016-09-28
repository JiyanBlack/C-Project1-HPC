
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

void readData();
void readKeys();
void getBlocks(int col);
int isNeighbour(double a, double b);
int getKey(char line_numbers);
double findMin(double array[4400]);
int compFunc(const void *a, const void *b);
void printCol(int col);
int *transBlockString(char block);
int * intdup(int const * src, size_t len);

//define core number below
static const int core_number=8;

static double data[500][4400];
static long keys[4400];
static const double dia = 0.000001;
static int *neighbours[500];    //store the row number of all neighbours in a column
static char *blocks[500];      //blocks[col][array of the rows, e.g:"1 6 7 9","2 4 8 19"]
static long final_result[500]; //final result of the combination of row..

int main(void)
{
    //control the main work flow
    double localSum;

    readData(data);
    readKeys(keys);

    printf("Current dia is %f\n\n", dia);

    omp_set_num_threads(core_number);
#pragma omp parallel firstprivate(localSum)
    {
#pragma omp for
        for (int i = 0; i < 1; i++)
        {
            getBlocks(i);
        }
    }
    
}

void getBlocks(int col)
{
    //generate block lines as a string into the static blocks array
    int col_neighbours[212326];
    int neighbour_index=0;
    for (int i = 0; i < 4400; i++)
    {
        for (int j = i + 1; j < 4400; j++)
        {
            if (isNeighbour(data[col][i], data[col][j]) == 1)
            {
                col_neighbours[neighbour_index]=i;
                col_neighbours[neighbour_index+1]=j;
                printf("%d,%d",i,j);
                neighbour_index+=2;
            }
        }
    }
    printf("col %d has neighbour pairs %d\n",col,neighbour_index+1);
    neighbours[col]=col_neighbours;
}

int * intdup(int const * src, size_t len)
{
   int * p = malloc(len * sizeof(int));
   memcpy(p, src, len * sizeof(int));
   return p;
}

int isNeighbour(double a, double b)
{
    // check if two elements are neighbours
    if (a >= b && a - b < dia)
    {
        return 1;
    }
    else if (b >= a && b - a < dia)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int getKey(char line_numbers)
{
    return 0;
}

int compFunc(const void *a, const void *b)
{
    //computer function of qsort()
    double res = (*(double *)a - *(double *)b);
    if (res > 0.0)
    {
        return 1;
    }
    else if (res == 0.0)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

void readData()
{
    //read data from data.txt
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int line_index = 0;
    int column_index;

    fp = fopen("./data.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1)
    {
        // printf("Retrieved line of length %zu :\n", read);
        // printf("%s", line);
        char *pch;
        column_index = 0;
        pch = strtok(line, " ,");
        while (pch != NULL)
        {
            sscanf(pch, "%lf", &data[column_index][line_index]);
            column_index += 1;
            pch = strtok(NULL, " ,");
        }
        line_index += 1;
    }

    fclose(fp);
    if (line)
        free(line);
}

void readKeys()
{
    //read keys from keys.txt
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int index = 0;

    fp = fopen("./keys.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1)
    {
        // printf("Retrieved line of length %zu :\n", read);
        // printf("%s", line);
        char *pch;
        pch = strtok(line, " ,");
        while (pch != NULL)
        {
            sscanf(pch, "%ld", &keys[index]);
            index += 1;
            pch = strtok(NULL, " ,");
        }
    }
    fclose(fp);
    if (line)
        free(line);
}

void printCol(int col)
{
    //print a single column in data
    printf("Data of column %d\n", col);
    for (int i = 0; i < 4400; i++)
    {
        printf("Row %d, value %f\n", i, data[col][i]);
    }
}

int *transBlockString(char block)
{
    static int one_block[4];
    int index = 0;
    char *pch;
    pch = strtok(&block, " ");
    while (pch != NULL)
    {
        sscanf(pch, "%d", &one_block[index]);
        index += 1;
        pch = strtok(NULL, " ,");
    }
    return one_block;
}
