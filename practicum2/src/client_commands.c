#include "constants.h"
#include "utils.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "client_commands.h"




void prepare_message(char* buffer, client_message* cm) {
  /**
   * Converts the raw message given by the client to one that can be processed
   * by the server.
   *
   */

  char* message;
  if (string_equal(cm->command, "WRITE")) {
    ;
    // will be in form 'WRITE <local file path> <remote file path>'
    // missing remote file name defaults to local file name
    char* localFilePath = cm->localFilePath;
    char* remoteFilePath = cm->remoteFilePath == NULL ? localFilePath : cm->remoteFilePath;

    char* actualFilePath = get_actual_fp(localFilePath, 0);

    char* fileData = calloc(MAX_FILE_SIZE, sizeof(char));

    FILE* actualFile = fopen(actualFilePath, "r");

    if (actualFile == NULL) {
      printf("Error opening file %s !\n", actualFilePath);
      return;
    }
    fread(fileData, sizeof(char), MAX_FILE_SIZE, actualFile);

    fclose(actualFile);
    char* response = calloc(MAX_COMMAND_SIZE, sizeof(char));

    sprintf(response, "WRITE %s %s %s", remoteFilePath, cm->permissions, fileData);

    free(fileData);
    message = response;
  } else if (string_equal(cm->command, "GET")) {
    // will be in form 'GET <remote file path> <local file path>'
    // missing local file name defaults to remote file name

    char* remoteFilePath = cm->remoteFilePath;

    char* response = calloc( MAX_COMMAND_SIZE, sizeof(char) );

    sprintf(response, "GET %s", remoteFilePath);
    message = response;
  } else if (string_equal(cm->command, "RM")) {
    // will be in form 'RM <remote file path>'
    char* remoteFilePath = cm->remoteFilePath;

    char* response = calloc( MAX_COMMAND_SIZE, sizeof(char) );

    sprintf(response, "RM %s", remoteFilePath);
    message = response;
  } else if (string_equal(cm->command, "LS")) {
    // will be in form 'LS <remote file path>'
    char* remoteFilePath = cm->remoteFilePath;

    char* response = calloc( MAX_COMMAND_SIZE, sizeof(char) );

    sprintf(response, "LS %s", remoteFilePath);
    message = response;
  } else {
    printf("[Client] Invalid command\n");
    return;
  }
  

  snprintf(buffer, MAX_COMMAND_SIZE, "%s", message); // secure copy
}

client_message* parse_message(char* message){
  /**
   * Parses the message given by the user.
   * 
   * the message will be in one of the forms:
   * 
   * WRITE <local file path> <remote file path> <permissions>
   * 
   * <permissions> is one of 'rw' or 'ro'
   * 
   * remote path is optional, check to see if the 2nd field is == 'rw' or 'ro' to determine if remote path is present
   * 
   * GET <remote file path> <local file path>
   * 
   * local path is optional, check to see if the 2nd field is not NULl to determine if local path is present
   * 
   * RM <remote file path>
   * 
  */

  strip_newline(message);

  char** message_split = split_str(message, " ");

  if (message_split[0] == NULL){
    return NULL;
  }

  client_message* cm = calloc(1, sizeof(client_message));

  if (string_equal(message_split[0], "WRITE")){

    cm->command = "WRITE";
    cm->localFilePath = message_split[1];
    cm->remoteFilePath = (message_split[2] == "rw" ||  message_split[2] == "ro") ?  NULL : message_split[2];
    cm->permissions = cm->remoteFilePath == NULL ? message_split[2] : message_split[3];

  } else if (string_equal(message_split[0], "GET")){

    cm->command = "GET";
    cm->remoteFilePath = message_split[1];
    cm->localFilePath = message_split[2] == NULL ? message_split[1] : message_split[2];

  } else if (string_equal(message_split[0], "RM")){

    cm->command = "RM";
    cm->remoteFilePath = message_split[1];

  } else if (string_equal(message_split[0], "LS")){

    // LS <remote file path>

    cm->command = "LS";
    cm->remoteFilePath = message_split[1];

    

  } else {
    printf("[Client] Invalid command\n");
    return NULL;
  }

  return cm;


}

int validate_message(char* message) {
  /**
   * Validates the raw message given by the user.
   *
   * returns:
   *  1 if message is valid, 0 otherwise
   */
  return parse_message(message) != NULL;

}

