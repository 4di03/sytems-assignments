/*
 * client.c -- TCP Socket Client
 *
 * adapted from:
 *   https://www.educative.io/answers/how-to-implement-tcp-sockets-in-c
 */


#include "client_commands.h"
#include "constants.h"
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

    char* command = calloc(MAX_COMMAND_SIZE, sizeof(char));
    
    for (int i =1; i <argc; i++){
        strcat(command, argv[i]);

        if (i != argc - 1){
        strcat(command, " ");
        }
    }

    
    return run_client(command);
}
