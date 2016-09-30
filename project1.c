
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

void readData();
void readKeys();
void getBlocks(int col);
int isNeighbour(int i, int j, int col);
double findMin(double array[4400]);
int compFunc(const void *a, const void *b);
void printCol(int col);
int *intdup(int const *src, size_t len);
void getNeighbours(int col);
void calculateSignatures(int col);
long getSignature(int row1, int row2, int row3, int row4);
void runBlock();
void getBlocks499();
void generateBlocks(int col);
void detectCollision();

//setting: provdie the core number of CPU
static const int core_number = 8;

//static data structures
static double data[500][4400];
static long keys[4400];
static const double dia = 0.000001;
static int *neighbours[500][4400]; //blocks[col][array of the rows, e.g:"1 6 7 9","2 4 8 19"]
static int neighbour_number[500];
static int *blocks[500];
static int block_number[500];
static long *signatures[500];  //store the signature of corresponding column
static long final_result[500]; //final result of the combination of row
static int colBlock499[299999999];

// fprintf(fp, "This is testing for fprintf,%d...\n", 10);
static FILE *result_txt;
static FILE *log_txt;

int main(void)
{
    //control the main work flow
    result_txt = fopen("./result.txt", "w+");
    log_txt = fopen("./log.txt", "w+");

    readData();
    readKeys();
    omp_set_num_threads(core_number);
#pragma omp parallel
    {
#pragma omp for
        for (int col = 499; col >= 0; col--)
        {
            getNeighbours(col);
            generateBlocks(col);
            calculateSignatures(col);
        }
#pragma omp barrier
    }
    detectCollision();
    fclose(result_txt);
    fclose(log_txt);
}

void calculateSignatures(int col)
{
    long col_signatures[15836];
    for (int i = 0; i < block_number[col]; i++)
    {
        // col_signatures[i] = getSignature(blocks[col][4 * i], blocks[col][4 * i], blocks[col][4 * i], blocks[col][]);
    }
}

long getSignature(int row1, int row2, int row3, int row4)
{
    return keys[row1] + keys[row2] + keys[row3] + keys[row4];
}

void detectCollision()
{
}

void getNeighbours(int col)
{
    //generate neighbours for each row in each col
    int row_counter = 0;
    for (int i = 0; i < 4400; i++)
    {
        int ele_Neighbours[4400];
        ele_Neighbours[0] = i;
        int ele_index = 1;
        for (int j = i + 1; j < 4400; j++)
        {
            if (isNeighbour(i, j, col) == 1)
            {
                ele_Neighbours[ele_index] = j;
                ele_index += 1;
            }
        }
        if (ele_index >= 4)
        {
            ele_Neighbours[ele_index] = 0;
            neighbours[col][row_counter] = ele_Neighbours;
            row_counter += 1;
        }
    }
    neighbour_number[col] = row_counter;
    if (row_counter > 0)
    {
        // printf("Col %d find block groups %d\n", col, neighbour_number[col]);
        // printf("example output: %d,%d,%d,%d\n", neighbours[col][0][0], neighbours[col][0][1], neighbours[col][0][2], neighbours[col][0][3]);
    }
}

void generateBlocks(int col)
{
    int colBlock[999999];
    colBlock[0] = -1;
    int block_index = 0;
    for (int i = 0; i < neighbour_number[col]; i++)
    {
        for (int x = 1; neighbours[col][i][x] > 0; x++)
        {
            for (int y = x + 1; neighbours[col][i][y] > 0; y++)
            {
                for (int z = y + 1; neighbours[col][i][z] > 0; z++)
                {
                    colBlock[4 * block_index] = neighbours[col][i][0];
                    colBlock[4 * block_index + 1] = neighbours[col][i][x];
                    colBlock[4 * block_index + 2] = neighbours[col][i][y];
                    colBlock[4 * block_index + 3] = neighbours[col][i][z];
                    block_index += 1;
                }
            }
        }
    }
    if (neighbour_number[col] > 0)
    {
        blocks[col] = colBlock;
        block_number[col] = block_index;
        printf("Col %d generate blocks %d\n", col, block_index);
    }
}

int isNeighbour(int i, int j, int col)
{
    // check if two elements are neighbours
    if (data[col][i] >= data[col][j])
    {
        if ((data[col][i] - data[col][j]) < dia / 2)
            return 1;
    }
    else
    {
        if ((data[col][j] - data[col][i]) < dia / 2)
            return 1;
    }
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
