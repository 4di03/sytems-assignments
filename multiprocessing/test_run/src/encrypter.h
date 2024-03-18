
#include "queue.h"
#include <stdio.h>

void write_to_pipe(FILE* stream, char* message);

queue_t* readStringsFromFile(char* fileLoc);

void encryptStrings(char* fileLoc, char* encryptedFileLoc);

