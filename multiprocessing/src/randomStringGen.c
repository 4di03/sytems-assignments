
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// default values for genRand LCG algorihtm
#define c 12345
#define a 1103515245
unsigned long seed = 411241;


int genRand(int start, int end){
/**
 * Generate Random integer in the range of start and end using the 
 * Linear Congruential Generator algorithm with a = 71239 and c = 31014.
 * 
 * 
*/

seed = (a * seed + c) % (end - start);

return  (unsigned int) (seed + start);

}


char* randomString(int length){
    /**
     * Generate a random alphabetical string of length {length}.
    */

   char* string = (char*)malloc(length * sizeof(char));
   for (int i = 0; i < length; i++){
        int randNum  = genRand(97, 123);
       string[i] = (char) randNum;
   }

   return string;

}

char** generateRandomStringsInMemory(int numStrings, int stringLength){
    /**
     * Generate {numStrings} random strings of length {stringLength} and save them to the array of strings.
    */

    char** strings = (char**)malloc(numStrings * sizeof(char*));
    for(int i = 0; i < numStrings; i++){
        strings[i] = randomString(stringLength);
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