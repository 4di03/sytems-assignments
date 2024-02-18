
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include "src/randomStringGen.h"
#include "src/queue.h"
#include "src/encrypter.h"
#include "src/program.h"
#include "src/polybius.h"
#include <unistd.h>

char* getCWD() {
    char* cwd = NULL;
    long size = pathconf(".", _PC_PATH_MAX);

    if ((cwd = (char *)malloc((size_t)size)) != NULL) {
        if (getcwd(cwd, (size_t)size) == NULL) {
            perror("getcwd() error");
            free(cwd);
            return NULL;
        }
    } else {
        perror("malloc() error");
        return NULL;
    }

    return cwd;
}

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


void testPipe(){
    printf("Testing pipe...\n");
    int pipe_fd[2];
    pid_t pid;
    char message[] = "Hello from parent!\n";

    // Create a pipe
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Fork a child process
    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        close(pipe_fd[0]); // Close read end of pipe in child
        write_to_pipe(pipe_fd[1], "Hello from child!\n");
        exit(EXIT_SUCCESS);
    } else {
        // Parent process
        FILE* stream = fdopen(pipe_fd[0], "r");
        FILE* outputF = fopen("outputs/test_pipe.txt", "w");

        close(pipe_fd[1]); // Close write end of pipe in parent
        char c;
        // read the entire string from the pipe
        while ((c = fgetc (stream)) != 0 && ! feof(stream)){
            fputc(c, outputF);
        }
        fclose(outputF);
    }

    printf("Pipe tests passed!\n");
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

    testPipe();

    testEncrypt();

    integrationTest();

    printf("All tests passed!\n ");
}



int main(){
    runTests();
    return 0;
}

