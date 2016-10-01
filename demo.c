
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static double dia = 0.000001;
void giveValue(int col);
int isNeighbour(double oneBlock[]);
int *demo[10]; //blocks[col][array of the rows, e.g:"1 6 7 9","2 4 8 19"]
int main()
{
    for (int i = 0; i < 10; i++)
    {
        giveValue(i);
    }
    int originA = *(demo[0] + 2);
    int originB = *(demo[0] + 4);
    int temp = *(demo[0] + 2);

    demo[0][2] = *(demo[0] + 4);
    demo[0][4] = temp;
    printf("%d,%d\n%d,%d\n", originA, originB,*(demo[0] + 2),*(demo[0] + 4));
}

void giveValue(int col)
{
    int array[10];
    for (int i = 0; i < 10; i++)
    {
        array[i] = i;
    }
    demo[col] = array;
}

int isNeighbour(double oneBlock[])
{
    // check if two elements are neighbours
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
    if (max - min < 0.1)
        return 1;
    else
        return 0;
}
