
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
int *strToArray(char block);
int *intdup(int const *src, size_t len);
void generateBlocks(int col);
void calcNeighbours(int col);

//define core number below
static const int core_number = 8;

static double data[500][4400];
static long keys[4400];
static const double dia = 0.000001;
static int *neighbours[500];      //store the row number of all neighbours in a column
static int neighbour_length[500]; //store the length of all neighbours
static char *blocks[500];         //blocks[col][array of the rows, e.g:"1 6 7 9","2 4 8 19"]
static long final_result[500];    //final result of the combination of row..

int main(void)
{
    //control the main work flow
    double localSum;

    readData(data);
    readKeys(keys);

    printf("\nCurrent dia is %f.\n", dia);
    printf("CPU core number is setted to %d.\n\n", core_number);
    omp_set_num_threads(core_number);
#pragma omp parallel firstprivate(localSum)
    {
#pragma omp for
        for (int col = 0; col < 1; col++)
        {
            getBlocks(col);
        }
    }
}

void getBlocks(int col)
{
    calcNeighbours(col);
    generateBlocks(col);
}

void generateBlocks(int col)
{
    for (int i = 0; i < neighbour_length[col]; i++)
    {
        printf("%d,%d\n", neighbours[col][2 * i], neighbours[col][2 * i + 1]);
    }
}

int *quickSearch(int cur_pair, int isBigger, int result[], int col, int target_row, int result_index)
{
    int result[4400] = {-1};
    //quickly search the target row neighbour
    if (result_index == 0) //means it's the first time running
    {
        //find the rough index that the value would be
        int cur_pair = (int)(neighbour_length[col] * 1000 / 4400.0);
        if (target_row == neighbours[col][2 * cur_pair])
        {
            result[result_index]=neighbours[col][2 * cur_pair+1];
        }
    }
}

void calcNeighbours(int col)
{
    //generate block lines as a string into the static blocks array
    int col_neighbours[212326];
    int neighbour_index = -1;
    for (int i = 0; i < 4400; i++)
    {
        for (int j = i + 1; j < 4400; j++)
        {
            if (isNeighbour(data[col][i], data[col][j]) == 1)
            {
                neighbour_index += 1;
                col_neighbours[2 * neighbour_index] = i;
                col_neighbours[2 * neighbour_index + 1] = j;
                // printf("%d,%d\n", i, j);
            }
        }
    }
    neighbours[col] = col_neighbours;
    neighbour_length[col] = neighbour_index + 1;
    // printf("col %d has neighbour pairs %d\n", col, neighbour_length[col]);
}

int *intdup(int const *src, size_t len)
{
    int *p = malloc(len * sizeof(int));
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

int *strToArray(char block)
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
