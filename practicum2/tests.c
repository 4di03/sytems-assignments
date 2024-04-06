#include "commands.h"
#include "assert.h"

void test_commands(){
    printf("Testing commands . . .\n");

    // test write

    char* write_response = write("Hello, World!", "test.txt");

    
    assert(strcmp(write_response, "File written successfully to filesystems/server_fs/test.txt!\n") == 0);


    printf("Command tests passed ✔\n");
}


void clear_filesystems(){
    // clear filesystems
    system("rm -rf filesystems/*;mkdir filesystems/server_fs;mkdir filesystems/client_fs;");
    // using system call just for simplicity, in an actual application, we would use the C standard library to interact with the filesystem.
}


int main(){

    clear_filesystems();
    test_commands();

    return 0;
}