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
    message_t* messages[16]; // store pointers in cache for faster writing.
} cache_t;
