/**
 * commands.c
 * 
 * file for implementation of server side command handlers.
*/
#pragma once
#include <sys/socket.h>
#define MAX_FILE_SIZE 1024
#define MAX_COMMAND_SIZE 100
#include <stdio.h>


char* get_actual_fp(char* filePath, int remote);

char** split_str(char* str, char* delim);

char* process_request(char* request, int client_sock);