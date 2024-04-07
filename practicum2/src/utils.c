#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <libgen.h>
#include "utils.h"




void generate_random_file(char* filePath, int size){
  /**
   * Generate a text file with random alphanumeric characters  at the specified file path.
   * 
   * Args:
   * filePath (char*): path to file
   * size (int): size of file in MB
  */

  FILE* file = fopen(filePath, "w");

  if (file == NULL){
    printf("Error opening file!\n");
    exit(1);
  }

  char* randomChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789\n";

  for (int i = 0; i < size; i++){
    for (int j = 0; j < 1024 * 1024; j++){
      fputc(randomChars[rand() % 63], file);
    }
  }

  fclose(file);

  printf("File %s of size %d MB created successfully!\n", filePath, size);

  return;


}

void strip_newline(char* str){
  /**
   * Strip last newline character from string.
   * 
   * Args:
   * str (char*): string to strip newline from
  */
    int i = -1;
    while (str[++i] != '\0'){
    }

    if (str[i-1] == '\n'){ // only strip LAST newline character
        str[i-1] = '\0';
    }
  return;
}

int string_equal(char* str1, char* str2){
  /**
   * Compares strings even if their buffer sizes are different.
   * 
   * returns:
   *  1 if strings are equal, 0 otherwise
  */
  int i = 0;
  while (str1[i] != '\0' && str2[i] != '\0'){
    if (str1[i] != str2[i]){
      return 0;
    }
    i++;
  }
  return str1[i] == str2[i];
}

char* get_actual_fp(char* filePath, int remote){
    /**
     * returns the actual path to the remote filesystem given a relative path from its root.
     * args:
     * filePath (char*): relative path to file
     * remote (int): 1 if the file is on the remote filesystem, 0 otherwie(local)
     * 
    */

    char* fs_loc = remote ? "server_fs" : "client_fs";

    char* remote_fp = malloc(sizeof(char) * 1024);
    sprintf(remote_fp, "filesystems/%s/%s", fs_loc ,filePath);
    return remote_fp;

}

char** split_str(char* str, char* delim){
    /**
     * Split string into list of strings based on delimiter.
     * 
     * Doesn't destroy original string.
    */

    char* strCopy = strdup(str); // copy the string to avoid modifying the original
    char** result = malloc(sizeof(char*) * 10);
    char* token = strtok(strCopy, delim);
    int i = 0;
    while(token != NULL){
        result[i] = token;
        token = strtok(NULL, delim);
        i++;
    }
    result[i] = NULL; // set last element to NULL to indicate end of list

    result = realloc(result, sizeof(char*) * (i+1));
    return result;
}

int directory_exists(char* dirPath){
    /**
     * Check if directory exists.
     * 
     * Args:
     * dirPath (char*): path to directory
     * 
     * returns:
     * int: 1 if directory exists, 0 otherwise
    */

    if (opendir(dirPath)){
        return 1;
    }

    return 0;
}

void make_parent_dirs(char* filePath){
    /**
     * Create parent directories for a file.
     * 
     * Args:
     * filePath (char*): path to file
    */

    char* copyFilePath = strdup(filePath); // copy the file path to avoid modifying the original

    char* dir = dirname(copyFilePath);

    if(!directory_exists(dir)){

        make_parent_dirs(dir); // recursively create parent directories until a direector that is already made is reached
        mkdir(dir, 0777);
    }


}

char* write_data_to_file(char* fileData, char* filePath){
    /**
     * Write data to file.
     * 
     * Args:
     * fileData (char*): data to write to file
     * filePath (char*): path to file
    */

    make_parent_dirs(filePath);

    FILE* file = fopen(filePath, "w");
    if (file == NULL){
        return "Error opening file!\n";
        exit(1);
    }

    fputs(fileData, file);

    fclose(file);

    return "File written successfully!\n";
}