#include "src/server_commands.h"
#include "src/client_commands.h"
#include "src/utils.h"
#include "src/constants.h"
#include "assert.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>


void clear_filesystems(){
    /**
     * Helper to clear filesystems before running tests.
    */
    system("rm -rf filesystems/*;mkdir filesystems/server_fs;mkdir filesystems/client_fs;");
    // using system call just for simplicity, in an actual application, we would use the C standard library to interact with the filesystem.
}
int fileExists(const char *path) {
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
    assert(string_equal(write_response, "File written successfully to test.txt!\n"));
    FILE* file = fopen("filesystems/server_fs/test.txt", "r");
    char* file_data = malloc(sizeof(char) * MAX_FILE_SIZE);
    fgets(file_data, MAX_FILE_SIZE, file);
    fclose(file);
    assert(string_equal(file_data, "Hello, World!") == 1);
    free(file_data);

    // test on nested file
    write_response = write_remote("Hello, Jim!", "nested/jim.txt");
    assert(string_equal(write_response, "File written successfully to nested/jim.txt!\n"));
    FILE* file2 = fopen("filesystems/server_fs/nested/jim.txt", "r");
    char* file_data2 = malloc(sizeof(char) * MAX_FILE_SIZE);
    fgets(file_data2, MAX_FILE_SIZE, file);
    fclose(file2);
    assert(string_equal(file_data2, "Hello, Jim!") == 1);
    free(file_data2);

    // test on doubly nested file
    write_response = write_remote("Hello, Jim2!", "nested/jim/jim.txt");
    assert(string_equal(write_response, "File written successfully to nested/jim/jim.txt!\n"));
    FILE* file3 = fopen("filesystems/server_fs/nested/jim/jim.txt", "r");
    char* file_data3 = malloc(sizeof(char) * MAX_FILE_SIZE);
    fgets(file_data3, MAX_FILE_SIZE, file3);
    fclose(file3);
    assert(string_equal(file_data3, "Hello, Jim2!") == 1);
    free(file_data3);
}



void test_delete(){
    // test delete
    printf("Testing delete . . .\n");

    assert(fileExists("filesystems/server_fs/test.txt"));
    char* delete_response = delete("test.txt");
    assert(string_equal(delete_response, "File deleted successfully!\n"));
    assert(!fileExists("filesystems/server_fs/test.txt"));

    assert(fileExists("filesystems/server_fs/nested/jim.txt"));

    delete_response = delete("nested/jim.txt");
    assert(string_equal(delete_response, "File deleted successfully!\n"));
    assert(!fileExists("filesystems/server_fs/nested/jim.txt"));
}


void test_read(){
    // test read
    printf("Testing read . . .\n");

    write_remote("Hello, World!", "test.txt");
    char* read_response = read_remote("test.txt");

    assert(string_equal(read_response, "File Data:\nHello, World!"));

    write_remote("Hello, Jim!", "nested/jim.txt");
    read_response = read_remote("nested/jim.txt");
    assert(string_equal(read_response, "File Data:\nHello, Jim!"));
}

void test_utils(){

    char words[15] = "Hello, World!\n";
    strip_newline(words);
    assert(string_equal(words, "Hello, World!"));


}

void test_commands(){
    clear_filesystems();

    printf("Testing commands . . .\n");

    test_write();
    
    // test delete

    test_delete();

    test_read();

    test_utils();

    printf("Command tests passed ✔\n");
}





void test_prepare_message(){

    write_data_to_file("Hello, World!", "filesystems/client_fs/hi.txt");
    
    char* buffer = malloc(sizeof(char) * MAX_COMMAND_SIZE);
    prepare_message(buffer,"rfs WRITE hi.txt server_hi.txt");
    assert(string_equal(buffer, "rfs WRITE server_hi.txt Hello, World!"));
    memset(buffer, '\0', MAX_COMMAND_SIZE);

    prepare_message(buffer, "rfs GET server_hi.txt client_hi.txt");
    assert(string_equal(buffer, "rfs GET server_hi.txt"));
    memset(buffer, '\0', MAX_COMMAND_SIZE);

    prepare_message(buffer, "rfs RM server_hi.txt");
    assert(string_equal(buffer, "rfs RM server_hi.txt"));
    memset(buffer, '\0', MAX_COMMAND_SIZE);

    prepare_message(buffer,"exit");
    assert(string_equal(buffer, "exit"));
    memset(buffer, '\0', MAX_COMMAND_SIZE);


    prepare_message(buffer, "exit\n");
    assert(string_equal(buffer, "exit"));
    memset(buffer, '\0', MAX_COMMAND_SIZE);

    prepare_message(buffer, "rfs GET hi.txt\n");
    assert(string_equal(buffer, "rfs GET hi.txt"));
    memset(buffer, '\0', MAX_COMMAND_SIZE);

    prepare_message(buffer, "rfs RM hi.txt\n");
    assert(string_equal(buffer, "rfs RM hi.txt"));
    memset(buffer, '\0', MAX_COMMAND_SIZE);

}


