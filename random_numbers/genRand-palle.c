/*
 * gen-rand-palle.c / Memory Management
 *
 * Adithya Palle / CS5600 / Northeastern University
 * Spring 2024 / Jan 21, 2024
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "randNums.h"
#define START_RANGE 0
#define END_RANGE 1000


int main(int argc , char* argv[]){
    if (argc < 3){
        printf("Previous provide at least 2 command line arguments, a number and a file name, \nand optionally -a if you want to append rather than overwrite.\ncl");
        return -1;
    }

    int n =0;
    for (int i=0; i<strlen(argv[1]); i++){
        n = n*10 + charToInt(argv[1][i]);
    }

    char* filename = argv[2];

    char mode = 'w';

    if (argc == 4 && strcmp(argv[3] , "-a") == 0){


        mode = 'a'; // append in this case
    }

    FILE *f = fopen(filename, &mode);


    genRand(START_RANGE,END_RANGE);


    for (int i=0; i<n; i++){
        fprintf(f, "%d\n", genRand(START_RANGE, END_RANGE));
    }

    fclose(f);

}