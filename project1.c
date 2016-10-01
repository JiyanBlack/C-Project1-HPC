
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
void recordNeighbours();
int isSearched(long signature);
void quicksort(long x[], long first, long last);
//setting: provdie the core number of CPU
static const int core_number = 8;

//static data structures
static double data[500][4400];
static long keys[4400];
static const double dia = 0.000001;
static long *signatures[500];
static long signature_number[500] = {0};
static int *signatures_one[500];
static int *signatures_two[500];
static int *signatures_three[500];
static int *signatures_four[500];
static long final_result[500]; //final resulti of the combination of row
static int total_col_has_neighbours = 0;
static long *finished_signatures = new long[30000000];

// fprintf(fp, "This is testing for fprintf,%d...\n", 10);
static FILE *result_txt;
static FILE *log_txt;
static FILE *neighbours_txt;

int main(void)
{
    //control the main work flow
    result_txt = fopen("./result.txt", "w+");
    log_txt = fopen("./log.txt", "w+");
    neighbours_txt = fopen("./neighbours.txt", "w+");

    readData();
    readKeys();
    omp_set_num_threads(core_number);
#pragma omp parallel
    {
#pragma omp for
        for (int col = 499; col > 498; col--)
        {
            getNeighbours(col);
        }
    }
    quicksort(signatures[499], 0, signature_number[499]);
    printf("Total %d columns that have neighbours\n", total_col_has_neighbours);
#pragma omp parallel
    {
#pragma omp for
        for (int col = 0; col < 499; col++)
        {
            for (long index = 0; index < signature_number[col]; index++)
            {
                long target_signature = *(signatures[col] + index);
                for (int col2 = col + 1; col2 < 499; col2++)
                {
                    for (long index2 = 0; index2 < signature_number[col2]; index2++)
                    {
                        long compared_signature = *(signatures[col2] + index2);
                        if (target_signature == compared_signature)
                        {
                            // printf("col %d and %d has collision: %ld, %ld\n", col, col2, target_signature, compared_signature);
                        }
                    }
                }
            }
        }
    }
    fclose(result_txt);
    fclose(log_txt);
    fclose(neighbours_txt);
}

//quickSort in C:
void quicksort(long x[], long first, long last)
{
    // printf("sorting");
    long pivot, j, temp, i, tempone, temptwo, tempthree, tempfour;
    if (first < last)
    {
        pivot = first;
        i = first;
        j = last;
        while (i < j)
        {
            while (x[i] <= x[pivot] && i < last)
                i++;
            while (x[j] > x[pivot])
                j--;
            if (i < j)
            {
                tempone = *(signatures_one[499] + i);
                temptwo = *(signatures_two[499] + i);
                tempthree = *(signatures_three[499] + i);
                tempfour = *(signatures_four[499] + i);
                temp = x[i];
                signatures_one[499][i] = signatures_one[499][j];
                signatures_two[499][i] = signatures_two[499][j];
                signatures_three[499][i] = signatures_three[499][j];
                signatures_four[499][i] = signatures_four[499][j];
                x[i] = x[j];
                signatures_one[499][j] = tempone;
                signatures_two[499][j] = temptwo;
                signatures_three[499][j] = tempthree;
                signatures_four[499][j] = tempfour;
                x[j] = temp;
            }
        }
        tempone = *(signatures_one[499] + pivot);
        temptwo = *(signatures_two[499] + pivot);
        tempthree = *(signatures_three[499] + pivot);
        tempfour = *(signatures_four[499] + pivot);
        temp = x[pivot];
        signatures_one[499][pivot] = signatures_one[499][j];
        signatures_two[499][pivot] = signatures_two[499][j];
        signatures_three[499][pivot] = signatures_three[499][j];
        signatures_four[499][pivot] = signatures_four[499][j];
        x[pivot] = x[j];
        signatures_one[499][j] = tempone;
        signatures_two[499][j] = temptwo;
        signatures_three[499][j] = tempthree;
        signatures_four[499][j] = tempfour;
        x[j] = temp;
        quicksort(x, first, j - 1);
        quicksort(x, j + 1, last);
    }
}

int isSearched(long signature)
{
    for (long i = 0; i < 30000000; i++)
    {
        if (finished_signatures[i] == signature)
        {
            return 1;
        }
        if (finished_signatures[i] < 100000)
        {
            break;
        }
    }
    return 0;
}

long getSignature(int row1, int row2, int row3, int row4)
{
    return keys[row1] + keys[row2] + keys[row3] + keys[row4];
}

void getNeighbours(int col)
{
    //generate neighbours for each row in each col
    int col_blocks[9000];
    int col_neighbours[4400][200];
    long *col_signatures = new long[30000000];
    int *col_signatures_one = new int[30000000];
    int *col_signatures_two = new int[30000000];
    int *col_signatures_three = new int[30000000];
    int *col_signatures_four = new int[30000000];

    int exist_neighbours = 0;
    for (int row = 0; row < 4400; row++)
    {
        col_neighbours[row][0] = -2;
        int index = 0;
        for (int x = row + 1; x < 4400; x++)
        {
            if (isNeighbour(row, x, col) == 1)
            {
                col_neighbours[row][index] = x;
                index += 1;
            }
        }
        if (index < 3)
        {
            col_neighbours[row][0] = -2;
        }
        else
        {
            col_neighbours[row][index] = -2;
            col_neighbours[row][index + 1] = -2;
            col_neighbours[row][index + 2] = -2;
            exist_neighbours = 1;
            fprintf(log_txt, "Col %d row %d find neighbours %d\n", col, row, index);
            // printf("Col %d row %d find neighbours %d\n", col, row, index);
        }
    }
    if (exist_neighbours == 1)
    {
        total_col_has_neighbours += 1;
        int index = 0;
        for (int row = 0; row < 4400; row++)
        {
            if (col_neighbours[row][0] >= 0)
            {
                for (int x = 0; col_neighbours[row][x] > 0; x++)
                {
                    if (col_neighbours[row][x] < 0)
                        break;
                    for (int y = x + 1; col_neighbours[row][y] > 0; y++)
                    {
                        if (col_neighbours[row][y] < 0)
                            break;
                        for (int z = y + 1; col_neighbours[row][z] > 0; z++)
                        {
                            if (col_neighbours[row][z] < 0)
                                break;
                            long singature = getSignature(row, col_neighbours[row][x], col_neighbours[row][y], col_neighbours[row][z]);
                            col_signatures[index] = singature;
                            col_signatures_one[index] = row;
                            col_signatures_two[index] = col_neighbours[row][x];
                            col_signatures_three[index] = col_neighbours[row][y];
                            col_signatures_four[index] = col_neighbours[row][z];
                            index += 1;
                        }
                    }
                }
            }
        }
        signature_number[col] = index;
        printf("Col %d has blocks %d\n", col, index);
    }
    signatures[col] = col_signatures;
    signatures_one[col]=col_signatures_one;
    signatures_two[col]=col_signatures_two;
    signatures_three[col]=col_signatures_three;
    signatures_four[col]=col_signatures_four;
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
