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
