#include "commands.h"
#include "assert.h"
#include <unistd.h>
#include <stdlib.h>


int isFileExists(const char *path) {
    // Check if the file exists
    if (access(path, F_OK) != -1) {
        // File exists
        return 1;
    } else {
        // File does not exist
        return 0;
    }
}

void test_write(){
    // test write

    printf("Testing write . . .\n");
    char* write_response = write_remote("Hello, World!", "test.txt");
    assert(custom_strcmp(write_response, "File written successfully to test.txt!\n"));
    
    FILE* file = fopen("filesystems/server_fs/test.txt", "r");
    char* file_data = malloc(sizeof(char) * MAX_FILE_SIZE);
    fgets(file_data, MAX_FILE_SIZE, file);
    fclose(file);
    assert(custom_strcmp(file_data, "Hello, World!") == 1);


    free(file_data);

    // test on nested file
    write_response = write_remote("Hello, Jim!", "nested/jim.txt");
    assert(custom_strcmp(write_response, "File written successfully to nested/jim.txt!\n"));
    
    FILE* file2 = fopen("filesystems/server_fs/nested/jim.txt", "r");
    char* file_data2 = malloc(sizeof(char) * MAX_FILE_SIZE);
    fgets(file_data2, MAX_FILE_SIZE, file);
    fclose(file2);

    assert(custom_strcmp(file_data2, "Hello, Jim!") == 1);
    free(file_data2);

}



void test_delete(){
    // test delete
    printf("Testing delete . . .\n");

    assert(isFileExists("filesystems/server_fs/test.txt"));
    char* delete_response = delete("test.txt");
    assert(custom_strcmp(delete_response, "File deleted successfully!\n"));
    assert(!isFileExists("filesystems/server_fs/test.txt"));

    assert(isFileExists("filesystems/server_fs/nested/jim.txt"));

    delete_response = delete("nested/jim.txt");
    assert(custom_strcmp(delete_response, "File deleted successfully!\n"));
    assert(!isFileExists("filesystems/server_fs/nested/jim.txt"));
}


void test_read(){
    // test read
    printf("Testing read . . .\n");

    write_remote("Hello, World!", "test.txt");
    char* read_response = read_remote("test.txt");

    assert(custom_strcmp(read_response, "File Data:\nHello, World!"));

    write_remote("Hello, Jim!", "nested/jim.txt");
    read_response = read_remote("nested/jim.txt");
    assert(custom_strcmp(read_response, "File Data:\nHello, Jim!"));
}

void test_commands(){
    printf("Testing commands . . .\n");

    test_write();
    
    // test delete

    test_delete();

    test_read();

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