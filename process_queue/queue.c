/*
 *  queue.c / Implement Process Queue
 *
 * Adithya Palle / CS5600 / Northeastern University
 * Spring 2024 / Jan 27, 2024
 *
 */

#include <stdio.h>
#include <stdlib.h>

typedef struct process_t{
    /**
     * Represents a process on a computer
    */
    int identifier;
    char* name;
    long runtime;
    int priority;
}process_t;


typedef struct node_t{
    /**
     * Represents a node in a queue
    */
    void* data; // generic data type
    node_t* rightNeighbor;
    node_t* leftNeighbor;
}node_t;

typedef struct queue_t{

    node_t* frontNode;
    node_t* backNode;

}queue_t;


void add2q(queue_t* queue, void* element){
    /**
     * Adds one instance of an element to the end of the passed queue.
    */

}

void* popQ(queue_t* queue){
    /**
     * Removes the element (generic) at the front of the queue and returns it.
    */
}

int qsize(queue_t* queue){
    /**
     * Returns the number of elements in a queue.
     * 
    */

    node_t* node = queue->frontNode;
    int size = 0;
    while(node != NULL){
        size++;
        node = node->rightNeighbor;
    }   
    return size;
}

process_t* rmProcess(queue_t* queue){
    /**
     * Removes and returns the process with the highest priority in the process queue.
    */
}

