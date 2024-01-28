
#include <stdio.h>
#include <stdlib.h>
#include "randNums.h"
// default values for genRand LCG algorihtm
#define c 10297
#define a 72153
int seed = 41;

int genRand(int start, int end){
/**
 * Generate Random integer in the range of start and end using the 
 * Linear Congruential Generator algorithm with a = 71239 and c = 31014.
 * 
 * 
*/

seed = (a * seed + c) % end;

return seed;

}


int charToInt(char digitChar) {
    /**
     * Converts a character representing a digit to an integer.
     * 
    */
    if (digitChar >= '0' && digitChar <= '9') {
        return digitChar - '0';
    } else {
        // Handle error or return a default value
        fprintf(stderr, "Error: Not a valid digit character: %c \n", digitChar);
        exit(1); // Return a default value or indicate an error
    }
}
