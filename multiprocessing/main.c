/*
 *  main.c / Multiprocessing in C
 *
 * Adithya Palle / CS5600 / Northeastern University
 * Spring 2024 / Feb 16, 2024
 *
 */
#include "src/program.h"
#include <stdio.h>

int main(int argc, char* argv[]) {

  if (argc != 2) {
    printf("Must provide a run name for file saving!\n");
    return 1;
  } else {
  runProgram("outputs", argv[1]);
  return 0;
  }
}


