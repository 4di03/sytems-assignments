/**
 * commands.c
 * 
 * file for implementation of server side command handlers.
*/
#define _GNU_SOURCE             
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
#include <pthread.h>


pthread_mutex_t fileAccessMutex;

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

    pthread_mutex_lock(&fileAccessMutex); // lock file access
    char* message = write_data_to_file(fileData, fp);

    pthread_mutex_unlock(&fileAccessMutex); // unlock file access

    char* out_message = calloc(MAX_COMMAND_SIZE, sizeof(char));

    sprintf(out_message, "%s", message);

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


    pthread_mutex_lock(&fileAccessMutex); // lock file access

    FILE* file = fopen(get_actual_fp(remoteFilePath,1), "r");
    if (file == NULL){
        printf("[Server] Error opening file!\n");
        return strdup("Error opening file!\n");
    }

    char* file_data = calloc(MAX_FILE_SIZE, sizeof(char));
    fread(file_data, sizeof(char),MAX_FILE_SIZE, file);

    fclose(file);

    pthread_mutex_unlock(&fileAccessMutex); // unlock file access

    char* response = calloc(MAX_FILE_SIZE + 20, sizeof(char));

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

    char* actualFp = get_actual_fp(remoteFilePath, 1);

    pthread_mutex_lock(&fileAccessMutex); // lock file access
    int status = remove(actualFp);
    pthread_mutex_unlock(&fileAccessMutex); // unlock file access

    free(actualFp);

    if (status != 0){
        
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
    char* msg;



    if (string_equal(command_split[0] , "WRITE")){
        // request should be in form WRITE <remote_file_path> <file_data>

        char* remoteFilePath = command_split[1];

        char* remaining_data = calloc(MAX_FILE_SIZE, sizeof(char));

        int j = 2;
        while (command_split[j] != NULL){
            strcat(remaining_data, command_split[j]);
            j++;
            if (command_split[j] != NULL){
                strcat(remaining_data, " "); // to prevent adding extra space at the end
            }
        }
        
        msg = write_remote(remaining_data, remoteFilePath);

    }else if (string_equal(command_split[0], "GET")){
        // request should be in form GET <remote_file_path> 

        msg = read_remote(command_split[1]);

    } else if (string_equal(command_split[0] , "RM")){
        // request should be in form RM <remote_file_path> 

        msg =  delete(command_split[1]);

    } else {
        msg = strdup("Invalid command! The command after should be one of WRITE, GET, or RM\n");
    }

    char* retBuffer = calloc(MAX_COMMAND_SIZE, sizeof(char));
    snprintf(retBuffer, strlen(msg) + 1, "%s", msg);
    return retBuffer;


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
  printf("[Server] Client connected at IP: %s and port: %i with socket id: %d\n", 
         inet_ntoa(client_addr.sin_addr), 
         ntohs(client_addr.sin_port), client_sock);

  return client_sock;
}



void* serve_client(void* data){
  /**
   * Serves a client connection.
  */



  int client_sock = *((int*)data);

  char* client_message = calloc(MAX_COMMAND_SIZE, sizeof(char));
  long messageSize = MAX_COMMAND_SIZE * sizeof(char);

  printf("[Server] Waiting to receive client %d's message . . .\n", client_sock);

  // Receive client's message:
  if (receive_all(client_sock, client_message, messageSize) < 0){
    printf("[Server] Couldn't receive\n");
    close(client_sock);
    return NULL;
  }

  printf("[Server] Received message from client %d\n", client_sock);

  if (client_message == NULL){
    printf("[Server] Client message is NULL\n");
    close(client_sock);
    return NULL;
  } 

  int messageLen = strlen(client_message);
  if (messageLen < 100){
    printf("[Server] received message from client %d: %s\n", client_sock, client_message);
  } else{
    printf("[Server] received message from client %d with length %d\n", client_sock ,messageLen);

  }


  if (string_equal(client_message, "exit\n") || string_equal(client_message, "exit")){
    // does nothing in multithreaded server
    return NULL;
  }

  // Process the request:
  char* server_message = process_request(client_message);

  printf("[Server] Sending respone to client %d  . . .\n", client_sock);

  if (send_all(client_sock, server_message, MAX_COMMAND_SIZE * sizeof(char)) < 0){
    printf("[Server] Can't send\n");
    close(client_sock);
    return NULL;
  }

  printf("[Server] Sent response to client %d\n", client_sock);

  // Closing the socket:
  close(client_sock);

  return NULL;

}


