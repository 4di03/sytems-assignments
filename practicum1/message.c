#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include "message.h"


cache_t cache;


void init_cache(enum ReplacementPolicy replacementPolicy){
    cache.replacementPolicy = replacementPolicy;



    for (int i = 0; i < 16; i++){
        cache.messages[i] = NULL; // initialize all messages to NULL
    }
}


int hash_id(unsigned int id){
    return id % 16;
}
int hash(message_t* message){
    return hash_id(message->id);
}


void lru_insert(cache_t* cache, message_t* message){
    /**
     * Inserts the message into the cache using the least recently used replacement policy
    */
   if(cache->capacity < CACHE_SIZE){
       cache->messages[cache->capacity] = message;
       message->cached_time = cache->capacity;
       cache->capacity++;
       return;
   }else{


   for (int i = 0 ; i < cache->capacity; i++){

         if (cache->messages[i]->cached_time == 0){
            free(cache->messages[i]); // free it as we are deleting it
            cache->messages[i] = message;
         }else{
            cache->messages[i]->cached_time--; // to make sure the least recently used message is at index 0
         } 
   }

   // Least recently used message is at index min_cache_index


   }
}


void random_insert(cache_t* cache, message_t* message){
    /**
     * Inserts the message into the cache using the random replacement policy
    */

   int index;
    if (cache->capacity < CACHE_SIZE){
        // if the cache is not full, we can just insert the message
        index = cache->capacity;

        cache->capacity++;

    }else{

    int index = rand() % 16;

    if (cache->messages[index] != NULL){
        free(cache->messages[index]); // free memory as we are overriding it
    }
    }

    cache->messages[index] = message;
    return;
}

void insert_message_into_cache(cache_t* cache, message_t* message){
    /**
     * Inserts the message into the cache at the index given by the hash function
    */
   cache->capacity++;

   if (cache->replacementPolicy == LRU){

       lru_insert(cache, message);
    }else{
        random_insert(cache, message);
    }

    // if (cache->messages[hash(message)] != NULL){
    //     // if the message is already in the cache, we don't want to insert it again
    //     return;
    // }

    // int index = hash(message);
    // cache->messages[index] = message;
    // return;

}

int message_in_cache(cache_t* cache, unsigned int id){
    /**
     * Returns 1 if the message is in the cache, 0 otherwise
     
    */
    int index = hash_id(id);
    if (cache->messages[index]->id == id){
        return index;
    }
    return -1;
}
void* copy_object(void* object, size_t size){
    void* copy = malloc(size);
    if (copy == NULL){
        fprintf(stderr, "Malloc failed!\n");
        exit(1);
    }
    memcpy(copy, object, size);
    return copy;
}

void update_lru_times(cache_t* cache, int index){
    /**
     * Updates the cached_time field of all messages in the cache
     * 
     * The element with max cache time is most recently used, so 
     * we want to find all elements with a higher cache time
     * than the element at the given index and decrement their cache time by 1,
     * while setting the cache time of the element at the given index to the max cache time (cache->capacity - 1)
    */
    for (int i = 0; i < cache->capacity; i++){

        if (cache->messages[i] != NULL && cache->messages[i]->cached_time > cache->messages[index]->cached_time){
            cache->messages[i]->cached_time--;
        }
    }
    cache->messages[index]->cached_time = cache->capacity - 1;
}

message_t* get_message_from_cache(cache_t* cache, unsigned int id){
    /**
     * Returns the message from the cache given its unique identifier
    */
   for (int i = 0; i < 16; i++){
       if (cache->messages[i]->id == id){
            update_lru_times(cache, i);
           return copy_object(cache->messages[i], sizeof(message_t));
       }
   }




}


message_t* create_msg(unsigned int id, long timeSent, char* sender, char* receiver, char* content, int delivered){
    /**
     * Dynamically allocate memory for a message_t struct and initialize its fields.
     * 
     * id : unique identifier for message
     * timeSent: epoch time (seconds) when message was ent
     * sender : sender name, if longer than 24 characters, it will be truncated
     * receiver : receiver name, if longer than 24 characters, it will be truncated
     * content : message content , if longer than the max amount of characters, it will be truncated
     * delivered: boolean for whether the  message was delivered or not
    */
    message_t* message = (message_t*) malloc(sizeof(message_t));

    if (message == NULL){
        fprintf(stderr, "Malloc failed!\n");
        exit(1);
    }

    message->id = id;
    message->timeSent = timeSent;
    strncpy(message->sender, sender,sizeof(message->sender));
    strncpy(message->receiver, receiver, sizeof(message->receiver));
    strncpy(message->content, content, sizeof(message->content));
    message->delivered = delivered;

    return message;
}


