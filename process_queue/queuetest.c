/*
 *  queuetest.c / Implement Process Queue
 *
 * Adithya Palle / CS5600 / Northeastern University
 * Spring 2024 / Jan 27, 2024
 *
 */
#include <stdio.h>

#include "queue.h"

int main() {
  // builds and uses a process queue
  printf("Running tests...\n");
  runAllTests();
  int n_processes = 5;

  printf("Generating %d random processes...\n", n_processes);
  process_t** processes = generateRandomProcesses(n_processes);
  queue_t* queue = initializeQueue();

  printf("Size of queue: %d\n", qsize(queue));
  for (int i = 0; i < 5; i++) {
    addAndPrint(queue, processes[i]);
  }

  printf("Removing processes by priority...\n");
  while (qsize(queue) > 0) {
    process_t* removedProcess = rmProcess(queue);
    printf("Removed process %s which has priority %d\n", removedProcess->name,
           removedProcess->priority);
    printQueue(queue);

    printf("Size of queue: %d\n", qsize(queue));
  }

  printf("Adding 5 more random processes...\n");
  processes = generateRandomProcesses(n_processes);
  for (int i = 0; i < 5; i++) {
    addAndPrint(queue, processes[i]);
  }

  printf("Removing all processes from queue from left...\n");

  while (qsize(queue) > 0) {
    process_t* removedProcess = popQ(queue);
    printf("Removed process %s which has priority %d\n", removedProcess->name,
           removedProcess->priority);
    printQueue(queue);
    printf("Size of queue: %d\n", qsize(queue));
  }

  return 0;
}