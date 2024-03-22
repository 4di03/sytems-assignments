
#include <stdlib.h>
#include <stdio.h>
typedef struct message_t { 
    unsigned int id;
    long timeSent; // in epoch time
    char* sender;
    char* receiver;
    char* content;
    int delivered;

} message_t;
// Each message has a fixed size of 1024 bytes total (storage size)

typedef struct cache_t{
    // 16 message cache
    message_t messages[16]; // store actual data in cache for faster access
} cache_t;

message_t* create_msg(unsigned int id, long timeSent, char* sender, char* receiver, char* content, int delivered);

void store_msg(message_t* msg);

message_t* retrieve_msg(unsigned int id);

void clear_message_store();