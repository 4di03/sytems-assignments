
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void* worker(void* data) {
  /**
   * Worker that simply prints message from thread 120 times.
   */
  char* name = (char*)data;

  for (int i = 0; i < 120; i++) {
    usleep(50000);
    printf("Hi from thread name = %s\n", name);
  }

  printf("Thread %s done!\n", name);

  return NULL;
}

void* writer(void* data) {
  /**
   * Writes 10 integers to array
   */
  int* num = (int*)data;

  for (int i = 0; i < 10; i++) {
    usleep(10000);
    num[i] = i;
    printf("Writing: num[%d] = %d\n", i, num[i]);
  }
}

void* reader(void* data) {
  /**
   * Reads 10 integers from array
   */
  int* num = (int*)data;

  for (int i = 0; i < 10; i++) {
    usleep(10000);

    printf("Reading: num[%d] = %d\n", i, num[i]);
  }
}

/*
# code for creating two threads, and waiting for them to finish
pthread_t th1, th2;
pthread_create(&th1, NULL, worker, "X");
pthread_create(&th2, NULL, worker, "Y"); // spawn two threads
sleep(5);
printf("Exiting from main program\n");
pthread_join(th1, NULL);
pthread_join(th2, NULL); // wait for both threads to finish
return 0;
*/

int main() {
  /**
   * Multi-threaded program that spawns two threads in which one thread writes
   * to an array and the other thread reads from the array concurrently.
   */
  pthread_t th1, th2;

  int* data = malloc(
      10 * sizeof(int)); // 'global' integer array that both threads can access
  pthread_create(&th1, NULL, writer, data);
  pthread_create(&th2, NULL, reader, data); // spawn two threads
  sleep(5);
  printf("====> Cancelling Thread %lu!\n", th1);
  pthread_cancel(th2);
  usleep(100000);
  printf("====> Cancelling Thread %lu!\n", th2);
  pthread_cancel(th1);
  printf("exiting from main program\n");
  return 0;
}

/**
 * Question 4 Answer:
 *
 * The above program which spawns two thread in which one thread writes to an
array and the other thread reads from the array is not consistent.
 * In certain instances the Reader thread reads data at an index after the
Writer thread writes to the array at that index,
 * but in certain instances the Reader thread reads before the Writer thread
writes to the array,causing it to read the default value of 0.
 * For example, here is an output from one run:

Writing: num[0] = 0
Reading: num[0] = 0
Writing: num[1] = 1
Reading: num[1] = 1
Writing: num[2] = 2
Reading: num[2] = 2
Writing: num[3] = 3
Reading: num[3] = 3
Writing: num[4] = 4
Reading: num[4] = 4
Writing: num[5] = 5
Reading: num[5] = 5
Writing: num[6] = 6
Reading: num[6] = 6
Writing: num[7] = 7
Reading: num[7] = 7
Reading: num[8] = 0
Writing: num[8] = 8
Reading: num[9] = 0
Writing: num[9] = 9
====> Cancelling Thread 281472873066784!!
====> Cancelling Thread 281472864612640!
exiting from main program

* In the above, the code works like we want it to for the first 8 iterations,
but after that the reader thread is invoked before the writer thread,
* causing it to read empty data at indices 8 and 9. This inconsistency is
EXPECTED in such a multi-threaded environment as
* the OS schedules the threads and we cannot predict the order in which the
threads are scheduled because other processes running on the system at the time
can also affect the scheduling.
*
* A temporary solution to this problem is to increase the sleep time in the
reader function to ensure that the writer thread has enough time to write to the
array before the reader thread reads from it,
* but this is not a foolproof solution as the OS can still schedule the threads
in any order and allocate an unpredicatable amount of time to each thread.
* Another idea would be join the writer thread before spawning the reader
thread, but this would defeat the purpose of multi-threading as the reader
thread would have to wait for the writer thread to finish before it can start.
* I believe the best solution would be to apply some sort of synchronization
mechanism such as a mutex to each index of the array.
* The idea is to have a mutexes on each indices of the global array, and only
unlock these mutexes in the writer thread before writing to the array.
* This way the reader is blocked from reading from the array until the writer
has finished writing to it and unlocked the mutex.
* However, I'm not sure of the implications of locking all mutexes from the
parent thread and then using the child threads to unlock them as it seems
* that it is best practice to lock and unlock the mutexes in the same thread.
* Ultimately, the nature of this program is not conducive to multi-threading as
we are expected to read from an array concurrently while writing to it,
* so any solution will have its drawbacks.
*/
