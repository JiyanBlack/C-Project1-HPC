
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

void readData();
void readKeys();
int getKey(int line_index);
static double data[500][4400];
static long keys[4400];
int main(void)
{


    readData(data);
    readKeys(keys);
    printf("%f",data[0][0]);
}

void readData(){
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int line_index=0;
    int column_index;

    fp = fopen("./data.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
        // printf("Retrieved line of length %zu :\n", read);
        // printf("%s", line);
        char * pch;
        column_index=0;
        pch = strtok(line," ,");
        while (pch != NULL)
        {
            sscanf(pch, "%lf", &data[column_index][line_index]);
            column_index+=1;
            pch = strtok(NULL, " ,");
        }
        line_index+=1;
    }

    fclose(fp);
    if (line)
        free(line);
}

void readKeys(){
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    int index=0;

    fp = fopen("./keys.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&line, &len, fp)) != -1) {
        // printf("Retrieved line of length %zu :\n", read);
        // printf("%s", line);
        char * pch;
        pch = strtok(line," ,");
        while (pch != NULL)
        {
            sscanf(pch, "%ld", &keys[index]);
            index+=1;
            pch = strtok(NULL, " ,");
        }
    }
    printf("\n\n");
    fclose(fp);
    if (line)
        free(line);
}

int getKey(int line_index){

    return 0;
}