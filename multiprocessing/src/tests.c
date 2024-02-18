
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "tests.h"
#include "randomStringGen.h"
#include "queue.h"
#include "encrypter.h"
#include "program.h"
#include "polybius.h"

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


void testEncrypt(){
    printf("Testing encryption...\n");
    
    generateRandomStrings(10000,4, "outputs/test_save_large.txt");
    encryptStrings("outputs/test_save_large.txt", "outputs/test_encrypted.txt");

    queue_t* queue = readStringsFromFile("outputs/test_encrypted.txt");
    queue_t* origQueue = readStringsFromFile("outputs/test_save_large.txt");
    assert(qsize(queue) == 10000);
    while(queue->size > 0){
        char* curString = (char*)popQ(queue);
        char* origString = (char*)popQ(origQueue);
        assert(strlen(curString) == 4);
        assert(pbDecode(curString, shuffledTable) == origString);
    }

    printf("Encryption tests passed!\n");

}


void integrationTest(){
    printf("Running integration test...\n");

    runProgram("outputs", "test_run");


    printf("Integration test passed!\n");
}


void runTests(){
    /**
     * Runs all the tests for all programs in this codebase.
    */
    printf("Running tests...\n");
    //TODO : write the test cases
    testRandomString();

    testReadStringFromFile();


    testEncrypt();

    integrationTest();

    printf("All tests passed!\n ");
}



int main(){
    runTests();
    return 0;
}

