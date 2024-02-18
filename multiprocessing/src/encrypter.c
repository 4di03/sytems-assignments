#include "queue.h"

#include "encrypter.h"
#include "mp.h"
#include "polybius.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>


int arrayLen(void** arr) {
  int i = 0;
  while (arr[i] != NULL) {
    i++;
  }
  return i;
}

// char** encrypt(char** word) {
//   /**
//    * Encrypts the words in the array {word} and returns the encrypted words in a
//    * new array.
//    */

//   int numWords = arrayLen((void**)word);

//   char** encrypted = malloc(sizeof(char*) * numWords);
//   for (int i = 0; i < 10; i++) {
//     encrypted[i] = pbEncode(word[i], shuffledTable);
//   }
//   return encrypted;
// }

queue_t* readStringsFromFile(char* fileLoc) {
  /**
   * Reads the strings from the file at {fileLoc} and returns them as a queue in
   * shared memory. The strings in the file must be separated by newline
   * characters. The strings in the file cannot have more than 100 characters.
   */
  // TODO
  FILE* file = fopen(fileLoc, "r");
  size_t len = 0;
  char curChar = fgetc(file);

  queue_t* queue = initializeSharedQueue();
  while (1) {
    char* line = malloc(100 * sizeof(char));

    int i = 0;
    while (curChar != '\n' && i < 99) {
      line[i] = curChar;
      i++;
      curChar = fgetc(file);
    }
    line[i] = '\0';
    line = realloc(line, i * sizeof(char));
    add2SharedQ(queue, line);

    while (curChar == '\n' || curChar == '\r') {
      curChar =
          fgetc(file); // for dealing with case where line ends with "\r\n"
    }
    if (feof(file)) {
      break;
    }
  }
  return queue;
}


void write_to_pipe (int file, char* message)
{
/**
 * Writes output to the pipe.
 * Args:
 *  file - pipe opening to write to
 *  message - string message
 */
    
  FILE* stream;
  stream = fdopen (file, "w");
  fprintf (stream, message);
  fclose (stream);
}
void encryptAndPipe(queue_t* queue, int pipe,
                           pthread_mutex_t* sharedQueueMutex,
                           pthread_mutex_t* sharedFileMutex, int stringsPerProcess) {
  /**
   * Encrypts the strings in the queue {queue} and writes the encrypted strings
   * to the file at {fileLoc}. The strings in the queue are encrypted in
   * parallel.
   */
  // TODO

  FILE* file = fdopen(pipe, "w");


  int popped = 0;
  while (popped <= stringsPerProcess) {
    // lock queue with mutex
    pthread_mutex_lock(sharedQueueMutex);

    if (queue->size == 0) { // its safe to check size of queue here as we have
                            // locked the queue
      // unlock queue with mutex
      pthread_mutex_unlock(sharedQueueMutex);
      break;
    }

    char* line = popQ(queue);

    // unlock queue with mutex
    pthread_mutex_unlock(sharedQueueMutex);

    char* encrypted = pbEncode(line, shuffledTable);

    popped++;


    // pipe string to file, no need for synchronization as we are using seperate pipes per process
    write_to_pipe(pipe, strcat(encrypted, "\n"));


  }


}





void encryptStrings(char* fileLoc, char* encryptedFileLoc) {
  /**
   * Encrypts the strings in the file at {fileLoc} and pipes the encrypted
   * strings to the file at {encryptedFileLoc}. the strings in the file must be
   * seprated by newline character
   * Args:
   *   fileLoc: location of the file to read strings from
   *   encryptedFileLoc: location of the file to write encrypted strings to
   *   stringsPerProcess: max number of strings to encrypt per process (10 child processes total), default = 100
   *
   * The encryption is run across multiple processes.
   */
  // TODO

  int stringsPerProcess = 100; // can process a max of 10,000 strings

  queue_t* stringQueue = readStringsFromFile(fileLoc);

  int numWords = qsize(stringQueue);

  // move queue to shared memory

  int shm_id = shmget(IPC_PRIVATE, sizeof(queue_t), IPC_CREAT | 0666);
  if (shm_id == -1) {
    perror("shmget");
    exit(EXIT_FAILURE);
  }
  void* shm_ptr = shmat(shm_id, NULL, 0);
  if (shm_ptr == (void*)-1) {
    perror("shmat");
    exit(EXIT_FAILURE);
  }

  pthread_mutex_t* sharedQueueMutex = create_shared_mutex();
  pthread_mutex_t* sharedFileMutex = create_shared_mutex();

  // Create 10 child processes
  int num_processes = 10;
  pid_t pid[num_processes];


  int outputPipe[num_processes][2];
  for (int i = 0; i < num_processes; i++) {
    if (pipe(outputPipe[i]) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
  }



  // Create 10 child processes
  for (int i = 1; i <= num_processes; i++) {
    pid[i] = fork();

    if (pid[i] == -1) {
      // Error occurred
      perror("fork");
      exit(EXIT_FAILURE);
    } else if (pid[i] == 0) {
      // Child process
      printf("Child process %d (PID: %d) created\n", i, getpid());

      close(outputPipe[i][0]); // close read end of pipe
      encryptAndPipe(stringQueue, outputPipe[i][1], sharedQueueMutex,
                            sharedFileMutex, stringsPerProcess);

      // Child process terminates
      exit(EXIT_SUCCESS);
    } else{
        // Parent process

        close(outputPipe[i][1]); // close write end of pipe

        FILE* outputF = fopen(encryptedFileLoc, "w");
        // read from pipe and write to file
        char* buffer[100];
        FILE* stream = fdopen(outputPipe[i][0], "r");
        int c;
        // read teh entire string from the pipe
        while ((c = fgetc (stream)) != EOF){
            fputc(c, outputF);
        }
        fclose(outputF);


    }
  }

   // Parent process waits for all child processes to terminate
    for (int i = 1; i <= num_processes; i++) {
        waitpid(pid[i], NULL, 0);     // wait for all child processes to terminate
    }
}
