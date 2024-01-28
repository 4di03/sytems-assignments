
#include <stdio.h>
#include <stdlib.h>
#include "dynblock.h"
#define MAX_HEIGHT 100 // defines max number of lines in file
#define MAX_WIDTH 100 // defines max number characters in a line


dynBlock* allocDynBlock(){
    /**
     * Allocates memory for a dynamic block.
    */
    dynBlock* block = (dynBlock*) malloc(sizeof(dynBlock));

    if (block != NULL){
        block->arr = NULL;
        block->size = 0;
    }else{
        printf("Heap memory could not be allocated for dynBlock");
        exit(1);
    }

    return block;
}

void storeMem2Blk(int* arr, size_t size, dynBlock* block){
    /**
     * Stores an array of integers in a dynamic block.
    */
    block->arr = (int*) malloc(sizeof(int) * size);

    if (block->arr == NULL){
        printf("Heap memory could not be allocated for dynBlock.arr");
        exit(1);
    }

    block->arr = arr;
    block->size = size;

}

void printDynBlock(dynBlock* block){
    /**
     * Prints the contents of a dynamic block.
    */
    for (int i = 0; i < block->size; i++){
        printf("%d ", block->arr[i]);
    }
    printf("\n");
}

int charToInt(char digitChar) {
    /**
     * Converts a character representing a digit to an integer.
    */
    if (digitChar >= '0' && digitChar <= '9') {
        return digitChar - '0';
    } else {
        // Handle error or return a default value
        fprintf(stderr, "Error: Not a valid digit character: %c \n", digitChar);
        exit(1); // Return a default value or indicate an error
    }
}


dynBlock** loadData(char* dataPath){
    /**
     * Loads data from a file into an array of dynamic blocks.
    */
    FILE* f = fopen(dataPath, "r");
    dynBlock** blocks = (dynBlock**) malloc(sizeof(dynBlock*) * MAX_HEIGHT);

    int i = 0;
    char curChar = fgetc(f);

    while(1){
        int* curArr = (int*) malloc(sizeof(int) * MAX_WIDTH);
        int j = 0;
        dynBlock* block = allocDynBlock();

        while (curChar != '\n' && curChar != '\r' && !feof(f)){
            int curNum = 0;
            while (curChar != ' ' && curChar != '\n' && curChar != '\r' && !feof(f)){
                curNum = curNum * 10 + charToInt(curChar);
                curChar = fgetc(f);
            }
            curArr[j] = curNum;
            if (curChar == ' '){
                curChar = fgetc(f);
            }
            j++;
        } 

        curArr = realloc(curArr, sizeof(int) * j);
        storeMem2Blk(curArr, j, block);
        blocks[i] = block;
        i++;

        while (curChar == '\n' || curChar == '\r'){
            curChar = fgetc(f); // for dealing with case where line ends with "\r\n"
        }
        if (feof(f)){
            break;
        }
        
    }

    fclose(f);

    blocks = realloc(blocks, sizeof(dynBlock*) * i);     // resize blocks to fit the number of lines in the file,
    blocks[i] = allocDynBlock();// keep an extra block at the end with size 0 to indicate end of blocks
    blocks[i]->size = -1;
    return blocks;
}
