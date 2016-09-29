
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static double dia = 0.000001;

int isNeighbour(double oneBlock[]);

int main()
{
    double oneBlock[4] = {1.11, 1.12, 1.14, -1};
    if (isNeighbour(oneBlock))
        printf("Yes!");
    else
        printf("No...");
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
