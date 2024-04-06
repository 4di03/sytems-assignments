/**
 * commands.c
 * 
 * file for implementation of server side command handlers.
*/

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands.h"
#include <dirent.h>
#include <libgen.h>
#include <sys/stat.h>

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
    */

    char** result = malloc(sizeof(char*) * 10);
    char* token = strtok(str, delim);
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

char* write_remote(char* fileData, char* remoteFilePath){
    /**
     * Write file to server.
     * 
     * Args:
     * localFile (char*): data of file to write.
     * remoteFilePath (char*): path to remote file
     * 
     * returns:
     * char*: response message
     * 
    */

   char* fp = get_actual_fp(remoteFilePath, 1);

    write_data_to_file(fileData, fp);

    char* out_message = malloc(sizeof(char) * MAX_COMMAND_SIZE);

    sprintf(out_message, "File written successfully to %s!\n", remoteFilePath);

    return out_message;
}

int custom_strcmp(char* str1, char* str2){
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

char* read_remote(char* remoteFilePath){
    /**
     * Read file from server, and send to client.
     * 
     * Args:
     * remoteFilePath (char*): path to remote file
     * client_sock (int): client socket
    */

    

    FILE* file = fopen(get_actual_fp(remoteFilePath,1), "r");
    if (file == NULL){
        printf("Error opening file!\n");
        exit(1);
    }

    char* file_data = malloc(sizeof(char) * MAX_FILE_SIZE);
    fgets(file_data, MAX_FILE_SIZE, file);

    fclose(file);

    char* response = malloc(sizeof(char) * MAX_FILE_SIZE + 20);

    sprintf(response, "File Data:\n%s", file_data);
    return response;
}

char* delete(char* remoteFilePath){
    /**
     * Delete file from server.
     * 
     * Args:
     * remoteFilePath (char*): path to remote file
    */

    if (remove(get_actual_fp(remoteFilePath, 1)) != 0){
        return "Error deleting file!\n";
    }

    return "File deleted successfully!\n";
}

char* process_request(char* request, int client_sock){
    /**
     * Helper to process command from client on the serverside.
     * 
     * Args:
     * command (char*): command from client
     * client_sock (int): client socket
     * 
     * returns:
     * char*: response message
     * 
    */


    int i = 0;
    char* command = malloc(sizeof(char) * MAX_COMMAND_SIZE);

    while (request[i] != '\n'){
        command[i] = request[i];
        i++;
    }

    command[i] = '\0';
    command = realloc(command, sizeof(char) * (i+1));

    char** command_split = split_str(command, " ");

    if (strcmp(command_split[0] ,"rfs") != 0){
        printf("Invalid command, must start with 'rfs'!\n");
        exit(1);

    }
    if (command_split[1] == "WRITE"){
        // request should be in form rfs WRITE <remote_file_path> <file_data?

        char* remaining_data = malloc(sizeof(char) * 1024);

        int j =0;
        while (request[i] != '\0'){
            remaining_data[j] = request[i];
            j++;
        }

        if (j == 0 && command_split[1] == "WRITE"){
            printf("No file data provided with WRITE command!\n");
            exit(1);
        }

        remaining_data[j] = '\0';

        remaining_data = realloc(remaining_data, sizeof(char) * (j+1));
        
        return write_remote(remaining_data, command_split[3]);

    }else if (command_split[1] == "GET"){
        // request should be in form rfs GET <remote_file_path> 

        return read_remote(command_split[3]);

    } else if (command_split[1] == "RM"){
        // request should be in form rfs RM <remote_file_path> 

        return delete(command_split[2]);

    } else {

        return "Invalid command! The command after RFS should be one of WRITE, GET, or RM\n";
    }


}