void store_msg(message_t* msg){
    /**
     * Stores the message in a file named "message_store.txt", creating it if it doesn't exist.
     * 
     * The reason we store the messages in a file is for faster retrieval as it is faster to seek 
     * from a single file than to seek the location of a file in a directory of multiple files.
     * 
     * The seperate messages in the message_store are sepearted by a newline character, and the fields are seperated by pipes
    */



    unsigned int id = msg->id;

    FILE* file = fopen("message_store.txt", "a");

    fprintf(file, "%d|%ld|%s|%s|%s|%d\n", msg->id, msg->timeSent, msg->sender, msg->receiver, msg->content, msg->delivered);

    fclose(file);

    insert_message_into_cache(&cache, msg); // insert whether or nnot themessage is already in caceh

}

int char_to_int(char c){
    return c - '0';
}




int id_matches(char* message_line, unsigned int id){
    /**
     * Checks if the id matches the id of the message that is represented by the given string (fields are pipeseperated)
    */

    int i = 0;
    unsigned int message_id = 0;
    while(message_line[i] != '\0' && i < 10){ // the id will never be more than 10 digits long as 2^32 is a 10 digit number
        message_id = (10 * message_id) + char_to_int(message_line[i]);
        i++;

        if (message_line[i] == '|'){
            return message_id == id;
        }

    }

    fprintf(stderr, "Could not find | to end id field in message string!\n");
    exit(1);

}




message_t* read_msg_from_line(char* message_line){
    /**
     * Creates a message_t instance on the heap given a string representing the message. 
    */
    unsigned int id = atoi(strtok(message_line,"|"));
    long timeSent = atoi(strtok(NULL,"|")); // call it with NULL after the first use
    char* sender = strtok(NULL,"|");
    char* receiver =   strtok(NULL,"|");
    char* content =  strtok(NULL,"|");
    int delivered =  atoi(strtok(NULL,"|"));

    return create_msg(id, timeSent, sender, receiver, content, delivered);

}

message_t* retrieve_msg(unsigned int id){
    /**
     * Retrieves a message given its unique identifier from the message store.
     * 
     * NOTE: returns a copy of the message and not the original message.
     * If you would like to modify the message and save changes, please call store_msg with the modified message.
     * The reason for this is we don't want users updating the id of messages in the cache, as their position in the cache is determined by their id.
     * 
    */

    if(message_in_cache(&cache, id) != -1){
        cache.numHits++;
        return get_message_from_cache(&cache, id);
    }else{
        cache.numMisses++;
    }
    
    FILE* file = fopen("message_store.txt", "r");

    if (file == NULL){
        fprintf(stderr , "Could not read from file!");
        exit(1);
    }
    char* line = malloc(sizeof(char) * 200); // no message should be longer than 200 characters
    size_t len = 0;
    ssize_t lineLength = 0;

    message_t* message;
    while ((lineLength = getline(&line, &len, file)) != -1){

        if (id_matches(line, id)){
            fclose(file);
            line = realloc(line, sizeof(char) * lineLength); // free up space with realloc
            message = read_msg_from_line(line);
        }

    }

    insert_message_into_cache(&cache, message); // put the message in the cache as it was recently accessed
    return message;

    fprintf(stderr, "Could not find message with id: %d in message_store.txt\n", id);

    fclose(file);
    exit(1);


}


void clear_message_store(){
    if (remove("message_store.txt") == 0) {
        printf("message store deleted successfully.\n");
    } else {
        printf("Could not find message store to delete\n");
    }


    // ommited due to leading to double free errors
    // for (int i = 0; i < 16; i++){
    //     if (cache.messages[i] != NULL){
    //         free(cache.messages[i]); // free memory 
    //         cache.messages[i] = NULL; // empty;
    //     }

    // }


}