/*
 * client.c -- TCP Socket Client
 *
 * adapted from:
 *   https://www.educative.io/answers/how-to-implement-tcp-sockets-in-c
 */

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "commands.h"
#include <stdlib.h>

char* prepare_message(char* raw_message){
  /**
   * Converts the raw message given by the client to one that can be processed by the server.
  */
  char** message_split = split_str(raw_message, " ");
  if (custom_strcmp(message_split[1], "WRITE")){;
    // will be in form 'rfs WRITE <local file path> <remote file path>'
    // missing remote file name defaults to local file name
    char* localFilePath = message_split[2];
    char* remoteFilePath;
    if (message_split[3] == NULL){
      remoteFilePath = localFilePath;
    }else{
      remoteFilePath = message_split[3];
    }

    char* actualFilePath = get_actual_fp(localFilePath, 0);

    char* fileData = malloc(sizeof(char) * MAX_FILE_SIZE);

    FILE* actualFile = fopen(actualFilePath, "r");

    fgets(fileData, MAX_FILE_SIZE, actualFile);

    fclose(actualFile);
    char* response = malloc(sizeof(char) * MAX_COMMAND_SIZE);

    sprintf(response, "rfs WRITE %s %s", remoteFilePath, fileData);
    return response;
  } else if (custom_strcmp(message_split[1], "GET")){
    // will be in form 'rfs GET <remote file path> <local file path>'
    // missing local file name defaults to remote file name

    char* remoteFilePath = message_split[2];

    if (remoteFilePath == NULL){
      printf("Invalid use of GET, must provide remote file path.\n");
      exit(1);
    }
    char* response = malloc(sizeof(char) * MAX_COMMAND_SIZE);

    sprintf(response, "rfs GET %s", remoteFilePath);    
    return response;
  } else{
    return raw_message;
  }
}

void handle_response(char* server_message , char* client_message){
    char** command_split = split_str(client_message, " ");

    if (custom_strcmp(command_split[1] , "WRITE")){
      printf("Attempted to write %s to remote location : %s\n", command_split[2], command_split[3]);

      printf("Server Response: %s\n", server_message);
    }else if (custom_strcmp(command_split[1] , "GET")){
      int startReading = 0;
      char* remoteFilePath = command_split[2];
      char* localFilePath = get_actual_fp(command_split[3], 0);
      FILE* localFile = fopen(localFilePath, "w");
      int i = 0;
      while (server_message[i] != '\0'){

        if (startReading == 1){


          printf("%c", server_message[i]);
        }
        
        if (server_message[i] == '\n'){
          startReading = 1;
          fputc(server_message[i], localFile);
        }
      }

      fclose(localFile);

      printf("File from remote (%s) written to %s\n", remoteFilePath, localFilePath);

    }else if (command_split[1] == "RM"){

      printf("Attempted to remove file at remote location : %s\n", command_split[2]);

      printf("Server Response: %s\n", server_message);

    }
    else{
      printf("Invalid command\n");
    }

    free(command_split);
  
}


int run_client(){
  int socket_desc;
  struct sockaddr_in server_addr;
  char server_message[2000], raw_message[2000];

  // Clean buffers:
  memset(server_message, '\0', sizeof(server_message));
  memset(raw_message, '\0', sizeof(raw_message));
  int state = 0;

  while(1){

    // read message before making connection in case user wants to quit right away
    // Get input from the user:
    printf("Enter message: ");
    fgets(raw_message, MAX_COMMAND_SIZE, stdin);


    printf("Client Message: %s\n", raw_message);

    if (custom_strcmp(raw_message, "exit\n")) {
      close(socket_desc);
      printf("Closing the client !\n");
      break;
    }

    // Send connection request to server:
    int retries = 0;
    while (retries < 3) {

      retries++;


      // Create socket:
      socket_desc = socket(AF_INET, SOCK_STREAM, 0);

      if (socket_desc < 0) {
        printf("Unable to create socket\n");
        close(socket_desc);
        return -1;
      }

      printf("Socket created successfully\n");

      // Set port and IP the same as server-side:
      server_addr.sin_family = AF_INET;
      server_addr.sin_port = htons(2000);
      server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");


      printf("Attempt Number: %d\n", retries);
      if (connect(socket_desc, (struct sockaddr*)&server_addr,
                  sizeof(server_addr)) < 0) {
        printf("Unable to connect, retrying . . .\n");
        sleep(1);
      } else {
        break;
      }
    }

    if (retries > 3) {
      printf("Unable to connect to server\n");
      close(socket_desc);
      return -1;
    }

    printf("Connected with server successfully\n");


    char* client_message = prepare_message(raw_message);

    // Send the message to server:
    if (send(socket_desc, client_message, strlen(client_message), 0) < 0) {
      printf("Unable to send message\n");
      close(socket_desc);
      return -1;
    }

    // Receive the server's response:
    if (recv(socket_desc, server_message, sizeof(server_message), 0) < 0) {
      printf("Error while receiving server's msg\n");
      close(socket_desc);
      return -1;
    }

    handle_response(server_message, client_message);

  
  }

  printf("exiting program!\n");
    // Close the socket:

  return 0;
}



int main() {
return run_client();
}
