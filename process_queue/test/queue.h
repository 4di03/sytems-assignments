

/*
 *  queue.h / Implement Process Queue
 *
 * Adithya Palle / CS5600 / Northeastern University
 * Spring 2024 / Jan 27, 2024
 *
 */
typedef struct process_t {
  /**
   * Represents a process on a computer
   */
  int identifier;
  char* name;
  long runtime;
  int priority;
} process_t;

typedef struct node_t {
  /**
   * Represents a node in a queue
   */
  void* data; // generic data type
  struct node_t* leftNeighbor;
  struct node_t* rightNeighbor;
  int isSentinel; // whether or not the node is a sentinel (dummy node at front
                  // and back of queue)

} node_t;

typedef struct queue_t {
  node_t* frontNode; // sentinel node at the front of the queue
  node_t* backNode;  // sentinel node at the end of the queue

} queue_t;

void add2q(queue_t* queue, void* element);

void* popQ(queue_t* queue);

int qsize(queue_t* queue);

process_t* rmProcess(queue_t* queue);

void printQueue(const queue_t* queue);

queue_t* addAndPrint(queue_t* queue, process_t* process);

char* genRandString();

process_t** generateRandomProcesses(int n);

queue_t* initializeQueue();

void runAllTests(); // runs all tests for the above functions