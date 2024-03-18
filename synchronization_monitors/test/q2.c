/*
 * q2.c / Synchronization with Monitors
 *
 * This is the modification of pqueuepmain.c as required by question 2.
 *
 * Adithya Palle / CS5600 / Northeastern University
 * Spring 2024 / Feb 25, 2024
 *
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_SIZE 15

char* buffer[BUF_SIZE]; // buffer to store strings
int num = 0;
int produced = 0;
int consumed = 0;
int producer_sleep = 1; // sleep time for producer ( seconds)
pthread_mutex_t mut;
pthread_cond_t cond_empty,
    cond_full; // Split the condition variable into two, one for empty and one
               // for full , for better organization

void* producer(void* param);
void* consumer(void* param);

void printBuffer() {
  printf("Buffer:\n[");
  for (int i = 0; i < BUF_SIZE; i++) {
    printf("%s ", buffer[i]);
  }
  printf("]\n");
}

void initBuffer() {
  /**
   * Initializes the buffer with empty strings.
   */
  for (int i = 0; i < BUF_SIZE; i++) {
    buffer[i] = "";
  }
}

int main(int argc, char* argv[]) {

  initBuffer(); // initialize the buffer
  printf("The code was modified to buffer strings and to shrink the buffer "
         "rather than just updating the index when consuming for better "
         "visualization.\n");
  printf("The producers stop when 30 items have been produced, and the "
         "consumer stops when 30 items have been consumed.\n");
  pthread_t t1, t2;

  if (pthread_mutex_init(&mut, NULL) != 0) {
    perror("pthread_mutex_init");
    exit(1);
  }
  if (pthread_cond_init(&cond_empty, NULL) != 0) {
    perror("pthread_cond_init");
    exit(1);
  }
  if (pthread_cond_init(&cond_full, NULL) != 0) {
    perror("pthread_cond_init");
    exit(1);
  }

  // spawn three producer threads, each producing a different string
  if (pthread_create(&t1, NULL, producer, "A") !=
      0) { // first producer puts "A" into the buffer
    perror("pthread_create");
    exit(1);
  }
  if (pthread_create(&t1, NULL, producer, "B") !=
      0) { // second producer puts "B" into the buffer
    perror("pthread_create");
    exit(1);
  }
  if (pthread_create(&t1, NULL, producer, "C") !=
      0) { // third producer puts "C" into the buffer
    perror("pthread_create");
    exit(1);
  }

  // spawn two consumer threads which function the same, consuming the strings
  // from the buffer
  if (pthread_create(&t2, NULL, consumer, NULL) != 0) {
    perror("pthread_create");
    exit(1);
  }
  if (pthread_create(&t2, NULL, consumer, NULL) != 0) {
    perror("pthread_create");
    exit(1);
  }
  /* wait for 30 events to be both produced and consumed and then exit */
  while (1) {
    pthread_mutex_lock(&mut); // lock so we don't read the counters
                              // mid-production or mid-consumption
    printf("Totals: %d Produced , %d Consumed\n", produced, consumed);
    if (produced >= 30 &&
        consumed >= 30) { // exit if 30 items have been produced and consumed
      printf("WE ARE DONE\n");
      exit(1);
      break;
    }
    pthread_mutex_unlock(
        &mut); // unlock so producers and consumers can continue
    sleep(1);
  }
}

void* producer(void* param) {
  /**
   * puts a string into the buffer.
   * Args:
   * param: void* - the string to be produced
   */

  char* res = (char*)param; // the string to be produced
  while (1) {
    pthread_mutex_lock(&mut);
    if (num > BUF_SIZE) {
      exit(1); // exit if the buffer size counter exceeds the buffer size as it
               // should never happen
    }

    while (num == BUF_SIZE) // only wait if the buffer is full
      pthread_cond_wait(&cond_full, &mut);

    // put this check right after wait so that producers that are woken up after
    // 30 items have been produced don't produce extra items
    if (produced >= 30) {
      printf("Producer %s done as %d items total have been produced! Exiting "
             "this thread.\n",
             res, produced);
      pthread_mutex_unlock(&mut); // unlock the mutex before exiting
      pthread_exit(NULL);         // exit the  current thread
    }

    buffer[num] = res;
    printf("Produced %s at index %d:\n", res, num);
    produced++; // increment the number of items produced
    num++;
    printBuffer();

    pthread_mutex_unlock(&mut);
    pthread_cond_signal(&cond_empty); // signal for consumer wiating on empty
                                      // condition to consume
    sleep(producer_sleep);
  }
}

void* consumer(void* param) {
  /**
   * Consumes a string from the buffer.
   */
  char* res; // the string to be consumed
  while (1) {
    // printf("DEBUG: Consumer  loop , num: %d\n", num);
    pthread_mutex_lock(&mut);
    if (num < 0) {
      exit(1); // exit if the buffer size counter is negative, as it should
               // never happen
    }
    while (num == 0) // only wait if the buffer is empty
      pthread_cond_wait(&cond_empty, &mut);

    if (consumed >= 30) {
      printf("Consumer done with %d items total have been consumed! Exiting "
             "this thread.\n",
             consumed);
      pthread_mutex_unlock(&mut); // unlock the mutex before exiting
      pthread_exit(NULL);         // exit the  current thread
    }

    res = buffer[num - 1];
    buffer[num - 1] = ""; // remove the consumed item from the buffer, replacing
                          // it with an empty string
    printf("Consumed %s at index %d:\n", res, num - 1);
    consumed++; // increment the number of items consumed
    num--;
    printBuffer();
    pthread_mutex_unlock(&mut);
    pthread_cond_signal(
        &cond_full); // signal for producer waiting on full condition to produce
    sleep(producer_sleep *
          2); // sleep for twice the time of the producer so that the consumer
              // is slower so that the buffer is fully utilized
  }
}
