
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    char str[] = "12345.56\n";
    double d;
    sscanf(str, "%lf", &d);
    printf("%lf", d);
}
