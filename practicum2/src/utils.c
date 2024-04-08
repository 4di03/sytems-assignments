#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <libgen.h>
#include "utils.h"
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>

int min(int a, int b){
  /**
   * Returns the minimum of two integers.
   * 
   * Args:
   * a (int): first integer
   * b (int): second integer
   * 
   * returns:
   * int: minimum of a and b
  */
  return a < b ? a : b;
}

long send_all(int s, char *buf, long len)
{
  /**
   * Sends all data from buffer through socker.
   * 
   * returns
   * numbero f bytes on success, -1 on failure
  */
     //FROM https://stackoverflow.com/questions/33646539/should-send-and-recv-buffer-size-be-as-big-as-possible
    long total = 0;        // how many bytes we've sent
    int n;

    int iter = 0;

    while(total < len) {
        n = send(s, buf+total, min(4096, len - total), 0);
        if (n == -1) { break; }
        total += n;


        if (iter++ % 100 == 0){
            printf("Sent %ld bytes\n", total);
        }

    }

    printf("Total sent %ld bytes\n", total);

    if (n == -1 || total < len) {
        printf("Have %ld bytes left to send\n", len - total);
        return -1; // return -1 on failure, 0 on success
    }else{
        return total;
    }
} 


long receive_all(int s, char *buf, long len) {
    /**
     * Receives all data into buffer from socket.
     * 
     * returns
     * 0 on success, -1 on failure
    */
    long total = 0; // how many bytes we've received
    int n;
    int iter = 0;
    int chunkSize = 4096;

    while (total < len){ // while the read amount of data is smaller than teh chunk size
        n = recv(s, buf + total, chunkSize, 0); 

        if (n <= 0 ){
            printf("Error receiving data: %s\n", strerror(errno));
            return -1;
        }

        total += n;

        if(iter++ % 100 == 0){
        printf("Received %ld bytes\n", total);
        }
    } // if n is equal to the chunk size, it means there is more data to be read

    printf("Total recieved %ld bytes\n", total);


    return (total > 0) ? total : -1; // return -1 on failure, 0 on success
}
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

    char* remote_fp = calloc(1024, sizeof(char));
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
    char** result = calloc(10, sizeof(char*));
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