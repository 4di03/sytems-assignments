
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "src/randomStringGen.h"
#include "src/queue.h"
#include "src/encrypter.h"
#include "src/program.h"
#include "src/polybius.h"
#include "src/mp.h"
#include <unistd.h>
#include <pthread.h>
#include <sys/shm.h>

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
        assert(rand[i] >= 97 && rand[i] <= 123 || rand[i] == 33 || rand[i] == 63 || rand[i] == 46); // check that the string is alphabetic or contains termination markers
    }

    char** randomStrings = generateRandomStringsInMemory(100, 5);
    int allEqual = 1;
    for (int i = 0; i < 10; i++){
        assert(strlen(randomStrings[i]) == 5);
        if (i > 0){
        allEqual = allEqual && (strcmp(randomStrings[i-1], randomStrings[i]) == 0);
        }

    }

    assert(allEqual == 0); // check that the strings are not all equal


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

    char* outputFP = "outputs/test_pipe.txt";
    // clear the file
    FILE* file = fopen(outputFP, "w");
    fclose(file);

    int numChildren = 100;
    int dup = 100;// number of times to duplicate the message
    printf("Testing pipe...\n");
    int pipe_fd[numChildren][2];
    pid_t pid[numChildren];
    char message[] = "Hello from parent!\n";

    for (int i = 0; i < numChildren; i++){
        // Create a pipe
        if (pipe(pipe_fd[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        // redirect write end of pipe to file
        int f = open(outputFP, O_WRONLY | O_APPEND);
        dup2(f, pipe_fd[i][1]);

    }
    pthread_mutex_t* fileMutex = create_mutex();

    for (int i = 0; i < numChildren; i++){
        // Fork a child process
        pid[i]= fork();
        if (pid[i] == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid[i] == 0) {
            // Child process
            char message[50];
            FILE* stream = fdopen(pipe_fd[i][1], "a");
            for (int j = 0; j < dup; j++){

                //printf("Child %d writing to pipe for %dth time\n", i,j);
                sprintf(message, "Hello from child %d , v%d!\n", i,j);
                write_to_pipe(stream, message);
            }

            fclose(stream);

            exit(EXIT_SUCCESS);
        } 
    }

    // wait for all children to finish
    for (int i = 0; i < numChildren; i++){
        waitpid(pid[i], NULL, 0);     // wait for all child processes to terminate
    }

    file = fopen(outputFP, "r");
    char* line = malloc(50 * sizeof(char));
    size_t len = 0;
    for (int i = 0; i < numChildren * dup; i++){
        getline(&line, &len, file);
        char expected[50];

        assert(strlen(line) >= strlen("Hello from child 0 , v0!\n"));
        if (feof(file)){
            if (i < numChildren * dup - 1){
                printf("Error: file ended prematurely\n");
                exit(1);
            }
            break;
        }
    }

    printf("Pipe tests passed!\n");
}


char** stringQueueToArray(queue_t* queue){
    char** arr = malloc(queue->size * sizeof(char*));
    int i = 0;
    while (queue->size > 0){
        arr[i++] = (char*)popQ(queue);
    }
    return arr;
}

int cipher_strcmp(char* str1, char* str2){
    /**
     * Compares two strings and returns 1 if they are equal and 0 if they are not.
     * This function is used to compare strings that are encoded using the polybius cipher.
     * i and j are considered equal in the comparison.
    */
    
    int i =0;

    while (str1[i] != '\0'){
        
        char cur_str2 = lowercase(str2[i]);
        char cur_str1 = lowercase(str1[i]);
        

        if( cur_str1 == cur_str2 || (cur_str1== 'i' && cur_str2 == 'j') || (cur_str1 == 'j' && cur_str2 == 'i')){
            i++;
        }else{
            return 0;
        }

    }

    return (str2[i] == '\0');
}

int inArray(char* str, char** arr){

    int i = 0;

    while (arr[i] != NULL){ // keep going until we reach the end of the array
        if (cipher_strcmp(arr[i], str) == 1){
            return 1;
        }
        i++;
    }
    return 0;
}

void testCiphers(){  
    printf("Testing ciphers...\n");
    char** randStrings = generateRandomStringsInMemory(100, 5);

    for (int i = 0; i < 10; i++){
        char* encrypted = pbEncode(randStrings[i], shuffledTable);
        char* decrypted = pbDecode(encrypted, shuffledTable);
        assert(cipher_strcmp(pbDecode(encrypted, shuffledTable), decrypted) == 1);
        assert(cipher_strcmp(pbEncode(decrypted, shuffledTable), encrypted) == 1);
    }

    free(randStrings); // free the memory allocated for the random strings

    // test case where non alphabetic characters are present
    assert(cipher_strcmp(pbDecode(pbEncode("hello~world!", shuffledTable), shuffledTable) , "hello~world!") == 1);

    //test case where only alphabetic characters are present
    assert(cipher_strcmp(pbDecode(pbEncode("hello", shuffledTable) , shuffledTable), "hello") == 1);

    assert(cipher_strcmp(pbDecode("51413111", shuffledTable) , "upkf") == 1);
    printf("Cipher tests passed!\n");
}


void validateFiles(char* stringsFile, char* encryptedFile, int numStrings){

    queue_t* queue = readStringsFromFile(encryptedFile);
    queue_t* origQueue = readStringsFromFile(stringsFile);

    
    assert(qsize(queue) == numStrings);
    assert(qsize(origQueue) == qsize(queue));
    char** origStrings = stringQueueToArray(origQueue);
    char** encryptedStrings = stringQueueToArray(queue);

    // detach from shared memory
    shmdt(queue); 
    shmdt(origQueue);


    for(int i = 0; i < numStrings; i++){
        char* encrypted = encryptedStrings[i];
        char* orig = origStrings[i];


        assert(inArray(pbEncode(orig, shuffledTable), encryptedStrings) == 1); // check that all original strings are present in the encrypted strings

        assert(inArray(pbDecode(encrypted, shuffledTable), origStrings) == 1); // check that all encrypted strings are present in the original strings
    }

}


void testEncrypt(){
    printf("Testing encryption...\n");

    int numStrings = 10;
    
    generateRandomStrings(numStrings,4, "outputs/test_save_large.txt");
    encryptStrings("outputs/test_save_large.txt", "outputs/test_encrypted.txt");

    validateFiles("outputs/test_save_large.txt", "outputs/test_encrypted.txt", numStrings);

    printf("Encryption tests passed!\n");

}


void integrationTest(){
    printf("Running integration test...\n");

    // test generating 100 random strings and encrypting them
    int numStrings = 5000;
    runProgram("outputs", "test_run", numStrings);

    char* randomStringsFile = "outputs/test_run_RandomStrings.txt";
    char* encryptedStringsFile = "outputs/test_run_EncryptedStrings.txt";

    validateFiles(randomStringsFile, encryptedStringsFile, numStrings);

    // test the empty file case
    numStrings = 0; 
    runProgram("outputs", "test_empty", numStrings);
    randomStringsFile = "outputs/test_empty_RandomStrings.txt";
    encryptedStringsFile = "outputs/test_empty_EncryptedStrings.txt";

    validateFiles(randomStringsFile, encryptedStringsFile, numStrings);

    // test teh odd num strings case
    numStrings = 1023;
    runProgram("outputs", "test_odd", numStrings);
    randomStringsFile = "outputs/test_odd_RandomStrings.txt";
    encryptedStringsFile = "outputs/test_odd_EncryptedStrings.txt";
    validateFiles(randomStringsFile, encryptedStringsFile, numStrings);


    printf("Integration test passed!\n");
}


void runTests(){
    /**
     * Runs all the tests for all programs in this codebase.
    */
    printf("Running tests...\n");

    integrationTest(); // run this first because it creates output folder

    clearAllSharedMemory(); // clear all shared memory before running further tests

    printf("Running unit tests...\n");
    //TODO : write the test cases
    testRandomString();

    testReadStringFromFile();

    testPipe();

    testCiphers();

    clearAllSharedMemory(); // clear all shared memory before running further tests
    
    testEncrypt();
    clearAllSharedMemory(); // clear all shared memory before running further tests

    printf("Unit tests passed!\n");

    printf("All tests passed!\n ");
}

int main(){
    runTests();
    return 0;
}//

