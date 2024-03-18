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

void runProgram(char* outputFolder, char* runName, int numStrings) {
  /**
   * Generate random strings and use several concurrent processes to parse these
   * strings in parallel. Args: outputFolder: The folder to save the output
   * files to. runName: The name of the run to save the output files to.
   * args:
   *  outputFolder: The folder to save the output files to.
   * runName: The name of the run to save the output files to.
   * numStrings: The number of random strings to generate.
   * returns:
   *  Outputs encrypted strings to the output folder specified by {outputFolder}
   * and the run name specified by {runName}.
   */

  createFolder(outputFolder);

  char* fileLoc =
      concat(outputFolder, concat("/", concat(runName, "_RandomStrings.txt")));

  generateRandomStrings(numStrings, 10, fileLoc);

  char* encryptedFileLoc = concat(
      outputFolder, concat("/", concat(runName, "_EncryptedStrings.txt")));

  encryptStrings(fileLoc, encryptedFileLoc);

  printf("Random strings saved at %s\n", encryptedFileLoc);

  printf("Encrypted random strings saved at %s\n", encryptedFileLoc);

  // TODO: figure out way to show that it worked, decrypt?
}