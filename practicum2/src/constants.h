#define NUM_CONCURRENT_REQUESTS 5
#define FILE_SIZE_MB 1 // cannot handle more than 2 MB right now
#define MAX_FILE_SIZE 1 + FILE_SIZE_MB * 1024 * 1024
#define MAX_COMMAND_SIZE MAX_FILE_SIZE + 1024
