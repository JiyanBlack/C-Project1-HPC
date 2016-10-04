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

//static data structures
static double data[500][4400];
static long keys[4400];
static const double dia = 0.000001;
static long *signatures = new long[29327137];
static long start_point[500] = {0};
static long filled_signature[500] = {0};
static int *signatures_one = new int[29327137];
static int *signatures_two = new int[29327137];
static int *signatures_three = new int[29327137];
static int *signatures_four = new int[29327137];
static int *correspond_col = new int[29327137];
static long final_result[500]; //final resulti of the combination of row
static int total_col_has_neighbours = 0;
static long total_block_number = 0;
static long collision_number = 0;
static long signature_number[500] = {5, 0, 0, 0, 0, 1, 0, 50, 0, 0, 1, 1, 0, 2, 2, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 6, 3, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 3, 1, 962, 1, 0, 0, 0, 0, 4, 0, 1, 1, 1, 1, 0, 1, 0, 0, 3, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 234, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 33, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 38, 73, 61, 0, 0, 0, 0, 85, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 2, 1, 0, 0, 0, 0, 1, 0, 0, 62, 2, 0, 0, 1, 2, 0, 35, 0, 0, 1, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 28, 92, 0, 1, 1, 0, 1, 0, 0, 0, 16, 0, 2, 1, 0, 0, 0, 1, 0, 1, 2, 0, 0, 0, 1, 4, 1, 0, 0, 0, 0, 1, 2, 1, 1, 0, 0, 0, 1, 0, 2, 0, 0, 1, 0, 0, 0, 0, 0, 1268, 0, 1, 0, 69, 0, 0, 28, 1, 0, 0, 0, 5, 0, 2, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 5, 0, 0, 0, 2, 0, 0, 0, 1, 1, 0, 0, 0, 168, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 72, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 2, 0, 0, 1, 1, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 31, 0, 0, 4, 0, 9, 0, 1, 1, 0, 0, 0, 1, 5, 0, 37, 0, 350, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1553, 1, 0, 0, 0, 0, 3, 0, 1, 3, 0, 0, 0, 0, 5, 0, 0, 0, 1, 0, 0, 0, 158, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 17, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 2, 1, 0, 0, 0, 1, 0, 0, 29321366};
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
    omp_set_num_threads(core_number);
    //start parallel computing to calculate signatures for each column
#pragma omp parallel
    {
#pragma omp for
        for (int col = 499; col >= 0; col--)
        {
            calcSignatures(col);
        }
    }
    //omp section ends, all signatures are calculated into an array
    printf("%d columns have blocks, total block number is %ld\n", total_col_has_neighbours, total_block_number);
    fprintf(log_txt, "%d columns have blocks, total block number is %ld\n", total_col_has_neighbours, total_block_number);
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
        total_col_has_neighbours += 1;
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
                            total_block_number += 1;
                        }
                    }
                }
            }
        }
        printf("Col %d has blocks %d\n", col, index);
        fprintf(log_txt, "Col %d has blocks %d\n", col, index);
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
