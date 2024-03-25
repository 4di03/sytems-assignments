
#ifndef CONSTANTS_H
#define CONSTANTS_H

#define CONTENT_SIZE 256
#define ACTUAL_CONTENT_SIZE (CONTENT_SIZE-64)
#define CACHE_SIZE 16

#endif



typedef struct message_t { 
    unsigned int id; // 4 bytes
    long timeSent; // in epoch time, 4 bytes
    char sender[24]; //  24 bytes
    char receiver[24] ; // 24 bytes
    char content[ACTUAL_CONTENT_SIZE];
    int delivered; // 4 bytes
    int cached_time; // 4 bytes

} message_t;
// Each message has a fixed size of 1024 bytes total (storage size)



enum ReplacementPolicy{
    LRU,
    RANDOM
};

typedef struct cache_t{
    // 16 message cache
    message_t* messages[CACHE_SIZE]; // store pointers in cache for faster writing.
    unsigned int numHits;
    unsigned int numMisses;
    enum ReplacementPolicy replacementPolicy;
    int capacity;


} cache_t;
