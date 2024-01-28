/*
 * alloc-mem-palle.c / Memory Management
 *
 * Adithya Palle / CS5600 / Northeastern University
 * Spring 2024 / Jan 21, 2024
 *
 */
#include <stdio.h>
#include <stdlib.h>
int main(){

    // allocate 10 MB of heap memory
    int* arr = (int*) malloc(10 * 1024 * 1024);

    // free the heap memory
    free(arr);

    // allocate 5 MB of static segment memory
    static int arr2[5*1024*1024];

    {
        // allocate 1 MB of stack memory
        int arr3[1*1024*1024];
    }
    // stack memory is freed when the block ends (arr3 is out of scope)

    return 0; // static memory is freed at program termination


    // When the program exits and its process is deleted, all memory(heap, stack, static) is freed
}