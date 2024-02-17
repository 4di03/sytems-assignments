/*
 *  main.c / Multiprocessing in C
 *
 * Adithya Palle / CS5600 / Northeastern University
 * Spring 2024 / Feb 16, 2024
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "encrypter.h"
#include "randomStringGen.h"
#include <sys/stat.h>

void createFolder(const char* folder_name) {
  /**
   *  Function to create a folder (directory)
   * Parameters:
   *  folder_name: Name of the folder to be created
   * Returns:
   *  0 if folder creation is successful, -1 otherwise
   */
  // Create directory
  int status =
      mkdir(folder_name,
            0777); // 0777 provides full permissions; you may adjust as needed

  if (status == 0) {
    printf("Directory '%s' created successfully.\n", folder_name);
  } else {
    printf("Directory already exists '%s'.\n", folder_name);
  }
}

char* concat(const char* s1, const char* s2) {
  /**
   * Function to concatenate two strings and return the result
   */

  char* result =
      malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
  // in real code you would check for errors in malloc here
  strcpy(result, s1);
  strcat(result, s2);
  return result;
}

void runProgram(char* outputFolder, char* runName) {
  /**
   * Generate random strings and use several concurrent processes to parse these
   * strings in parallel. Args: outputFolder: The folder to save the output
   * files to. runName: The name of the run to save the output files to.
   * returns:
   *  Outputs encrypted strings to the output folder specified by {outputFolder}
   * and the run name specified by {runName}.
   */

  createFolder(outputFolder);

  char* fileLoc =
      concat(outputFolder, concat("/", concat(runName, "_RandomStrings.txt")));

  generateRandomStrings(100, 100, fileLoc);

  char* encryptedFileLoc = concat(
      outputFolder, concat("/", concat(runName, "_EncryptedStrings.txt")));

  encryptStrings(fileLoc, encryptedFileLoc);

  printf("Random strings saved at %s\n", encryptedFileLoc);

  printf("Encrypted random strings saved at %s\n", encryptedFileLoc);

  // TODO: figure out way to show that it worked, decrypt?
}

int main(int argc, char* argv[]) {

  if (argc != 2) {
    printf("Must provide a run name for file saving!\n");
    return 1;
  } else {
  runProgram("outputs", argv[1]);
  return 0;
  }
}


