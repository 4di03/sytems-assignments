/*
 *  queue.c / Implement Process Queue
 *
 * Adithya Palle / CS5600 / Northeastern University
 * Spring 2024 / Jan 27, 2024
 *
 */

#include "queue.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

void add2q(queue_t* queue, void* element) {
  /**
   * Adds one instance of an element to the end of the passed queue.
   */

  // Allocate memory for the new node
  node_t* newNode = malloc(sizeof(node_t));
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
}

void* removeNodeWithinQueue(node_t* node) {
  /**
   * Removes a node from a queue and returns the data it contains.
   * Repairs the queue by linking the left and right neighbors of the removed
   * node.
   */

  node_t* leftNeighbor = node->leftNeighbor;
  node_t* rightNeighbor = node->rightNeighbor;

  leftNeighbor->rightNeighbor = rightNeighbor;
  rightNeighbor->leftNeighbor = leftNeighbor;

  // deteach node
  node->leftNeighbor = 0;
  node->rightNeighbor = 0;
}

void* popQ(queue_t* queue) {
  /**
   * Removes the element (generic) at the front of the queue and returns it.
   */

  node_t* frontNode =
      queue->frontNode->rightNeighbor; // node at front of queue to be removeed

  if (frontNode != 0) {
    removeNodeWithinQueue(frontNode);
    return frontNode->data;
  } else {
    printf("Queue is empty\n");
    exit(1);
  }
}

int qsize(queue_t* queue) {
  /**
   * Returns the number of elements in a queue.
   *
   */

  node_t* node = queue->frontNode->rightNeighbor;
  int size = 0;
  while (node->isSentinel != 1) {
    size++;
    node = node->rightNeighbor;
  }
  return size;
}

process_t* rmProcess(queue_t* queue) {
  /**
   * Removes and returns the process with the highest priority in the process
   * queue.
   */

  node_t* node = queue->frontNode->rightNeighbor;
  int maxPriority = INT_MIN;
  node_t* maxProcessNode;
  while (node->isSentinel != 1) {
    process_t* curProcess = node->data;
    if (curProcess->priority > maxPriority) {
      maxPriority = curProcess->priority;
      maxProcessNode = node;
    }
    node = node->rightNeighbor;
  }

  if (maxProcessNode == 0) {
    printf("No processes in queue\n");
    exit(1);
  }

  removeNodeWithinQueue(maxProcessNode);

  return maxProcessNode->data;
}

void printQueue(const queue_t* queue) {
  /**
   * Prints the contents of a queue.
   */

  node_t* node = (queue->frontNode->rightNeighbor);

  printf("Contents of queue:\n");
  printf("[");
  while (node->isSentinel != 1) {
    process_t* curProcess = node->data;
    printf("%s,", curProcess->name);

    node = node->rightNeighbor;
  }
  printf("]\n");
}
queue_t* addAndPrint(queue_t* queue, process_t* process) {
  /**
   * Adds a process to the queue and prints the queue.
   */

  printf("Adding process %s with priority %d to queue\n", process->name,
         process->priority);
  add2q(queue, process);

  printQueue(queue);

  printf("Size of queue: %d\n", qsize(queue));

  return queue;
}

char* genRandString() {
  /**
   * Generaters a random 3 letter string.
   */

  char* str = malloc(4);
  for (int i = 0; i < 3; i++) {
    str[i] = rand() % 26 + 'a';
  }
  str[3] = '\0';

  return str;
}

process_t** generateRandomProcesses(int n) {
  /**
   * Generates N random processes in a list.
   */

  process_t** processes = malloc(n * sizeof(process_t*));

  for (int i = 0; i < n; i++) {
    char* randName = genRandString();
    int randRuntime = rand() % 1000;
    int randPriority = rand() % 10;

    process_t* process =
        malloc(sizeof(process_t)); //{i, randName, randRuntime, randPriority};
    process->identifier = i;
    process->name = randName;
    process->runtime = randRuntime;
    process->priority = randPriority;
    processes[i] = process;
  }

  return processes;
}

// Function to initialize an empty queue
queue_t* initializeQueue() {
  // Allocate memory for the front sentinel node
  node_t* frontNode = malloc(sizeof(node_t));

  // Check if memory allocation is successful
  if (frontNode == NULL) {
    printf("Memory allocation for front sentinel node failed\n");

    exit(1);
  }

  // Allocate memory for the back sentinel node
  node_t* backNode = malloc(sizeof(node_t));

  // Check if memory allocation is successful
  if (backNode == NULL) {
    printf("Memory allocation for back sentinel node failed\n");
    exit(1);
  }

  // Set up the sentinel nodes
  frontNode->isSentinel = 1;
  backNode->isSentinel = 1;
  frontNode->rightNeighbor = backNode;
  backNode->leftNeighbor = frontNode;

  // Allocate memory for the queue structure
  queue_t* queue = malloc(sizeof(queue_t));

  // Check if memory allocation is successful
  if (queue == NULL) {
    printf("Memory allocation for queue failed\n");
    exit(1);
  }

  // Set queue properties
  queue->frontNode = frontNode;
  queue->backNode = backNode;

  return queue;
}

void runAllTests() {
  /**
   * Runs all tests for the above functions.
   */

  queue_t* queue = initializeQueue();

  assert(qsize(queue) == 0);
  assert(queue->frontNode->rightNeighbor == queue->backNode);
  assert(queue->backNode->leftNeighbor == queue->frontNode);
  node_t* node = queue->frontNode->rightNeighbor;
  assert(node->isSentinel == 1);

  process_t* lowPrioProcess = &(process_t){1, "a", 1, 1};
  process_t* highPrioProcess = &(process_t){2, "b", 2, 2};

  add2q(queue, lowPrioProcess);

  assert(qsize(queue) == 1);
  assert(queue->frontNode->rightNeighbor->data == lowPrioProcess);
  assert(queue->backNode->leftNeighbor->data == lowPrioProcess);
  node = queue->frontNode->rightNeighbor;
  assert(node->isSentinel == 0);
  assert(queue->frontNode->rightNeighbor->rightNeighbor == queue->backNode);
  assert(queue->frontNode->rightNeighbor->rightNeighbor->isSentinel == 1);

  assert(rmProcess(queue) == lowPrioProcess);

  add2q(queue, lowPrioProcess);
  add2q(queue, highPrioProcess);
  assert(qsize(queue) == 2);

  assert(queue->frontNode->rightNeighbor->data == lowPrioProcess);
  assert(queue->frontNode->rightNeighbor->rightNeighbor->data ==
         highPrioProcess);
  assert(queue->backNode->leftNeighbor->data == highPrioProcess);
  assert(queue->backNode->leftNeighbor->leftNeighbor->data == lowPrioProcess);

  assert(rmProcess(queue) == highPrioProcess);
  assert(rmProcess(queue) == lowPrioProcess);
  assert(qsize(queue) == 0);

  add2q(queue, lowPrioProcess);
  add2q(queue, highPrioProcess);
  assert(qsize(queue) == 2);
  assert(popQ(queue) == lowPrioProcess);
  assert(qsize(queue) == 1);

  assert(popQ(queue) == highPrioProcess);
  assert(qsize(queue) == 0);

  printf("All tests passed!\n");
}