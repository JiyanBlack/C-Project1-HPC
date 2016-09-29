
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

void readData();
void readKeys();
void getBlocks(int col);
int isBlock(int i, int j, int m, int n, int col);
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
static int *blocks[500];       //blocks[col][array of the rows, e.g:"1 6 7 9","2 4 8 19"]
static int block_number[500];  //the number of block in each column
static long final_result[500]; //final result of the combination of row

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
        for (int col = 0; col < 500; col++)
        {
            getBlocks(col);
        }
    }
}

void getBlocks(int col)
{
    //generate block lines as a string into the static blocks array
    int oneColBlock[4004];
    int index = 0;
    for (int i = 0; i < 4400; i++)
    {
        for (int j = i + 1; j < 4400; j++)
        {
            if (isBlock(i, j, -1, -1, col) == 1)
            {
                // printf("%d,%d\n", i, j);
                for (int m = j + 1; m < 4400; m++)
                {
                    if (isBlock(i, j, m, -1, col) == 1)
                    {
                        // printf("%d,%d,%d\n", i, j, m);
                        for (int n = m + 1; n < 4400; n++)
                        {
                            if (isBlock(i, j, m, n, col) == 1)
                            {
                                // printf("Find block at col %d: %d, %d, %d, %d.\n", col, i, j, m, n);
                                oneColBlock[index] = i;
                                oneColBlock[index + 1] = j;
                                oneColBlock[index + 2] = m;
                                oneColBlock[index + 3] = n;
                                index += 4;
                            }
                        }
                    }
                }
            }
        }
    }
    block_number[col] = index / 4;
    if (index > 0)
        printf("Col %d has blocks number: %d\n", col, block_number[col]);
}

int isBlock(int i, int j, int m, int n, int col)
{
    // check if two elements are neighbours
    double oneBlock[4] = {-1.0};
    oneBlock[0] = data[col][i];
    oneBlock[1] = data[col][j];
    if (m >= 0)
        oneBlock[2] = data[col][m];
    if (n >= 0)
        oneBlock[3] = data[col][n];

    double min = oneBlock[0];
    double max = oneBlock[0];
    for (int i = 1; i < 4; i++)
    {
        if (oneBlock[i] > -0.1)
        {
            if (min > oneBlock[i])
            {
                min = oneBlock[i];
            }
            if (max < oneBlock[i])
            {
                max = oneBlock[i];
            }
        }
    }
    if (max - min < dia)
        return 1;
    else
        return 0;
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
