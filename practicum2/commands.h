/**
 * commands.c
 * 
 * file for implementation of server side command handlers.
*/
#pragma once
#include <sys/socket.h>
#define MAX_FILE_SIZE 1024
#define MAX_COMMAND_SIZE 1024
#include <stdio.h>


char* write_remote(char* fileData, char* remoteFilePath);

int custom_strcmp(char* str1, char* str2);
char* read_remote(char* remoteFilePath);

char* delete(char* remoteFilePath);



char* get_actual_fp(char* filePath, int remote);

char** split_str(char* str, char* delim);

char* process_request(char* request, int client_sock);

int custom_strcmp(char* str1, char* str2);