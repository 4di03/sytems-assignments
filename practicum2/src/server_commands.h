/**
 * commands.c
 * 
 * file for implementation of server side command handlers.
*/


char* write_remote(char* fileData, char* remoteFilePath);


char* read_remote(char* remoteFilePath);

char* delete(char* remoteFilePath);

char* process_request(char* request);


int run_server();
void init_metadata();