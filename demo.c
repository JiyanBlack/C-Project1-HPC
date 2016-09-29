
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static double dia=0.000001;

char *paddingWithZero(int num);

int main()
{
    int neighbour_length=611;
    int cur_pair=(int)(neighbour_length*1000/4400.0);
    printf("%d",cur_pair);
}

char *paddingWithZero(int num){
    static char str[4];
    sprintf(str, "%d", num);
    int len=(int)strlen(str);
    printf("%d",len);
    return str;
}