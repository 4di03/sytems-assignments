
#include <stdlib.h>
#include <stdio.h>
#include "data_structures.h"

message_t* create_msg(unsigned int id, long timeSent, char* sender, char* receiver, char* content, int delivered);

void store_msg(message_t* msg);

message_t* retrieve_msg(unsigned int id);

void clear_message_store();

void insert_message_into_cache(cache_t* cache, message_t* message);

int message_in_global_cache(unsigned int id);
int message_in_cache(cache_t* cache, unsigned int id);

message_t* get_message_from_cache(cache_t* cache, unsigned int id);
void init_cache();

void free_msg(message_t* msg);