/*
 * cipher.c/ Create Cipher
 *
 * Adithya Palle / CS5600 / Northeastern University
 * Spring 2024 / Jan 23, 2024
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "polybius.h"
#include <string.h>



int main(int argc, char* argv[]){
    /**
     * Pass in an option (-e or -d) and a string afterwards to encode(-e) or decode(-d) the string.
    */

    printf("Running tests...\n");
    testCipher();

    table_t table  = {
        {
            {'a','b','c','d','e'},
            {'f','g','h','i','k'},
            {'l','m','n','o','p'},
            {'q','r','s','t','u'},
            {'v','w','x','y','z'}
        }
    }; // hardcoded table


    if (argc < 2){
        printf("Need to past option (-e or -d) and string to encode/decode\n");
        return -1;
    }

    char* option = argv[1];

    char* sentence = malloc(sizeof(char)*1000); // max 1000 characters
    int j = 0;
    for (int i = 2; i < argc ; i++){
        for (int k = 0; argv[i][k] != '\0'; k++){
            sentence[j] = argv[i][k];
            j++;
        }
        sentence[j] = ' ';
        j++;
        
    }

    sentence[j] = '\0';
    sentence = realloc(sentence, sizeof(char)*j);

    if (strcmp(option, "-e") == 0){
        char* encoded = pbEncode(sentence, table);
        printf("Encoding: %s\n", sentence);
        printf("%s\n", encoded);
    }else if (strcmp(option, "-d") == 0){
        char* decoded = pbDecode(sentence, table);
        printf("Decoding: %s\n", sentence);
        printf("%s\n", decoded);
    } else{
        printf("Invalid option: %s\n", option);
        return -1;
    }

}