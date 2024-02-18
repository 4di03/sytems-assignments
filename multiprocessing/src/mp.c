#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <pthread.h>
#include <sys/wait.h>
#include "queue.h"
#include <string.h>
#include <unistd.h>
#include <sched.h>
#include "mp.h"

void* allocate_shared_mem(int size) {
    /**
     * creates a a pointer to {size} bytes of data in shared memory and returns the pointer.
    */
    int shm_id = shmget(IPC_PRIVATE, size, IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(1);
    }
    void* shm_ptr = shmat(shm_id, NULL, 0);
    if (shm_ptr == (void*) -1) {
        perror("shmat");
        exit(1);
    }
    return shm_ptr;
}


void add2SharedQ(queue_t* queue, void* element) {
  /**
   * Adds one instance of an element to the end of a queue on shared memory
   * Naturally, the node will also be in shared memory.
   */

  // Allocate memory for the new node
  node_t* newNode = allocate_shared_mem(sizeof(node_t));
  if (newNode == NULL) {
    // Handle memory allocation failure
    printf("Memory allocation for new node failed\n");
    return;
  }

  // Initialize the new node
  *newNode =
      (node_t){element, queue->backNode->leftNeighbor, queue->backNode, 0};

  // Update the pointers in the existing nodes
  queue->backNode->leftNeighbor->rightNeighbor = newNode;
  queue->backNode->leftNeighbor = newNode;
  queue->size += 1;
}


queue_t* initializeSharedQueue() {
    /**
     * Initializes a shared queue in shared memory.
    */
    queue_t* shared_q= (queue_t*) allocate_shared_mem(sizeof(queue_t));

    node_t* frontNode = allocate_shared_mem(sizeof(node_t));
    node_t* backNode = allocate_shared_mem(sizeof(node_t));

    frontNode->isSentinel = 1;
    backNode->isSentinel = 1;
    shared_q->frontNode = frontNode;
    shared_q->backNode = backNode;

    shared_q->frontNode->rightNeighbor = shared_q->backNode;
    shared_q->backNode->leftNeighbor = shared_q->frontNode;

    return shared_q;
}


pthread_mutex_t* create_shared_mutex() {
    /**
     * Creates a mutex in shared memory and returns a pointer to it.
    */
    pthread_mutex_t* mutex = (pthread_mutex_t*) allocate_shared_mem(sizeof(pthread_mutex_t));
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(mutex, &attr);
    return mutex;
}

