//YanJi, 21824073
//Compile with g++ -fopenmp project1.cpp
//Results are recorded in the result.txt file. Logs are in the log.txt file.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

//setting: provdie the core number of CPU
static const int core_number = 8;

void readData();
void readKeys();
void getBlocks(int col);
int isNeighbour(int i, int j, int col);
void printCol(int col);
void calcSignatures(int col);
long getOneSignature(int row1, int row2, int row3, int row4);
void setSignature(int col, int index, long value, int one, int two, int three, int four);
long getStartPoint(int col);
void quicksort(long x[], long first, long last);
void allocateMemory(int col);

//static data structures
static double data[500][4400];                    //original data
static long keys[4400];                           //the keys of each row
static const double dia = 0.000001;               // dia
static long *signatures = new long[30000000];     //signatures of all rows in a one-dimensinal array
static long signature_number[500] = {0};          //the total signature number of each column
static long start_point[500] = {0};               //management of the start and end index of each column
static long filled_signature[500] = {0};          //count the already calculated signature number of each column
static int *signatures_one = new int[30000000];   //record the first element of each block
static int *signatures_two = new int[30000000];   //record the second element of each block
static int *signatures_three = new int[30000000]; //record the third element of each block
static int *signatures_four = new int[30000000];  //record the fourth element of each block
static int *correspond_col = new int[30000000];   //record the column number of each block

static int total_col_has_blocks = 0; //total number of columns that have blocks
static long total_block_number = 0;  //total number of blocks in all columns
static long collision_number = 0;    //total collision number
//result and log file
static FILE *result_txt;
static FILE *log_txt;

int main(void)
{
    //control the main work flow
    result_txt = fopen("./result.txt", "w+");
    log_txt = fopen("./log.txt", "w+");
    //read data:
    readData();
    readKeys();
    //omp section start
    omp_set_num_threads(core_number); //set thread number
    int interval = (int)498 / (core_number - 1) + 1;
//start parallel computing to calculate signatures for each column
#pragma omp parallel
    {
#pragma omp for
        for (int col = 499; col >= 0; col--)
        {
            //allocate memory for the blocks of each column
            allocateMemory(col);
        }
#pragma omp barrier
#pragma omp for
        for (int col = 499; col >= 0; col--)
        {
            //calculate signatures for each column
            calcSignatures(col);
        }
    }
    //omp section ends, all signatures are calculated into an array
    printf("%d columns have blocks, total block number is %ld\n", total_col_has_blocks, total_block_number);
    fprintf(log_txt, "%d columns have blocks, total block number is %ld\n", total_col_has_blocks, total_block_number);
    //sorting all signatures
    printf("\nQuick sorting all signatures...\n");
    fprintf(log_txt, "\nQuick sorting all signatures......\n");
    quicksort(signatures, 0, total_block_number);
    printf("Quick sorting finished! \n\nStart collision detecting...\n");
    fprintf(log_txt, "Quick sorting finished! \n\nStart collision detecting...\n");
    //compare sorted signatures, if they are equal then collisions are detected.
    int i = 0;
    while (i < total_block_number)
    {
        if (signatures[i] == signatures[i + 1] && correspond_col[i] != correspond_col[i + 1])
        {
            int last_same_index = i + 1;
            while (signatures[i] == signatures[last_same_index])
            {
                last_same_index += 1;
            }
            collision_number += 1;
            fprintf(result_txt, "Signature %ld -- block: M%d ,M%d, M%d, M%d -- collisions in columns: ", signatures[i], signatures_one[i], signatures_two[i], signatures_three[i], signatures_four[i]);
            for (int j = i; j < last_same_index && (i == j || correspond_col[i] != correspond_col[j]); j++)
            {
                fprintf(result_txt, "%d ", correspond_col[j]);
            }
            fprintf(result_txt, "\n");
            i = last_same_index;
        }
        else
        {
            i += 1;
        }
    }
    printf("%ld collisions are detected.\n", collision_number);
    fprintf(log_txt, "%ld collisions are detected.\n", collision_number);
    printf("\nLogs are recorded in the log.txt file.\n");
    printf("Collisions are recorded in the result.txt file.\n");
    fclose(result_txt);
    fclose(log_txt);
}

void allocateMemory(int col)
{
    //generate neighbours for each row in each col
    int col_neighbours[4400][200];
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
        }
    }
    int index = 0;
    if (exist_neighbours == 1)
    {
        total_col_has_blocks += 1;
        for (int row = 0; row < 4400; row++)
        {
            if (col_neighbours[row][0] >= 0)
            {
                for (int x = 0; col_neighbours[row][x] > 0; x++)
                {
                    for (int y = x + 1; col_neighbours[row][y] > 0; y++)
                    {
                        for (int z = y + 1; col_neighbours[row][z] > 0; z++)
                        {
                            index += 1;
                            total_block_number += 1;
                        }
                    }
                }
            }
        }
        printf("Allocate memory for column %d\n", col);
    }
    signature_number[col] = index;
}