void handle_response(char* server_message, char* client_message) {
  /**
   * Given the raw message inputted by user and the servers response , handles
   * the response.
   *
   * args:
   * server_message (char*): message from server
   * client_message (char*): message from client (this is the raw message,
   * prepare_message should not have been called on it)
   */
  char** command_split = split_str(client_message, " ");


  if (string_equal(command_split[0], "WRITE")) {

    char* localFilePath = command_split[1];
    char* remoteFilePath = command_split[2] == NULL ? localFilePath : command_split[2];

    printf("[Client] Attempted to write %s to remote location : %s\n",
           localFilePath, remoteFilePath);

    printf("[Client] Server Response: %s\n", server_message);
  } else if (string_equal(command_split[0], "GET")) {

    if(string_equal(server_message, "Error opening file!\n")){
      printf("[Client] File not found at remote location\n");
      return;
    }

    int startReading = 0;
    char* remoteFilePath = command_split[1];
    char* localFilePath;
    if (command_split[2] == NULL) {
      localFilePath = remoteFilePath;
    } else {
      localFilePath = command_split[2];
    }

    int i = 0;
    int j = 0;
    char* buffer = calloc(MAX_FILE_SIZE, sizeof(char));
    while (server_message[i] != '\0') {

      if (startReading == 1) {

        buffer[j++] = server_message[i];
      }

      if (server_message[i] == '\n') {
        startReading = 1;
      }
      i++;
    }

    buffer[j] = '\0';

    char* actualLocalFilePath = get_actual_fp(localFilePath, 0);

    strip_newline(actualLocalFilePath); // makes sure we don't have a newline at
                                        // the end of the file path
    write_data_to_file(buffer, actualLocalFilePath);

    printf("[Client] File from remote (%s) written to %s\n", remoteFilePath,
           localFilePath);

  } else if (string_equal(command_split[0], "RM")) {

    printf("[Client] Attempted to remove file at remote location : %s\n",
           command_split[1]);

    printf("[Client] Server Response: %s\n", server_message);

  } else {
    printf("[Client] Invalid command\n");
  }

  free(command_split);
}

int run_client(char* raw_command) {
  /**
   *
   * Runs an instance of the client.
   *
   * args:
   * given_command (char*): command to run, if NULL, will prompt user for input.
   */

  char* given_command = raw_command != NULL
                            ? strdup(raw_command)
                            : NULL; // free this to make it not a string literal

  int socket_desc;
  struct sockaddr_in server_addr;

  char* server_message = calloc(MAX_COMMAND_SIZE, sizeof(char));
  char* raw_message = calloc(MAX_COMMAND_SIZE, sizeof(char));

  int state = 0;

  char* client_message = calloc(MAX_COMMAND_SIZE, sizeof(char) );

  // Clean buffers:
  memset(server_message, '\0', sizeof(server_message));
  memset(raw_message, '\0', sizeof(raw_message));
  memset(client_message, '\0', MAX_COMMAND_SIZE*sizeof(char));

  // read message before making connection in case user wants to quit right
  // away Get input from the user:

  if (given_command == NULL) {
    printf("Please provide a command!");
    return 1;
  } else {
    strcpy(raw_message, given_command);
  }

  printf("User input: %s\n", raw_message);

  if (validate_message(raw_message) != 1) {
    printf("[Client] Invalid command\n");
    return 1;
  }


  prepare_message(client_message, raw_message); // put the client message there

  if (string_equal(client_message, "")) {
    printf("[Client] Message could not be prepared!\n");
    return 1;
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

    if (DEBUG){
    printf("[Client] Socket created successfully\n");
    }
    // Set port and IP the same as server-side:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");


    if (DEBUG){
    printf("[Client] Attempt Number: %d\n", retries);
    }
    if (connect(socket_desc, (struct sockaddr*)&server_addr,
                sizeof(server_addr)) < 0) {
      printf("Unable to connect, retrying . . .\n");
      sleep(1);
    } else {
      break;
    }
  }

  if (retries >= 3) {
    printf("[Client] Unable to connect to server\n");
    close(socket_desc);
    return -1;
  }

  printf("[Client] Connected with server successfully\n");

  int messageLen = MAX_COMMAND_SIZE * sizeof(char);

  if (messageLen < 100){
    printf("[Client %d] Sending message to server: %s\n",getpid(), client_message);
  } else{
    printf("[Client %d] Sending message to server with length %d\n",getpid(), messageLen);
  }
  // Send the message to server:
  if (send_all(socket_desc, client_message, messageLen) < 0) {
    printf("Unable to send message\n");
    close(socket_desc);
    return -1;
  }
  
  printf("[Client %d] Sent message to server!\n", getpid());



  printf("[Client %d] Waiting to receive server's response . . .\n", getpid());
  // Receive the server's response if the message is not exit
  if (receive_all(socket_desc, server_message, messageLen) < 0) {
    printf("Error while receiving server's msg\n");
    close(socket_desc);
    return -1;
  }

  printf("[Client %d] Recieved server's response!\n", getpid());


  handle_response(server_message, raw_message);

  if (DEBUG){
  printf("[Client] exiting program!\n");
  }
  // Close the socket:
  close(socket_desc);

  return 0;
}
