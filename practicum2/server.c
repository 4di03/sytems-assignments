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


#include "src/server_commands.h"


int main(void)
{
  return run_server();
}
