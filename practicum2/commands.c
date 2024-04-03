/**
 * commands.c
 * 
 * file for implementation of server side command handlers.
*/

#include <sys/socket.h>
#define MAX_FILE_SIZE 1024
#include <stdio.h>

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

char* write(char* localFile, char* remoteFilePath){
    /**
     * Write file to server.
     * 
     * Args:
     * localFilePath (char*): path to local file
     * remoteFilePath (char*): path to remote file
     * client_sock (int): client socket
     * 
     * returns:
     * char*: response message
     * 
    */


    FILE* file = fopen(remoteFilePath, "w");
    if (file == NULL){
        return "Error opening file!\n";
    }

    fputs(localFile, file);

    fclose(file);

    return "File written successfully!\n";
}

char* read(char* remoteFilePath, int client_sock){
    /**
     * Read file from server, and send to client.
     * 
     * Args:
     * remoteFilePath (char*): path to remote file
     * client_sock (int): client socket
    */

    FILE* file = fopen(remoteFilePath, "r");
    if (file == NULL){
        printf("Error opening file!\n");
        exit(1);
    }

    char* file_data = malloc(sizeof(char) * MAX_FILE_SIZE);
    fgets(file_data, MAX_FILE_SIZE, file);

    fclose(file);

    send(client_sock, file_data, strlen(file_data), 0);
    return "File sent successfully!\n";
}

char* delete(char* remoteFilePath){
    /**
     * Delete file from server.
     * 
     * Args:
     * remoteFilePath (char*): path to remote file
    */

    if (remove(remoteFilePath) != 0){
        return "Error deleting file!\n";
    }

    return "File deleted successfully!\n";
}

char* process_request(char* request, int client_sock){
    /**
     * Helper to process command from client.
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
    char* command = malloc(sizeof(char) * 1000);

    while (request[i] != '\n'){
        command[i] = request[i];
        i++;
    }

    command[i] = '\0';
    command = realloc(command, sizeof(char) * (i+1));

    char** command_split = split_str(command, " ");

    if (command_split[0] != "rfs"){
        printf("Invalid command, must start with 'rfs'!\n");
        exit(1);

    }
    if (command_split[1] == "WRITE"){

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
        
        return write(remaining_data, command_split[3]);

    }else if (command_split[1] == "GET"){

        return read(command_split[3], client_sock);

    } else if (command_split[1] == "RM"){

        return delete(command_split[2]);

    } else {

        return Invalid command! The command after RFS should be one of WRITE, GET, or RM\n";
    }


}