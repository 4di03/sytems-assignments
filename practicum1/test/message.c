#define _GNU_SOURCE
#include "message.h"
#include <stdio.h>
#include <string.h>

cache_t cache;
void free_msg(message_t* msg) {
  /**
   * Helper to free the message_t struct.
   */
  free(msg);
  msg = NULL;
}

void init_cache(enum ReplacementPolicy replacementPolicy) {
  /**
   * Reinitalizes the cache, refreshing all related values;
   */
  cache.replacementPolicy = replacementPolicy;
  cache.numHits = 0;
  cache.numMisses = 0;
  cache.capacity = 0;
  for (int i = 0; i < CACHE_SIZE; i++) {
    cache.messages[i] = NULL; // initialize all messages to NULL
  }
}

void update_lru_times(cache_t* cache, int index) {
  /**
   * Updates the cached_time field of all messages in the cache
   *
   * The element with max cache time is most recently used, so
   * we want to find all elements decrement their cache time by 1, except for
   * the inserted element. this will assure that the most recently used element
   * has the max cache time. while setting the cache time of the element at the
   * given index to the max cache time (cache->capacity - 1)
   *
   * Args:
   * cache : the cache to update
   * index : the index of the message that was most recently used
   */

  for (int i = 0; i < cache->capacity; i++) {
    if (cache->messages[i] != NULL && i != index) {
      cache->messages[i]->cached_time--;
    }
  }
  cache->messages[index]->cached_time = cache->capacity - 1; //
}

void lru_insert(cache_t* cache, message_t* message) {
  /**
   * Inserts the message into the cache using the least recently used
   * replacement policy
   *
   * Args:
   * cache : the cache to insert the message into
   * message : the message to insert
   *
   */

  int message_in_cache = 0;
  int store_index = 0;

  for (int i = 0; i < CACHE_SIZE; i++) {
    if (cache->messages[i] == NULL) {
      store_index = i;
      break;
    } else if (cache->messages[i]->id == message->id) {
      message_in_cache = 1;
      store_index = i;
      break;
    } else if (cache->messages[i]->cached_time <
               cache->messages[store_index]->cached_time) {
      store_index = i;
    }
  }

  if (!message_in_cache) {
    free(cache->messages[store_index]); // free it as we are deleting it
    cache->messages[store_index] = message;

    if (cache->capacity < CACHE_SIZE) {
      cache->capacity++;
    }
  }

  // update cached time of recently inserted mesage to be latest
  update_lru_times(cache, store_index);
}

void random_insert(cache_t* cache, message_t* message) {
  /**
   * Inserts the message into the cache using the random replacement policy
   *
   * Args:
   * cache : the cache to insert the message into
   * message : the message to insert
   */

  if (message_in_cache(cache, message->id)) { // do not do anything for hits.
    return;
  }
  int index;

  if (cache->capacity < CACHE_SIZE) {
    // if the cache is not full, we can just insert the message
    index = cache->capacity;

    cache->capacity++;

  } else {

    index = rand() % CACHE_SIZE;

    if (cache->messages[index] != NULL) {
      free_msg(cache->messages[index]); // free memory as we are overriding it
    }
  }

  cache->messages[index] = message;

  return;
}

void insert_message_into_cache(cache_t* cache, message_t* message) {
  /**
   * Inserts the message into the cache at the index given by the hash function
   *
   * Args:
   * cache : the cache to insert the message into
   * message : the message to insert
   *
   */

  if (cache->replacementPolicy == LRU) {
    lru_insert(cache, message);
  } else {
    random_insert(cache, message);
  }
}

int message_in_global_cache(unsigned int id) {
  /**
   * Returns 1 if the message is in the global cache, 0 otherwise
   *
   * args:
   * id : the unique identifier of the message
   */
  return message_in_cache(&cache, id);
}

int message_in_cache(cache_t* cache, unsigned int id) {
  /**
   * Returns 1 if the message is in the cache, 0 otherwise
   *
   * args:
   * cache : the cache to check
   * id : the unique identifier of the message
   */
  for (int i = 0; i < CACHE_SIZE; i++) {
    if (cache->messages[i] != NULL && cache->messages[i]->id == id) {
      return 1;
    }
  }
  return 0;
}
void* copy_object(void* object, size_t size) {
  /**
   * Copies the object to a new location in memory and returns the pointer to
   * the new location args: object : the object to copy size : the size of the
   * object
   */
  void* copy = malloc(size);
  if (copy == NULL) {
    fprintf(stderr, "Malloc failed!\n");
    exit(1);
  }
  memcpy(copy, object, size);
  return copy;
}

message_t* get_message_from_cache(cache_t* cache, unsigned int id) {
  /**
   * Returns the message from the cache given its unique identifier
   *
   * Args:
   * cache : the cache to retrieve the message from
   * id : the unique identifier of the message
   *
   */
  for (int i = 0; i < CACHE_SIZE; i++) {
    if (cache->messages[i]->id == id) {
      update_lru_times(cache, i);
      return copy_object(cache->messages[i], sizeof(message_t));
    }
  }
}

