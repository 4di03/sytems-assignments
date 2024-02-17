#include "queue.h"
#include <stdlib.h>
#include <stdio.h>
#include "polybius.h"


static table_t shuffledTable = {
        {{'f', 'x', 'a', 'm', 'c'},
         {'i', 'd', 'b', 'g', 'e'},
         {'k', 'y', 'h', 'n', 'o'},
         {'p', 'q', 'w', 's', 't'},
         {'u', 'v', 'r', 'l', 'z'}}};


int arrayLen(void** arr){
    int i = 0;
    while (arr[i] != NULL){
        i++;
    }
    return i;
}

char** encrypt(char** word){
    /**
     * Encrypts the words in the array {word} and returns the encrypted words in a new array.
    */

    int numWords = arrayLen(word);

    char** encrypted = malloc(sizeof(char*) * numWords);
    for (int i = 0; i < 10; i++){
        encrypted[i] = pbEncode(word[i], shuffledTable);
    }
    return encrypted;
}

queue_t* readStringsFromFile(char* fileLoc){
    /**
     * Reads the strings from the file at {fileLoc} and returns them as a queue.
     * The strings in the file must be separated by newline characters.
     * The strings in the file cannot have more than 100 characters.
    */
    // TODO
    FILE* file = fopen(fileLoc, "r");
    size_t len = 0;
    char curChar = fgetc(file);

    queue_t* queue = initializeQueue();
    while (1){
        char* line = malloc(100 * sizeof(char));

        int i = 0;
        while(curChar != '\n' && i < 99){
            line[i] = curChar;
            i++;
            curChar = fgetc(file);
        }
        line[i] = '\0';
        line = realloc(line, i * sizeof(char));
        add2q(queue, line);

        while (curChar == '\n' || curChar == '\r'){
            curChar = fgetc(file); // for dealing with case where line ends with "\r\n"
        }
        if (feof(file)){
            break;
        }
    }
    return queue;

}

char** encryptStringsInMemory(queue_t* stringQueue){
    /**
     * Encrypts the strings in the array {strings} and returns the encrypted strings in a new array.
     * Applies multi-process parallelism to encrypt the strings, 100 per process.
    */
    int numWords = qsize(stringQueue);

    char** encrypted = malloc(sizeof(char*) * numStrings);

    // TODO

    return encrypted;
}

void encryptStrings(char* fileLoc, char* encryptedFileLoc){
    /**
     * Encrypts the strings in the file at {fileLoc} and saves the encrypted strings to the file at {encryptedFileLoc}.
     * the strings in the file must be seprated by newline character
     * 
     * The encryption is parallelize across multiple processes.
    */
    // TODO

    queue_t* stringQueue = readStringsFromFile(fileLoc);

    int numWords = qsize(stringQueue);

    char** encryptedStrings = encryptStringsInMemory(stringQueue);
    FILE* file = fopen(encryptedFileLoc, "w");


    for (int i = 0; i < numWords; i++){
        fprintf(file, "%s\n", encryptedStrings[i]);
    }

}
