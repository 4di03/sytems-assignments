/*
 * server.c -- TCP Socket Server
 * 
 * adapted from: 
 *   https://www.educative.io/answers/how-to-implement-tcp-sockets-in-c
 */

/**
 * Notes:
  Hi all, just wanted to let you know that I added a couple additional sentences in the practicum 2 handout on canvas:

  If a client tries to delete a file that is read-only, it should also fail as though they are trying to write to it (deleting a file is treated like modifying or writing to it)
  For the bonus to implement encryption, you can use the previous cipher program if you want, but you can also extensively modify it or even implement some completely different algorithm if you want. 
*/

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "commands.h"


int connect_to_client(int socket_desc){
  /**
   * Accepts incoming client connection
  */
  
  printf("\nListening for incoming connections.....\n");

  struct sockaddr_in client_addr;
  // Accept an incoming connection:
  socklen_t client_size = sizeof(client_addr);
  int client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);
  
  if (client_sock < 0){
    printf("Can't accept\n");
    close(socket_desc);
    close(client_sock);
    return -1;
  }
  printf("Client connected at IP: %s and port: %i\n", 
         inet_ntoa(client_addr.sin_addr), 
         ntohs(client_addr.sin_port));

  return client_sock;
}


int main(void)
{
  int socket_desc;
  struct sockaddr_in server_addr;
 char client_message[8196];
  
  // Clean buffers:
  memset(server_message, '\0', sizeof(server_message));
  memset(client_message, '\0', sizeof(client_message));
  
  // Create socket:
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  
  if(socket_desc < 0){
    printf("Error while creating socket\n");
    return -1;
  }
  printf("Socket created successfully\n");
  
  // Set port and IP:
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(2000);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  
  // Bind to the set port and IP:
  if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
    printf("Couldn't bind to the port\n");
    return -1;
  }
  printf("Done with binding\n");
     // Listen for clients:
  if(listen(socket_desc, 1) < 0){
    printf("Error while listening\n");
    close(socket_desc);
    return -1;
  }
  
   
  int client_sock;
  do {
  // Accept incoming client connection
  client_sock = connect_to_client(socket_desc);
  // Receive client's message:
  if (recv(client_sock, client_message, 
           sizeof(client_message), 0) < 0){
    printf("Couldn't receive\n");
    close(socket_desc);
    close(client_sock);
    return -1;
  }
  printf("Msg from client: %s\n", client_message);

  // Process the request:
  char* server_message = process_request(client_message, client_sock);
  
  if (send(client_sock, server_message, strlen(server_message), 0) < 0){
    printf("Can't send\n");
    close(socket_desc);
    close(client_sock);
    return -1;

  }

  } while (strcmp(client_message, "exit") != 0); // when the client doesnt send the exit message. 
  //  TODO: find a better way to close the server, client shouldn't be able to close the server.

  printf("Closing the server\n");
  
  // Closing the socket:
  close(client_sock);
  close(socket_desc);
  
  return 0;
}