void quicksort(long x[], long first, long last)
{
    // quick sorting the whole signature array with the four elements row index array and the correspond column value
    long pivot, j, temp, i, tempone, temptwo, tempthree, tempfour, tempcol;
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
                temp = x[i];
                tempone = signatures_one[i];
                temptwo = signatures_two[i];
                tempthree = signatures_three[i];
                tempfour = signatures_four[i];
                tempcol = correspond_col[i];
                x[i] = x[j];
                signatures_one[i] = signatures_one[j];
                signatures_two[i] = signatures_two[j];
                signatures_three[i] = signatures_three[j];
                signatures_four[i] = signatures_four[j];
                correspond_col[i] = correspond_col[j];
                x[j] = temp;
                signatures_one[j] = tempone;
                correspond_col[i];
                signatures_two[j] = temptwo;
                signatures_three[j] = tempthree;
                signatures_four[j] = tempfour;
                correspond_col[j] = tempcol;
            }
        }
        temp = x[pivot];
        tempone = signatures_one[pivot];
        temptwo = signatures_two[pivot];
        tempthree = signatures_three[pivot];
        tempfour = signatures_four[pivot];
        tempcol = correspond_col[pivot];
        x[pivot] = x[j];
        signatures_one[pivot] = signatures_one[j];
        signatures_two[pivot] = signatures_two[j];
        signatures_three[pivot] = signatures_three[j];
        signatures_four[pivot] = signatures_four[j];
        correspond_col[pivot] = correspond_col[j];
        x[j] = temp;
        signatures_one[j] = tempone;
        signatures_two[j] = temptwo;
        signatures_three[j] = tempthree;
        signatures_four[j] = tempfour;
        correspond_col[j] = tempcol;
        quicksort(x, first, j - 1);
        quicksort(x, j + 1, last);
    }
}

long getOneSignature(int row1, int row2, int row3, int row4)
{
    return keys[row1] + keys[row2] + keys[row3] + keys[row4];
}

void calcSignatures(int col)
{
    //generate neighbours for each row in each col
    int col_neighbours[4400][200];
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
            fprintf(log_txt, "Col %d row %d find neighbour number %d\n", col, row, index);
        }
    }
    int index = 0;
    if (exist_neighbours == 1)
    {
        for (int row = 0; row < 4400; row++)
        {
            if (col_neighbours[row][0] >= 0)
            {
                for (int x = 0; col_neighbours[row][x] > 0; x++)
                {
                    for (int y = x + 1; col_neighbours[row][y] > 0; y++)
                    {
                        for (int z = y + 1; col_neighbours[row][z] > 0; z++)
                        {
                            long signature = getOneSignature(row, col_neighbours[row][x], col_neighbours[row][y], col_neighbours[row][z]);
                            setSignature(col, -1, signature, row, col_neighbours[row][x], col_neighbours[row][y], col_neighbours[row][z]);
                            index += 1;
                        }
                    }
                }
            }
        }
        printf("Col %d has signatures %d\n", col, index);
        fprintf(log_txt, "Col %d has signatures %d\n", col, index);
    }
    signature_number[col] = index;
}

void setSignature(int col, int index, long value, int one, int two, int three, int four)
{
    //set the at column - col, and the corresponding row infomation of the blocks four element
    long start = getStartPoint(col);
    if (index == -1) //means add a new value
    {
        signatures[start + filled_signature[col]] = value;
        signatures_one[start + filled_signature[col]] = one;
        signatures_two[start + filled_signature[col]] = two;
        signatures_three[start + filled_signature[col]] = three;
        signatures_four[start + filled_signature[col]] = four;
        correspond_col[start + filled_signature[col]] = col;
        filled_signature[col] = filled_signature[col] + 1;
    }
    else
    { //change an exist value
        signatures[start + index] = value;
        signatures_one[start + index] = one;
        signatures_two[start + index] = two;
        signatures_three[start + index] = three;
        signatures_four[start + index] = four;
        correspond_col[start + index] = col;
    }
    // printf("For col %d get start point %ld,setting to %ld,filled signatures is %ld\n", col, start, value, filled_signature[col]);
}

long getStartPoint(int col)
{
    //get the start index of a col
    if (start_point[499] == 0)
    //pre calculate the start point array
    {
        for (int i = 0; i < 500; i++)
        {
            long result = 0;
            for (int j = 0; j < i; j++)
            {
                result += signature_number[j];
            }
            start_point[i] = result;
        }
        return start_point[col];
    }
    else
    //return the index number
    {
        return start_point[col];
    }
}

int isNeighbour(int i, int j, int col)
{
    // check if two elements are within dia/2
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
