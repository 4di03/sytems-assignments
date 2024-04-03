/**
 * commands.c
 * 
 * file for implementation of server side command handlers.
*/

#include <sys/socket.h>
#define MAX_FILE_SIZE 1024
#include <stdio.h>

char** split_str(char* str, char* delim);

char* response =  process_request(char* request, int client_sock);