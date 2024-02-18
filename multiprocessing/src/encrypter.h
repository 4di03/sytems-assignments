
#include "queue.h"

void write_to_pipe(int file, char* message);

queue_t* readStringsFromFile(char* fileLoc);

void encryptStrings(char* fileLoc, char* encryptedFileLoc);