void test_validate_message(){
    assert(validate_message("rfs WRITE hi.txt server_hi.txt\n") == 1);
    assert(validate_message("rfs GET server_hi.txt client_hi.txt\n") == 1);
    assert(validate_message("rfs WRITE hi.txt\n") == 1);
    assert(validate_message("rfs GET server_hi.txt") == 1);
    assert(validate_message("rfs RM server_hi.txt") == 1);
    assert(validate_message("exit") == 1);
    assert(validate_message("exit\n") == 1);

    assert(validate_message("rfs RM priv/b.txt\n"));


    assert(validate_message("dummy command") == 0);
    assert(validate_message("rfs dummy command") == 0);
    assert(validate_message("rfs WRITE") == 0);
    assert(validate_message("rfs GET") == 0);
    assert(validate_message("rfs RM") == 0);
    assert(validate_message("rfs") == 0);
    assert(validate_message("") == 0);
    assert(validate_message("rf\ns WRITE hi.txt server_hi.txt") == 0);
}

void test_client_commands(){
    printf("Testing client commands . . .\n");
    clear_filesystems();
    test_prepare_message();

    test_validate_message();
    printf("Client command tests passed ✔\n");
}



void test_remote_write(char* localText, char* localFile , char* remoteFile){
    /**
     * Tests that teh local file at <localFile>  with data <localText> is properly written to the remote file at <remoteFile>.
    */
    clear_filesystems();

    char* passedRemoteFile = remoteFile;
    if (remoteFile == NULL){
        passedRemoteFile = ""; // for testing when remote file is not provided
        remoteFile = localFile;
    }

    char* actualLocalFP = get_actual_fp(localFile, 0);
    write_data_to_file(localText, actualLocalFP);

    char* command = malloc(sizeof(char) * MAX_COMMAND_SIZE);

    snprintf(command, MAX_COMMAND_SIZE, "rfs WRITE %s %s", localFile, passedRemoteFile);

    assert(run_client(command) == 0);


    FILE* file = fopen(get_actual_fp(remoteFile, 1), "r");
    char* file_data = malloc(sizeof(char) * MAX_FILE_SIZE);
    fgets(file_data, MAX_FILE_SIZE, file);
    fclose(file);
    assert(string_equal(file_data, localText));


    free(file_data);
    free(actualLocalFP);
}

void test_remote_get(char* remoteText, char* remoteFile, char* localFile){
    /**
     * Tests that the remote file at <remoteFile> with data <remoteText> is properly read and written to the local file at <localFile>.
    */
    clear_filesystems();

    char* passedLocalFile = localFile;
    if (localFile == NULL){
        passedLocalFile = ""; // for testing when local file is not provided
        localFile = remoteFile;
    }

    write_data_to_file(remoteText, get_actual_fp(remoteFile, 1));

    char* command = malloc(sizeof(char) * MAX_COMMAND_SIZE);

    snprintf(command, MAX_COMMAND_SIZE, "rfs GET %s %s", remoteFile, passedLocalFile);

    assert(run_client(command) == 0);

    FILE* file = fopen(get_actual_fp(localFile, 0), "r");
    char* file_data = malloc(sizeof(char) * MAX_FILE_SIZE);
    fgets(file_data, MAX_FILE_SIZE, file);
    fclose(file);
    assert(string_equal(file_data, remoteText));

    free(file_data);
}

void test_remote_delete(){
    // tests that the remote file is properly deleted
    clear_filesystems();

    write_data_to_file("Hello, World!", "filesystems/server_fs/server_hi.txt");

    assert(fileExists("filesystems/server_fs/server_hi.txt"));

    assert(run_client("rfs RM server_hi.txt") == 0);

    assert(!fileExists("filesystems/server_fs/server_hi.txt"));
}


void test_server(){
    clear_filesystems();

    // tests that the client can execute commands on the server
    printf("Testing server . . .\n");

    int pid = fork();

    if (pid == 0){

    if (run_server() != 0){
        printf("Error starting server\n");

        exit(1);
    }
    }else{
    sleep(1); // wait for server to start


    test_remote_delete();

    test_remote_write("Hello, World!", "hi.txt", "server_hi.txt");

    test_remote_write("Hello, Jim!", "v1/jim.txt", "/jims_files/v1/jim.txt");

    test_remote_write("Hello, Jim_missing_remote!", "v1/jim.txt", NULL);


    test_remote_get("Hello, World!", "server_hi.txt", "client_hi.txt");

    test_remote_get("Hello, Jim!", "/jims_files/v1/jim.txt", "client_jim.txt");

    test_remote_get("Hello, Jim_missing_local!", "/jims_files/v1/jim.txt", NULL);

    clear_filesystems(); // clear filesystems before running tests


    //testing invalid commands

    assert(run_client("dummy command") == 1);

    assert(run_client("rfs dummy command") == 1);

    assert(run_client("exit") == 0); // exit the client and server.



    }

    clear_filesystems(); // clear filesystems after running tests


    if (pid != 0){
    // wait for all child processes to finish
    printf("Waiting for server to close . . .\n");
    wait(NULL);
    printf("Server closed\n");

    printf("Server test passed ✔\n");
    
    }



}



int main(){
    clear_filesystems();
    test_commands();    
    test_client_commands(); 
    test_server();

    return 0;
}