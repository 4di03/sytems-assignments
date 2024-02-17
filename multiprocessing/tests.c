
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "tests.h"
#include "randomStringGen.h"
#include "encrypter.h"


void testReadStringFromFile(){
    printf("Testing reading strings from file...\n");
    FILE* file = fopen("outputs/test_read.txt", "w");
    fprintf(file, "hello\nworld\n");
    fclose(file);
    queue_t* queue = readStringsFromFile("outputs/test_read.txt");
    assert(qsize(queue) == 2);
    assert(strcmp((char*)queue->frontNode->rightNeighbor->data, "hello") == 0);
    assert(strcmp((char*)queue->frontNode->rightNeighbor->rightNeighbor->data, "world") == 0);
    printf("Reading strings from file tests passed!\n");
}


void testRandomString(){
    printf("Testing random string generation...\n");


    

    char* rand = randomString(10);

    for (int i = 0; i < 10; i++){
        assert(rand[i] >= 97 && rand[i] <= 122);
    }

    char** randomStrings = generateRandomStringsInMemory(10, 5);
    for (int i = 0; i < 10; i++){
        assert(strlen(randomStrings[i]) == 5);
    }


    generateRandomStrings(10, 9, "outputs/test.txt");
    FILE* file = fopen("outputs/test.txt", "r");
    char* line = malloc(9 * sizeof(char));
    size_t len = 0;
    for (int i = 0; i < 10; i++){
        getline(&line, &len, file);
        assert(strlen(line) == 9+ 1); // add +1 for the newline
    }
    printf("Random string generation tests passed!\n");
}

void runTests(){
    /**
     * Runs all the tests for all programs in this codebase.
    */
    printf("Running tests...\n");
    //TODO : write the test cases
    testRandomString();

    testReadStringFromFile();

    printf("All tests passed!\n ");
}



int main(){
    runTests();
    return 0;
}

