#define _GNU_SOURCE
#include "message.h"
#include <stdio.h>
#include <string.h>



message_t* create_msg(unsigned int id, long timeSent, char* sender, char* receiver, char* content, int delivered){
    /**
     * Dynamically allocate memory for a message_t struct and initialize its fields.
     * 
     * id : unique identifier for message
     * timeSent: epoch time (seconds) when message was ent
     * sender : sender name
     * receiver : receiver name
     * content : message content
     * delivered: boolean for whether the  message was delivered or not
    */
    message_t* message = (message_t*) malloc(sizeof(message_t));

    if (message == NULL){
        fprintf(stderr, "Malloc failed!\n");
        exit(1);
    }

    message->id = id;
    message->timeSent = timeSent;
    message->sender = sender;
    message->receiver = receiver;
    message->content = content;
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
    */

    FILE* file = fopen("message_store.txt", "r");

    if (file == NULL){
        fprintf(stderr , "Could not read from file!");
        exit(1);
    }
    char* line = malloc(sizeof(char) * 200); // no message should be longer than 200 characters
    size_t len = 0;
    ssize_t lineLength = 0;

    while ((lineLength = getline(&line, &len, file)) != -1){

        if (id_matches(line, id)){
            fclose(file);
            line = realloc(line, sizeof(char) * lineLength); // free up space with realloc
            return read_msg_from_line(line);
        }

    }

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
}