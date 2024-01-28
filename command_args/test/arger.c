/*
 * arger.c / Command Line Arguments
 *
 * Adithya Palle / CS5600 / Northeastern University
 * Spring 2024 / Jan 21, 2024
 *
 */
#include <stdio.h>
#include "string_util.h"

int main(int argc, char* argv[]){
    if (argc < 2){
        printf("Please provide more command line arguments!\n");
        return -1;
    } 
    if (stringEquals(argv[1], "-u")){  
        applyToSentence(upper, &argv[2], argc-2);

    }else if (stringEquals(argv[1], "-l")){
        applyToSentence(lower, &argv[2], argc-2);
    }
    else if (stringEquals(argv[1], "-cap")){
        applyToSentence(capitalize, &argv[2], argc-2);
    }
    else{
        printf("Please provide a valid first argument, it must be one of -u, -l, or -cap\n");
        return -1;
    }
    

    return 0;
}