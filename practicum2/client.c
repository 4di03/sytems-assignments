/*
 * client.c -- TCP Socket Client
 *
 * adapted from:
 *   https://www.educative.io/answers/how-to-implement-tcp-sockets-in-c
 */


#include "src/client_commands.h"
#include "src/constants.h"
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

    char* command = malloc(sizeof(char) * MAX_COMMAND_SIZE);
    
    for (int i =1; i <argc; i++){
        strcat(command, argv[i]);

        if (i != argc - 1){
        strcat(command, " ");
        }
    }

    
    return run_client(command);
}
