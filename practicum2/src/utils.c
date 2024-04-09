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
#include "constants.h"
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
   * args:
   * s (int): socket file descriptor
   * buf (char*): buffer to send
   * len (long): length of buffer
   * 
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
        if (n == -1) { 
          printf("Error sending data: %s\n", strerror(errno));
          break; 
          
          }
        total += n;


        if (iter++ % 1000 == 0 && DEBUG){
            printf("Sent %ld bytes\n", total);
        }

    }

    if (DEBUG)
      printf("Total sent %ld bytes\n", total);

    if (n == -1 || total < len && DEBUG) {
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
     * args:
     * s (int): socket file descriptor
     * buf (char*): buffer to receive data into
     * len (long): length of buffer
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

        if(iter++ % 1000 == 0 && DEBUG){
        printf("Received %ld bytes\n", total);
        }
    } // if n is equal to the chunk size, it means there is more data to be read

    if (DEBUG)
      printf("Total recieved %ld bytes\n", total);


    return (total > 0) ? total : -1; // return -1 on failure, 0 on success
}
void generate_random_file(char* filePath, int size){
  /**
   * Generate a text file with random alphanumeric characters  at the specified file path.
   * 
   * Args:
   * filePath (char*): path to file
   * size (int): size of file in bytes
  */

  FILE* file = fopen(filePath, "w");

  if (file == NULL){
    printf("Error opening file!\n");
    exit(1);
  }

  char* randomChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789\n";

  for (int i = 0; i < size; i++){
    fputc(randomChars[rand() % 63], file);
    
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

  if (str1 == NULL || str2 == NULL){
    return 0; // always false for null pointers
  }

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

void dict_to_file(dict* d, char* filePath){
    /**
     * Write dictionary to file in csv format.
     * 
     * Args:
     * d (dict*): dictionary to write to file
     * filePath (char*): path to file
    */

   

    FILE* file = fopen(filePath, "w");

    if (file == NULL){
        printf("Error opening file!\n");
        exit(1);
    }

    for (int i = 0; i < d->size; i++){

      if (d->keys[i] != NULL && d->values[i] != NULL){
        fprintf(file, "%s,%s\n", d->keys[i], d->values[i]);
      }
    }

    fclose(file);

}


dict* load_dict_from_file(char* filePath){
    /**
     * Load dictionary from file.
     * 
     * Args:
     * filePath (char*): path to file
     * 
     * returns:
     * dict*: dictionary loaded from file
    */

    FILE* file = fopen(filePath, "r");

    if (file == NULL){
        printf("Error opening file!\n");
        exit(1);
    }

    dict* d = calloc(1, sizeof(dict));
    d->size = MAX_FILE_SIZE;
    d->keys = calloc(d->size, sizeof(char*));
    d->values = calloc(d->size, sizeof(char*));

    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    int i = 0;
    while ((read = getline(&line, &len, file)) != -1){
        strip_newline(line);
        char** parts = split_str(line, ",");
        d->keys[i] = parts[0];
        d->values[i] = parts[1];
        i++;
    }

    fclose(file);

    return d;



}
int fileExists(const char *path) {
    // Check if the file exists
    if (access(path, F_OK) != -1) {
        // File exists
        return 1;
    } else {
        // File does not exist
        return 0;
    }
}

char* get_value_from_dict(dict* d, char* key){
    /**
     * Get value from dictionary.
     * 
     * Args:
     * d (dict*): dictionary
     * key (char*): key to search for
     * 
     * returns:
     * char*: value corresponding to key
     * NULL if key not found
    */

    for (int i = 0; i < d->size; i++){
        if (d->keys[i] != NULL && string_equal(d->keys[i], key)){
            return d->values[i];
        }
    }

    return NULL;
}

void update_dict(dict* d, char* key, char* value){
    /**
     * Update dictionary with key-value pair.
     * 
     * Args:
     * d (dict*): dictionary
     * key (char*): key to update
     * value (char*): value to update
     * 
     * returns:
     * char*: success message
    */

    int latestNullIndex = -1;

    for (int i = 0; i < d->size; i++){
        if (d->keys[i] == NULL){
            latestNullIndex = i;
        } else if (string_equal(d->keys[i], key)){
            d->values[i] = value;
            return;
        }
    }

    // if key not found, add it to the dictionary
    if (latestNullIndex == -1){
        printf("Dictionary full!\n");
        return;
    }else{
        d->keys[latestNullIndex] = key;
        d->values[latestNullIndex] = value;
    }
}

void remove_key_from_dict(dict* d, char* key){
    /**
     * Remove key from dictionary.
     * 
     * Args:
     * d (dict*): dictionary
     * key (char*): key to remove
     * 
     * DOES NOT FREE VALUES. Since we store pointers , it is the responsibility of the caller to free the memory.
    */

    for (int i = 0; i < d->size; i++){
        if (d->keys[i] != NULL && string_equal(d->keys[i], key)){

            d->keys[i] = NULL;
            d->values[i] = NULL;
        }
    }
}

int key_in_dict(dict* d, char* key){
    /**
     * Check if key is in dictionary.
     * 
     * Args:
     * d (dict*): dictionary
     * key (char*): key to search for
     * 
     * returns:
     * int: 1 if key is in dictionary, 0 otherwise
    */

    return get_value_from_dict(d, key) != NULL;
}