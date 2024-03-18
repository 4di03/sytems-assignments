#include "queue.h"
#include <pthread.h>

long getSharedMemoryLeft();

void* allocate_shared_mem(int size);

void clearAllSharedMemory();
void add2SharedQ(queue_t* queue, void* element);


queue_t* initializeSharedQueue();


pthread_mutex_t* create_shared_mutex();
pthread_mutex_t* create_mutex();