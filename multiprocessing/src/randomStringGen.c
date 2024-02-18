
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// default values for genRand LCG algorihtm, using defaults values from GCC
#define c 32145
#define a 1103515245
#define m 2147483648
#define NUM_VALID_RAND_NUMS 123 - 97 + 3

int validRandNums[NUM_VALID_RAND_NUMS];
int numValidRandNums;
unsigned long seed = 411241;

void initializeValidRandNums() {
    /**
     * Initialize the valid random numbers array.
    */
    int j = 0;
    for (int i = 97; i < 123; i++) {
        validRandNums[j++] = i;
    }
    validRandNums[j++] = 33; // !
    validRandNums[j++] = 63; // ?
    validRandNums[j++] = 46; // .

    numValidRandNums = NUM_VALID_RAND_NUMS;
}



int genRand(int start, int end){
/**
 * Generate Random integer in the range of start and end using the 
 * Linear Congruential Generator algorithm with a = 71239 and c = 31014.
 * 
 * 
*/

int range = end-start;
seed = (a * seed + c) % m;

return  (int) (seed %(range) + start);

}



char* randomString(int length){
    /**
     * Generate a random alphabetical string of length {length}.
    */
   if (numValidRandNums == 0){ // initialize validRandNums if it has not been initialized
       initializeValidRandNums();
   }

   char* string = (char*)malloc((length + 1)* sizeof(char));


    int i;
   for (i = 0; i < length; i++){
        int randIdx  = genRand(0, numValidRandNums);
        string[i] = (char) validRandNums[randIdx];
   }

   string[i] = '\0';
   return string;

}

char** generateRandomStringsInMemory(int numStrings, int stringLength){
    /**
     * Generate {numStrings} random strings of length {stringLength} and save them to the array of strings.
    */

    char** strings = (char**)malloc(numStrings * sizeof(char*));
    for(int i = 0; i < numStrings; i++){
        char* randString = randomString(stringLength);
        strings[i] = randString;
    }
    return strings;
    
}

void generateRandomStrings(int numStrings, int stringLength, char* saveFilePath){
    /**
     * Generate {numStrings} random strings of length {stringLength} and save them to the file at {saveFilePath}.
    */
    
    char** strings = generateRandomStringsInMemory(numStrings, stringLength);
    FILE* file = fopen(saveFilePath, "w");

    for (int i = 0; i < numStrings; i++){
        fprintf(file, "%s\n", strings[i]);
    }
    fclose(file);
}