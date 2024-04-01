
#ifndef CONSTANTS_H
#define CONSTANTS_H

#define CONTENT_SIZE 1024
#define ACTUAL_CONTENT_SIZE (CONTENT_SIZE - 64)
#define CACHE_SIZE 500
#define RANDOM_SEED 0
#define DEBUG 0
#endif

typedef struct message_t {
  /**
   * Represents a message in the system, with a fixed size of CONTENT_SIZE bytes
   */
  unsigned int id;   // 4 bytes
  long timeSent;     // in epoch time, 4 bytes
  char sender[24];   //  24 bytes
  char receiver[24]; // 24 bytes
  char content[ACTUAL_CONTENT_SIZE];
  int delivered;   // 4 bytes
  int cached_time; // 4 bytes

} message_t;
// Each message has a fixed size of 1024 bytes total (storage size)

enum ReplacementPolicy {
  /**
   * Represents the replacement policy for the cache.
   */
  LRU,
  RANDOM
};

typedef struct cache_t {
  /**
   * Represents a cache of messages in the system.
   *
   * field:
   * messages :  An array of pointers to messages in the cache.
   * numHits : The number of cache hits.
   * numMisses : The number of cache misses.
   * replacementPolicy : The replacement policy for the cache.
   * capacity : The maximum number of messages that can be stored in the cache.
   *
   */
  // 16 message cache
  message_t*
      messages[CACHE_SIZE]; // store pointers in cache for faster writing.
  unsigned int numHits;
  unsigned int numMisses;
  enum ReplacementPolicy replacementPolicy;
  int capacity;

} cache_t;
