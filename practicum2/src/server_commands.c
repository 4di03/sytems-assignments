/**
 * commands.c
 * 
 * file for implementation of server side command handlers.
*/

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server_commands.h"
#include <dirent.h>
#include <libgen.h>
#include <sys/stat.h>
#include "constants.h"
#include "utils.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>

void sigint_handler(int signum){
  /**
   * Signal handler for SIGINT (Ctrl+C)
   * 
   * Kills the socket listener at port 2000
  */
  printf("Ctrl+C (SIGINT) received. Exiting...\n");
  system("sh kill_tcp_server.sh"); // kills socket listener at port 2000

  exit(signum);
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

   strip_newline(fp); // remove newline character from file path

    write_data_to_file(fileData, fp);

    char* out_message = malloc(sizeof(char) * MAX_COMMAND_SIZE);

    sprintf(out_message, "File written successfully to %s!\n", remoteFilePath);

    return out_message;
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
        printf("[Server] Error opening file!\n");
        return strdup("Error opening file!\n");
    }

    char* file_data = malloc(sizeof(char) * MAX_FILE_SIZE);
    fread(file_data, sizeof(char),MAX_FILE_SIZE, file);

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
        return strdup("Error deleting file!\n");
    }

    return strdup("File deleted successfully!\n");
}

char* process_request(char* request){
    /**
     * Helper to process command from client on the serverside.
     * 
     * Args:
     * command (char*): command from client
     * 
     * returns:
     * char*: response message
     * 
     * 
     * RESPONSE MUST BE FREED AFTER USE
    */

    strip_newline(request);

    char** command_split = split_str(request, " ");

    if (string_equal(command_split[0] , "WRITE")){
        // request should be in form WRITE <remote_file_path> <file_data>

        char* remoteFilePath = command_split[1];

        char* remaining_data = malloc(sizeof(char) * MAX_FILE_SIZE);

        int j = 2;
        while (command_split[j] != NULL){
            strcat(remaining_data, command_split[j]);
            j++;
            if (command_split[j] != NULL){
                strcat(remaining_data, " "); // to prevent adding extra space at the end
            }
        }
        
        return write_remote(remaining_data, remoteFilePath);

    }else if (string_equal(command_split[0], "GET")){
        // request should be in form GET <remote_file_path> 

        return read_remote(command_split[1]);

    } else if (string_equal(command_split[0] , "RM")){
        // request should be in form RM <remote_file_path> 

        return delete(command_split[1]);

    } else {
        return strdup("Invalid command! The command after should be one of WRITE, GET, or RM\n");
    }


}



int connect_to_client(int socket_desc){
  /**
   * Accepts incoming client connection
  */
  

  printf("\n[Server] Listening for incoming connections.....\n");

  struct sockaddr_in client_addr;
  // Accept an incoming connection:
  socklen_t client_size = sizeof(client_addr);
  int client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);
  
  if (client_sock < 0){
    printf("[Server] Can't accept\n");
    close(socket_desc);
    close(client_sock);
    return -1;
  }
  printf("[Server] Client connected at IP: %s and port: %i\n", 
         inet_ntoa(client_addr.sin_addr), 
         ntohs(client_addr.sin_port));

  return client_sock;
}


int run_server(){


  if (signal(SIGINT, sigint_handler) == SIG_ERR){
    printf("Error while setting signal handler\n");
    return -1;
  }


 int socket_desc;
 struct sockaddr_in server_addr;
    
  // Create socket:
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);

  int _enable = 1;
  setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &_enable, sizeof(_enable)); // allow reuse of port

  
  if(socket_desc < 0){
    printf("Error while creating socket\n");
    return -1;
  }
  printf("[Server] Socket created successfully\n");
  
  // Set port and IP:
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(2000);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  
  // Bind to the set port and IP:
  if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
    printf("[Server] Couldn't bind to the port\n");
    return -1;
  }
  printf("[Server] Done with binding\n");
     // Listen for clients:
  if(listen(socket_desc, 1) < 0){
    printf("Error while listening\n");
    close(socket_desc);
    return -1;
  }
  
   
  int client_sock;
  char client_message[MAX_COMMAND_SIZE];

  while (1){

  // clear the message buffer:
  memset(client_message, '\0', sizeof(client_message));


  // Accept incoming client connection
  client_sock = connect_to_client(socket_desc);
  // Receive client's message:
  if (recv(client_sock, client_message, 
           sizeof(client_message), 0) < 0){
    printf("[Server] Couldn't receive\n");
    close(client_sock);
    continue; // does not close the server
  }

  if (client_message == NULL){
    printf("[Server] Client message is NULL\n");
    close(client_sock);
    continue; // does not close the server
  }

  printf("[Server] Recieved message from client: %s\n", client_message);

  if (string_equal(client_message, "exit\n") || string_equal(client_message, "exit")){
    break;
  }

  // Process the request:
  char* server_message = process_request(client_message);
  
  if (send(client_sock, server_message, strlen(server_message), 0) < 0){
    printf("[Server] Can't send\n");
    close(client_sock);
    continue; // does not close the server
  }




  } // when the client doesnt send the exit message. 
  //  TODO: find a better way to close the server, client shouldn't be able to close the server.

  printf("[Server] Closing the server\n");
  
  // Closing the socket:
  close(client_sock);
  close(socket_desc);
  
  return 0;

}