pthread_t* spawn_worker_thread(int* client_sock_ptr){
  /**
   * Spawns a worker thread to handle a specific client connection.
  */

  pthread_t* th1 = calloc(1, sizeof(pthread_t));

  printf("[Server] Spawning worker thread to handle client %d\n", *client_sock_ptr);
  if(pthread_create(th1, NULL, serve_client, client_sock_ptr) != 0){
    printf("[Server] Error while creating thread\n");
    exit(1);
  }

  

  return th1;
}



void* cleanup_function(void* data){
  /**
   * Periodically (every 5 seconds)cleans up threads that have finished execution.
   * 
   * 
   * Also modifies the nextAvailableIndex to point to the next available index in the threads array.
  */


    pthread_t** threads = (pthread_t**) data;


    while (1) {
        for (int i = 0; i < MAX_WORKERS; ++i) {
            if (threads[i] != NULL) { // Thread is still active
                if (pthread_tryjoin_np(*threads[i], NULL) == 0) {
                    printf("Thread %ld has finished\n", *threads[i]);
                    //free(threads[i]); // Free thread pointer
                    threads[i] = NULL; // Mark thread as finished
                }
            }
        }
        sleep(5); // Check every 5 seconds
    }
}

int findNextAvaliableIndex(pthread_t** threads){
  /**
   * Finds the next available index in the threads array.
   * 
   * returns:
   * int: index of next available thread
   * -1 if no thread is available
  */

  for (int i = 0; i < MAX_WORKERS; i++){
    if (threads[i] == NULL){
      return i;
    }
  }

  return -1;
}



int run_server(){
  /**
   * Runs the server.
  */

   if (pthread_mutex_init(&fileAccessMutex, NULL) != 0) {
    perror("pthread_mutex_init");
    exit(1);
  }


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

  if (DEBUG){
  printf("[Server] Socket created successfully\n");
  }
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
  if(listen(socket_desc, MAX_WORKERS) < 0){
    printf("Error while listening\n");
    close(socket_desc);
    return -1;
  }
  

  pthread_t** threads = calloc(MAX_WORKERS , sizeof(pthread_t*)); // array to store worker threads
 
  pthread_t* cleanup_thread = calloc(1,sizeof(pthread_t)); // thread to clean up finished threads
  if(pthread_create(cleanup_thread, NULL, cleanup_function, (void*)threads) != 0){
    printf("Error while creating cleanup thread\n");
    return -1;
  } // start thread to clean up finished threads and clear out the threads array
  int nextAvailableIndex;
  while (1){

  // Accept incoming client connection
  int client_sock = connect_to_client(socket_desc); // reinitalize int for each client so we don't share
  // parent thread is responsible for accepting incoming connections and spawning threads to handle them

  nextAvailableIndex = findNextAvaliableIndex(threads);
  while(nextAvailableIndex == -1){
    nextAvailableIndex = findNextAvaliableIndex(threads); // keep checking until a thread is available
  }

  // Create a separate socket descriptor for each thread
  int* client_sock_ptr = malloc(sizeof(int)); // Allocate memory for the socket descriptor
  if (client_sock_ptr == NULL) {
      fprintf(stderr, "Error: Failed to allocate memory for client_sock_ptr\n");
      close(client_sock); // Close the socket descriptor to prevent resource leaks
      continue; // Skip to the next iteration of the loop
  }
  *client_sock_ptr = client_sock; // Store the socket descriptor in dynamically allocated memory

  /**
   * Why this works:
   * 
   * Thread Doesn't Access Pointer After Scope Ends: The thread doesn't need to access client_sock_ptr itself; 
   * it only needs the socket descriptor it points to. Therefore, even though client_sock_ptr goes out of scope,
   * it doesn't affect the validity of the socket descriptor passed to the thread.
  */
  pthread_t* client_thread = spawn_worker_thread(client_sock_ptr);
  threads[nextAvailableIndex] = client_thread;

  } // when the client doesnt send the exit message. 

  printf("[Server] Closing the server\n");
  close(socket_desc);
  pthread_cancel(*cleanup_thread); // cancel the cleanup thread

  return 0;

}
