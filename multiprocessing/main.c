/*
 *  main.c / Multiprocessing in C
 *
 * Adithya Palle / CS5600 / Northeastern University
 * Spring 2024 / Feb 16, 2024
 *
 */
#include "src/program.h"
#include "src/mp.h"
#include <stdio.h>

int stringToInt(char* str) {
  int result = 0;
  for (int i = 0; str[i] != '\0'; i++) {
    result = result * 10 + (str[i] - '0');
  }
  return result;
}

int main(int argc, char* argv[]) {

  if (argc != 3) {
    printf("Must provide a run name for file saving!\n");
    return 1;
  } else {
    int numStrings = stringToInt(argv[2]);
    if (numStrings > 30000){
      printf("Number of strings must be less than or equal to 30000, see Readme for details\n");
      return 1;
    }
  runProgram("outputs", argv[1], stringToInt(argv[2]));
  clearAllSharedMemory(); // clear all shared memory before running further tests

  return 0;
  }
}


