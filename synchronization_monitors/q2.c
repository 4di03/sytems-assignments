/*
 * q2.c / Synchronization with Monitors
 *
 * This is the modification of pqueuepmain.c as required by question 2.
 * 
 * Adithya Palle / CS5600 / Northeastern University
 * Spring 2024 / Feb 25, 2024
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUF_SIZE 10    

int buffer[BUF_SIZE];						
int num = 0;									
pthread_mutex_t mut;	
pthread_cond_t cond; 

void *producer(void *param);
void *consumer(void *param);

void printBuffer() {
  printf("Buffer:\n[");
  for (int i=0; i<BUF_SIZE; i++) {
    printf("%d ", buffer[i]);
  }
  printf("]\n");
}

int main (int argc, char *argv[])
{
  printf("The code was modified to shrink the buffer rather than just updating the index when consuming for better visualization.\n");
  pthread_t t1, t2;	

  if(pthread_mutex_init(&mut, NULL) != 0) {
    perror("pthread_mutex_init");
    exit(1);
  }
  if(pthread_cond_init(&cond, NULL) != 0) {
    perror("pthread_cond_init");
    exit(1);
  }
  if (pthread_create(&t1, NULL, producer, NULL) != 0) {
    perror("pthread_create");
    exit (1);
  }
  if (pthread_create(&t2, NULL, consumer, NULL) != 0) {
    perror("pthread_create");
    exit (1);
  }
  /* wait a while and then exit */
   sleep(30);
   printf("WE ARE DONE\n");
}

void *producer(void *param)
{
  int res;
  while(1) {
    pthread_mutex_lock (&mut);
    if (num > BUF_SIZE) exit(1);	
    while (num == BUF_SIZE)			
      pthread_cond_wait (&cond, &mut);
    res = (num+1)*(num+1);
    buffer[num] = res;
    printf("Produced %d at index %d:\n", res, num);

    num++;
    printBuffer();

    pthread_mutex_unlock (&mut);
    pthread_cond_signal (&cond);
    sleep(1);
  }
}

void *consumer(void *param)
{
  int i=0;
  while (1) {
    pthread_mutex_lock (&mut);
    if (num < 0) exit(1);   
    while (num == 0)		
      pthread_cond_wait (&cond, &mut);
    i = buffer[num-1];
    buffer[num-1] = 0;
    printf("Consumed %d at index %d:\n",i, num-1);

    num--;
    printBuffer();
    pthread_mutex_unlock (&mut);
    pthread_cond_signal (&cond);
    sleep(3);
  }
}
