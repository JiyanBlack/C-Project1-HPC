
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static double dia = 0.000001;

int isNeighbour(double oneBlock[]);

int main() {
   FILE *fp;
   double x=0.1000001;
    double y=0.10000060;
   double mo=0.0000005;
   if(y-x<dia/2) printf("yes");
   else printf("no");
   return 0;
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
