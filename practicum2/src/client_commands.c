#include "constants.h"
#include "utils.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void prepare_message(char* buffer, char* tmp_message) {
  /**
   * Converts the raw message given by the client to one that can be processed
   * by the server.
   *
   */

  char* raw_message =
      strdup(tmp_message); // copy this to make it not a string literal

  strip_newline(raw_message);
  char** message_split = split_str(raw_message, " ");

  char* message;
  if (string_equal(message_split[0], "exit") ||
      string_equal(message_split[0], "exit\n")) {
    message = "exit";
  } else if (string_equal(message_split[0], "WRITE")) {
    ;
    // will be in form 'WRITE <local file path> <remote file path>'
    // missing remote file name defaults to local file name
    char* localFilePath = message_split[1];
    char* remoteFilePath;
    if (message_split[2] == NULL) {
      remoteFilePath = localFilePath;
    } else {
      remoteFilePath = message_split[2];
    }

    char* actualFilePath = get_actual_fp(localFilePath, 0);

    char* fileData = calloc(MAX_FILE_SIZE, sizeof(char));

    FILE* actualFile = fopen(actualFilePath, "r");

    if (actualFile == NULL) {
      printf("Error opening file %s !\n", actualFilePath);
      return;
    }

    fread(fileData, sizeof(char), MAX_FILE_SIZE, actualFile);

    fclose(actualFile);
    char* response = malloc(sizeof(char) * MAX_COMMAND_SIZE);

    sprintf(response, "WRITE %s %s", remoteFilePath, fileData);

    free(fileData);
    message = response;
  } else if (string_equal(message_split[0], "GET")) {
    // will be in form 'GET <remote file path> <local file path>'
    // missing local file name defaults to remote file name

    char* remoteFilePath = message_split[1];

    if (remoteFilePath == NULL) {
      printf("Invalid use of GET, must provide remote file path.\n");
      exit(1);
    }
    char* response = malloc(sizeof(char) * MAX_COMMAND_SIZE);

    sprintf(response, "GET %s", remoteFilePath);
    message = response;
  } else {
    message = raw_message;
  }

  snprintf(buffer, MAX_COMMAND_SIZE, "%s", message); // secure copy
}

int validate_message(char* message) {
  /**
   * Validates the raw message given by the user.
   *
   * returns:
   *  1 if message is valid, 0 otherwise
   */
  char** message_split = split_str(message, " ");

  if (message_split[0] == NULL) {
    return 0;
  }

  int first_word_exit = string_equal(message_split[0], "exit") ||
                        string_equal(message_split[0], "exit\n");
  int has_nulls = (message_split[0] == NULL || message_split[1] == NULL) &&
                  !first_word_exit; // if the first word is exit, we don't need
                                    // to check for nulls

  int first_word_valid =
      (first_word_exit) ||
      (message_split[0] != NULL && (string_equal(message_split[0], "WRITE") ||
                                    string_equal(message_split[0], "GET") ||
                                    string_equal(message_split[0], "RM")));

  return (!has_nulls) && first_word_valid;
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
    char* buffer = malloc(sizeof(char) * MAX_FILE_SIZE);
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
  char server_message[2000], raw_message[2000];

  int state = 0;

  char* client_message = malloc(sizeof(char) * MAX_COMMAND_SIZE);

  // Clean buffers:
  memset(server_message, '\0', sizeof(server_message));
  memset(raw_message, '\0', sizeof(raw_message));
  memset(client_message, '\0', sizeof(client_message));

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

    printf("[Client] Socket created successfully\n");

    // Set port and IP the same as server-side:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("[Client] Attempt Number: %d\n", retries);
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


  printf("[Client] Sending message: %s\n", client_message);

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

  handle_response(server_message, raw_message);

  printf("[Client] exiting program!\n");

  // Close the socket:
  close(socket_desc);

  return 0;
}