message_t* create_msg(unsigned int id, long timeSent, char* sender,
                      char* receiver, char* content, int delivered) {
  /**
   * Dynamically allocate memory for a message_t struct and initialize its
   * fields.
   *
   * id : unique identifier for message
   * timeSent: epoch time (seconds) when message was ent
   * sender : sender name, if longer than 24 characters, it will be truncated
   * receiver : receiver name, if longer than 24 characters, it will be
   * truncated content : message content , if longer than the max amount of
   * characters, it will be truncated delivered: boolean for whether the message
   * was delivered or not
   */
  message_t* message = (message_t*)malloc(sizeof(message_t));

  if (message == NULL) {
    fprintf(stderr, "Malloc failed!\n");
    exit(1);
  }

  message->id = id;
  message->timeSent = timeSent;
  strncpy(message->sender, sender, sizeof(message->sender));
  strncpy(message->receiver, receiver, sizeof(message->receiver));
  strncpy(message->content, content, sizeof(message->content));
  message->delivered = delivered;

  return message;
}

void store_msg(message_t* msg) {
  /**
   * Stores the message in a file named "message_store.txt", creating it if it
   * doesn't exist.
   *
   * The reason we store the messages in a file is for faster retrieval as it is
   * faster to seek from a single file than to seek the location of a file in a
   * directory of multiple files.
   *
   * The seperate messages in the message_store are sepearted by a newline
   * character, and the fields are seperated by pipes
   */

  unsigned int id = msg->id;

  FILE* file = fopen("message_store.txt", "a");

  fprintf(file, "%d|%ld|%s|%s|%s|%d\n", msg->id, msg->timeSent, msg->sender,
          msg->receiver, msg->content, msg->delivered);

  fclose(file);

  insert_message_into_cache(
      &cache, msg); // insert whether or nnot themessage is already in caceh
}

int char_to_int(char c) {
  /**
   * Helper to convert a character to an integer
   */
  return c - '0';
}

int id_matches(char* message_line, unsigned int id) {
  /**
   * Checks if the id matches the id of the message that is represented by the
   * given string (fields are pipeseperated)
   *
   * args:
   * message_line : the string representing the message
   * id : the id to check against
   */

  int i = 0;
  unsigned int message_id = 0;
  while (message_line[i] != '\0' &&
         i < 10) { // the id will never be more than 10 digits long as 2^32 is a
                   // 10 digit number
    message_id = (10 * message_id) + char_to_int(message_line[i]);
    i++;

    if (message_line[i] == '|') {
      return message_id == id;
    }
  }

  fprintf(stderr, "Could not find | to end id field in message string!\n");
  exit(1);
}

message_t* read_msg_from_line(char* message_line) {
  /**
   * Creates a message_t instance on the heap given a string representing the
   * message. args: message_line : the string representing the message
   */
  unsigned int id = atoi(strtok(message_line, "|"));
  long timeSent =
      atoi(strtok(NULL, "|")); // call it with NULL after the first use
  char* sender = strtok(NULL, "|");
  char* receiver = strtok(NULL, "|");
  char* content = strtok(NULL, "|");
  int delivered = atoi(strtok(NULL, "|"));

  return create_msg(id, timeSent, sender, receiver, content, delivered);
}

message_t* retrieve_msg(unsigned int id) {
  /**
   * Retrieves a message given its unique identifier from the message store.
   *
   * NOTE: returns a copy of the message and not the original message.
   * If you would like to modify the message and save changes, please call
   * store_msg with the modified message. The reason for this is we don't want
   * users updating the id of messages in the cache, as their position in the
   * cache is determined by their id.
   *
   * args:
   * id : the unique identifier of the message
   *
   */

  if (message_in_cache(&cache, id)) {
    cache.numHits++;

    if (DEBUG == 1) {
      printf("Cache hit for message with id %d\n", id);
    }

    return get_message_from_cache(&cache, id);
  } else {
    cache.numMisses++;
    if (DEBUG == 1) {
      printf("Cache miss for message with id %d\n", id);
    }
  }

  FILE* file = fopen("message_store.txt", "r");

  if (file == NULL) {
    fprintf(stderr, "Could not read from file!");
    exit(1);
  }
  char* line =
      malloc(sizeof(char) *
             CONTENT_SIZE); // no message should be longer than 200 characters
  size_t len = 0;
  ssize_t lineLength = 0;

  message_t* message = NULL;
  while ((lineLength = getline(&line, &len, file)) != -1) {

    if (id_matches(line, id)) {
      fclose(file);
      line = realloc(line,
                     sizeof(char) * lineLength); // free up space with realloc
      message = read_msg_from_line(line);
      break;
    }
  }

  if (message == NULL) {
    fclose(file);
    fprintf(stderr, "Could not find message with id %d\n", id);
    exit(1);
  }

  insert_message_into_cache(
      &cache,
      message); // put the message in the cache as it was recently accessed
  return message;
}

void clear_message_store() {
  /**
   * Clears the message store by deleting the file "message_store.txt"

  */
  if (remove("message_store.txt") == 0) {
    printf("message store deleted successfully.\n");
  } else {
    printf("Could not find message store to delete\n");
  }
}
