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

static int thread_safe;
void increment_shared_int(int* n, pthread_mutex_t* shared_mutex) {
    
    int inc = 0;

    while (inc < 1000){
    if (thread_safe == 1){

    pthread_mutex_lock(shared_mutex);
    }
    (*n)++;
    if (thread_safe == 1){

    pthread_mutex_unlock(shared_mutex);
    }
    //printf("shared int: %d from process : %d\n", *n, getpid());
    inc++;
    }


}

void increment_shared_q(queue_t* shared_q, pthread_mutex_t* shared_mutex) {
    
    int inc = 0;

    while (inc++ < 100){
    
    if (thread_safe == 1){
    pthread_mutex_lock(shared_mutex);
    }

    add2q(shared_q, &inc);

    if (thread_safe == 1){

    pthread_mutex_unlock(shared_mutex);
    }

    //printf("Q size: %d from process : %d\n", qsize(shared_q), getpid());
    }


}

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



queue_t* initSharedQueue() {
    queue_t* shared_q= (queue_t*) allocate_shared_mem(sizeof(queue_t));

    node_t* frontNode = malloc(sizeof(node_t));
    node_t* backNode = malloc(sizeof(node_t));

    frontNode->isSentinel = 1;
    backNode->isSentinel = 1;
    *shared_q->frontNode = *frontNode;
    *shared_q->backNode = *backNode;

    shared_q->frontNode->rightNeighbor = shared_q->backNode;
    shared_q->backNode->leftNeighbor = shared_q->frontNode;

    return shared_q;
}

int* initSharedInt(){
    return (int*) allocate_shared_mem(sizeof(int));

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
int main(int argc, char* argv[]) {

    thread_safe = argv[1][0] - '0'; // 1 for thread safe, 0 for not thread safe

    printf("Thread safe: %d\n", thread_safe);


    // increment  shared int across processes
    int num_processes = 1000;
    pid_t pid;
    
    pthread_mutex_t* mutex = create_shared_mutex();

    queue_t* shared_q = initSharedQueue();


    //int* shared_int = initSharedInt();

    // Create 10 child processes
    for (int i = 1; i <= num_processes; i++) {
        pid = fork();
        
        if (pid == -1) {
            // Error occurred
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process


            //increment_shared_int(shared_int, mutex);
            increment_shared_q(shared_q, mutex);

            // Child process terminates
            exit(EXIT_SUCCESS);

        }
    }

    // Parent process waits for all child processes to terminate
    for (int i = 1; i <= num_processes; i++) {
        wait(NULL); // Wait for any child process to terminate
    }
    pthread_mutex_destroy(mutex);    

    //printf("Shared int: %d, Expected 1000000\n", *shared_int);
    printf("Shared qsize: %d, Expected 1000000\n", qsize(shared_q));

    return 0;
}