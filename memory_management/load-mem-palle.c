/*
 * load-mem-palle.c / Memory Management
 *
 * Adithya Palle / CS5600 / Northeastern University
 * Spring 2024 / Jan 21, 2024
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include "dynblock.h"
int main(){

    char* dataPath = "blocks.data";

    dynBlock** blocks = loadData(dataPath);

    printf("Printing loaded data:\n");
    int i = 0;
    while (blocks[i]->size != -1){
        printDynBlock(blocks[i]);
        i++;
    }

    printf("Loaded %d dynamic blocks from %s\n", i, dataPath);

    return 0;
}