#include "queue.h"

#include "encrypter.h"
#include "mp.h"
#include "polybius.h"
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <math.h>

int ceil_divide(int dividend, int divisor) {
    // Perform regular integer division
    int quotient = dividend / divisor;

    // Check if there's a remainder
    if (dividend % divisor != 0) {
        // If there's a remainder, increment the quotient by 1
        quotient++;
    }

    return quotient;
}
int arrayLen(void** arr) {
  int i = 0;
  while (arr[i] != NULL) {
    i++;
  }
  return i;
}

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

  if (feof(file)) {
    return queue; // returns teh empty queu
  } 
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

void write_to_pipe(FILE* stream, char* message) {
  /**
   * Writes output to the pipe.
   * Args:
   *  stream - file opening to write to
   *  message - string message
   *
   * To give this funciton the ability to be called consectively on file
   * descriptor streams, we do not open or close the file stream in this
   * function and leave that up to the caller to handle.
   * 
   * This is thread-safe on unix systems as the write() function is atomic.
   */

  if (stream == NULL) {
    perror("fdopen");
    exit(EXIT_FAILURE);
  }

  fprintf(stream, "%s", message);
}
void encryptAndPipe(queue_t* queue, int pipe, pthread_mutex_t* sharedQueueMutex,
                     int stringsPerProcess) {
  /**
   * Encrypts the strings in the queue {queue} and writes the encrypted strings
   * to the file at {fileLoc}. The strings in the queue are encrypted in
   * parallel.
   * 
   * Implementation details:
   *  - This function is called by each child process to encrypt the strings in
   *  - the queue and write the encrypted strings to the file at {fileLoc}.
   *  - This function pops one string at a time from the queue, encrypts it, and then proceeds to write the encrypted string to the file at {fileLoc}.
   *  - This process repeats until the queue is empty or the max number of strings per process is reached.
   *  - The queue is locked with a mutex before popping a string from the queue and unlocked after the string is popped, ensuring process safety
   *  - The file is written to using a pipe, which is thread-safe on unix systems as the write() function is atomic.
   * 
   * Args:
   *  queue: queue of strings to encrypt
   *  pipe: file descriptor of the pipe to write to
   *  stringsPerProcess: max number of strings to encrypt per process
   * 
   */
  

  int popped = 0;

  FILE* stream = fdopen(pipe, "a");
  while (popped <= stringsPerProcess) {
    // lock queue with mutex
    pthread_mutex_lock(sharedQueueMutex); // lock queue before popping

    if (queue->size == 0) { // its safe to check size of queue here as we have
                            // locked the queue
      // unlock queue with mutex
      pthread_mutex_unlock(sharedQueueMutex);
      break;
    }

    char* line = popQ(queue);

    // unlock queue with mutex
    pthread_mutex_unlock(sharedQueueMutex);

    char* encrypted = pbEncode(line, shuffledTable); // uses cypher program

    popped++;

    // pipe string to file, no need for synchronization as we are using seperate
    // pipes per process
    write_to_pipe(stream, strcat(encrypted, "\n"));
  }
  fclose(stream);
}

void encryptStrings(char* fileLoc, char* encryptedFileLoc) {
  /**
   * Encrypts the strings in the file at {fileLoc} and pipes the encrypted
   * strings to the file at {encryptedFileLoc}. the strings in the file must be
   * seprated by newline character
   * Args:
   *   fileLoc: location of the file to read strings from
   *   encryptedFileLoc: location of the file to write encrypted strings to
   *   stringsPerProcess: max number of strings to encrypt per process (10 child
   * processes total), default = 100
   *
   * The encryption is run across multiple processes.
   */

  // clear the output file
  FILE* f = fopen(encryptedFileLoc, "w");
  fclose(f);



  queue_t* stringQueue = readStringsFromFile(fileLoc);

  int numWords = qsize(stringQueue);

  // Each process is responsible for encrypting 100 strings
  int stringsPerProcess = 100;

  int num_processes = ceil_divide(numWords, stringsPerProcess);


  pthread_mutex_t* sharedQueueMutex = create_shared_mutex();

  pid_t pid[num_processes];

  int outputPipe[num_processes][2];
  for (int i = 0; i < num_processes; i++) {
    if (pipe(outputPipe[i]) == -1) {
      perror("pipe");
      exit(EXIT_FAILURE);
    }

    int f = open(encryptedFileLoc, O_WRONLY | O_APPEND);
    dup2(f, outputPipe[i][1]); // redirect write end of pipe to file
  }

  // Create {num_processes} child processes to encrypt the strings in parallel
  for (int i = 0; i < num_processes; i++) {
    pid[i] = fork();

    if (pid[i] == -1) {
      // Error occurred
      perror("fork");
      exit(EXIT_FAILURE);
    } else if (pid[i] == 0) {
      // Child process for cypher

      close(outputPipe[i][0]); // close read end of pipe
      encryptAndPipe(stringQueue, outputPipe[i][1], sharedQueueMutex, stringsPerProcess);

      shmdt(stringQueue); // detach this process from shared memory
      // Child process terminates
      exit(EXIT_SUCCESS);
    }
  }

  // Parent process waits for all child processes to terminate, to make sure all processes are found
  for (int i = 0; i <= num_processes; i++) {
    waitpid(pid[i], NULL, 0);
  }
}
