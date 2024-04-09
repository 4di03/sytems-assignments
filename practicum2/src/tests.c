#include "assert.h"
#include "client_commands.h"
#include "constants.h"
#include "server_commands.h"
#include "utils.h"
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

extern dict* metadata; // metadata dictionary from "server_commands.c"

void clear_filesystems() {
  /**
   * Helper to clear filesystems before running tests.
   */
  system("rm -rf filesystems/*;mkdir filesystems/server_fs;mkdir "
         "filesystems/client_fs;");
  // using system call just for simplicity, in an actual application, we would
  // use the C standard library to interact with the filesystem.

  init_metadata(); // reinitalize metatada
}

void test_write() {
  // test write

  printf("Testing write . . .\n");
  char* write_response = write_remote("Hello, World!", "test.txt");
  assert(string_equal(write_response, "File written successfully!\n"));
  FILE* file = fopen("filesystems/server_fs/test.txt", "r");
  char* file_data = calloc(MAX_FILE_SIZE, sizeof(char));
  fread(file_data, sizeof(char), MAX_FILE_SIZE, file);
  fclose(file);
  assert(string_equal(file_data, "Hello, World!") == 1);
  free(file_data);

  free(write_response);
  write_response = NULL;

  // test on nested file
  write_response = write_remote("Hello, Jim!", "nested/jim.txt");
  assert(string_equal(write_response, "File written successfully!\n"));
  FILE* file2 = fopen("filesystems/server_fs/nested/jim.txt", "r");
  char* file_data2 = calloc(MAX_FILE_SIZE, sizeof(char));
  fread(file_data2, sizeof(char), MAX_FILE_SIZE, file);
  fclose(file2);
  assert(string_equal(file_data2, "Hello, Jim!") == 1);
  free(file_data2);

  free(write_response);
  write_response = NULL;

  // test on doubly nested file
  write_response = write_remote("Hello, Jim2!", "nested/jim/jim.txt");
  assert(string_equal(write_response, "File written successfully!\n"));
  FILE* file3 = fopen("filesystems/server_fs/nested/jim/jim.txt", "r");
  char* file_data3 = calloc(MAX_FILE_SIZE, sizeof(char));
  fread(file_data3, sizeof(char), MAX_FILE_SIZE, file3);
  fclose(file3);
  assert(string_equal(file_data3, "Hello, Jim2!") == 1);
  free(file_data3);

  free(write_response);
  write_response = NULL;
}

void test_delete() {
  // test delete
  printf("Testing delete . . .\n");

  assert(fileExists("filesystems/server_fs/test.txt"));
  char* delete_response = delete ("test.txt");

  assert(string_equal(delete_response, "File deleted successfully!\n"));

  free(delete_response);
  delete_response = NULL;
  assert(!fileExists("filesystems/server_fs/test.txt"));

  assert(fileExists("filesystems/server_fs/nested/jim.txt"));

  delete_response = delete ("nested/jim.txt");
  assert(string_equal(delete_response, "File deleted successfully!\n"));

  free(delete_response);
  delete_response = NULL;
  assert(!fileExists("filesystems/server_fs/nested/jim.txt"));
}

void test_read() {
  // test read
  printf("Testing read . . .\n");

  write_remote("Hello, World!", "test.txt");
  char* read_response = read_remote("test.txt");

  assert(string_equal(read_response, "File Data:\nHello, World!"));

  free(read_response);
  read_response = NULL;

  write_remote("Hello, Jim!", "nested/jim.txt");
  read_response = read_remote("nested/jim.txt");
  assert(string_equal(read_response, "File Data:\nHello, Jim!"));

  free(read_response);
  read_response = NULL;
}

void* thread_write() {
  /**
   * helper function to write to a file for thread testing
   */

  write_data_to_file("Hello, World!", "filesystems/server_fs/thread_test.txt");
  return NULL;
}

void test_write_data_to_file() {

  printf("Testing write_data_to_file . . .\n");

  clear_filesystems();
  assert(!fileExists("filesystems/server_fs/write.txt"));

  write_data_to_file("Hello, World!", "filesystems/server_fs/write.txt");

  assert(fileExists("filesystems/server_fs/write.txt"));

  FILE* file = fopen("filesystems/server_fs/write.txt", "r");
  char* file_data = calloc(MAX_FILE_SIZE, sizeof(char));
  fread(file_data, sizeof(char), MAX_FILE_SIZE, file);
  fclose(file);
  assert(string_equal(file_data, "Hello, World!"));

  free(file_data);

  assert(!fileExists("filesystems/server_fs/thread_test.txt"));

  pthread_t thread;
  pthread_create(&thread, NULL, thread_write, NULL);
  pthread_join(thread, NULL);

  assert(fileExists("filesystems/server_fs/thread_test.txt"));

  FILE* file2 = fopen("filesystems/server_fs/thread_test.txt", "r");
  char* file_data2 = calloc(MAX_FILE_SIZE, sizeof(char));

  fread(file_data2, sizeof(char), MAX_FILE_SIZE, file2);
  fclose(file2);
  assert(string_equal(file_data2, "Hello, World!"));
  free(file_data2);
}

void test_dict() {
  printf("Testing dictionary . . .\n");

  dict* d = calloc(1, sizeof(dict));
  d->size = 5;
  d->keys = calloc(d->size, sizeof(char*));
  d->values = calloc(d->size, sizeof(char*));

  // test key_in_dict

  assert(!key_in_dict(d, "hi.txt"));

  d->keys[0] = "hi.txt";
  d->values[0] = "ro";

  assert(key_in_dict(d, "hi.txt"));

  assert(string_equal(get_value_from_dict(d, "hi.txt"), "ro"));

  // clear dict
  d->keys[0] = NULL;
  d->values[0] = NULL;

  // test update_dict

  assert(!key_in_dict(d, "hi.txt"));

  update_dict(d, "hi.txt", "ro");

  assert(key_in_dict(d, "hi.txt"));

  assert(string_equal(get_value_from_dict(d, "hi.txt"), "ro"));

  // test remove_key_from_dict

  remove_key_from_dict(d, "hi.txt");

  assert(!key_in_dict(d, "hi.txt"));

  // test save_dict_to_file

  update_dict(d, "hi.txt", "ro");
  update_dict(d, "hello.txt", "rw");

  dict_to_file(d, "filesystems/server_fs/metadata.txt");

  FILE* file = fopen("filesystems/server_fs/metadata.txt", "r");

  char* file_data = calloc(MAX_FILE_SIZE, sizeof(char));

  fread(file_data, sizeof(char), MAX_FILE_SIZE, file);

  fclose(file);

  assert(string_equal(file_data, "hello.txt,rw\nhi.txt,ro\n"));

  free(file_data);
  file_data = NULL;
  // test load_dict_from_file

  dict* d2 = load_dict_from_file("filesystems/server_fs/metadata.txt");

  assert(key_in_dict(d2, "hi.txt"));
  assert(key_in_dict(d2, "hello.txt"));

  assert(string_equal(get_value_from_dict(d2, "hi.txt"), "ro"));
  assert(string_equal(get_value_from_dict(d2, "hello.txt"), "rw"));

  // test update_dict

  update_dict(d2, "hi.txt", "rw");

  assert(string_equal(get_value_from_dict(d2, "hi.txt"), "rw"));

  // test remove_key_from_dict

  remove_key_from_dict(d2, "hi.txt");

  assert(!key_in_dict(d2, "hi.txt"));
}

void test_utils() {

  printf("Testing utils . . .\n");

  char words[15] = "Hello, World!\n";
  strip_newline(words);
  assert(string_equal(words, "Hello, World!"));

  char* words2 = "GET hi.txt ";

  char** command_split = split_str(words2, " ");

  assert(string_equal(command_split[0], "GET"));
  assert(string_equal(command_split[1], "hi.txt"));
  assert(command_split[2] == NULL);

  free(command_split);

  char** command_split2 = split_str("WRITE v1/jim.txt  rw", " ");

  assert(string_equal(command_split2[0], "WRITE"));
  assert(string_equal(command_split2[1], "v1/jim.txt"));
  assert(string_equal(command_split2[2], "rw"));
  assert(command_split2[3] == NULL);

  free(command_split2);

  test_write_data_to_file();

  test_dict();
}

void test_process_request() {
  // test process_request
  clear_filesystems();

  init_metadata();

  char* response;

  write_data_to_file("Hello, World!", "filesystems/server_fs/delete.txt");

  assert(fileExists("filesystems/server_fs/delete.txt"));

  response = process_request("RM delete.txt");

  assert(string_equal(response, "File deleted successfully!\n"));

  assert(!fileExists("filesystems/server_fs/delete.txt"));

  free(response);
  response = NULL;

  response = process_request("WRITE server_hi.txt rw Hello, World!");

  assert(string_equal(response, "File written successfully!\n"));

  free(response);
  response = NULL;

  FILE* file = fopen("filesystems/server_fs/server_hi.txt", "r");
  char* file_data = calloc(MAX_FILE_SIZE, sizeof(char));
  fread(file_data, sizeof(char), MAX_FILE_SIZE, file);
  fclose(file);
  assert(string_equal(file_data, "Hello, World!"));
  free(file_data);

  response = process_request("GET server_hi.txt");
  assert(string_equal(response, "File Data:\nHello, World!"));
  free(response);
  response = NULL;

  assert(fileExists("filesystems/server_fs/server_hi.txt"));
  response = process_request("RM server_hi.txt");
  assert(string_equal(response, "File deleted successfully!\n"));

  free(response);
  response = NULL;
  assert(!fileExists("filesystems/server_fs/server_hi.txt"));

  write_data_to_file("Hello!", "filesystems/server_fs/jims_files/v1/jim.txt");

  response = process_request("GET /jims_files/v1/jim.txt");

  assert(string_equal(response, "File Data:\nHello!"));

  clear_filesystems();

  write_data_to_file("Hello, World!", "filesystems/client_fs/hi.txt");

  process_request("WRITE server_hi.txt ro ello mate!");

  assert(fileExists("filesystems/server_fs/server_hi.txt"));

  response = process_request("LS server_hi.txt");
  assert(string_equal(response,
                      "Permissions for file server_hi.txt: Read-Only\n"));

  // test file perm stuff

  clear_filesystems();

  write_data_to_file("Hello, World!", "filesystems/client_fs/hi.txt");

  write_data_to_file("overwrite", "filesystems/client_fs/x.txt");

  assert(string_equal(process_request("WRITE server_hi.txt ro Hello, World!"),
                      "File written successfully!\n"));

  assert(string_equal(
      process_request("WRITE server_hi.txt rw overwrite"),
      "File is read only. Cannot write to it!\n")); // should fail since file is
                                                    // read-only
}

void test_commands() {
  clear_filesystems();

  printf("Testing commands . . .\n");

  test_write();

  // test delete

  test_delete();

  test_read();

  test_utils();

  printf("Command tests passed ✔\n");

  clear_filesystems();

  printf("Testing server commands . . .\n");

  test_process_request();

  printf("Server command tests passed ✔\n");
}

void test_prepare_message() {

  write_data_to_file("Hello, World!", "filesystems/client_fs/hi.txt");

  char* buffer = calloc(MAX_COMMAND_SIZE, sizeof(char));
  prepare_message(buffer, parse_message("WRITE hi.txt server_hi.txt ro"));
  assert(string_equal(buffer, "WRITE server_hi.txt ro Hello, World!"));
  memset(buffer, '\0', MAX_COMMAND_SIZE);

  prepare_message(buffer, parse_message("GET server_hi.txt client_hi.txt"));
  assert(string_equal(buffer, "GET server_hi.txt"));
  memset(buffer, '\0', MAX_COMMAND_SIZE);

  prepare_message(buffer, parse_message("RM server_hi.txt"));
  assert(string_equal(buffer, "RM server_hi.txt"));
  memset(buffer, '\0', MAX_COMMAND_SIZE);

  prepare_message(buffer, parse_message("GET hi.txt\n"));
  assert(string_equal(buffer, "GET hi.txt"));
  memset(buffer, '\0', MAX_COMMAND_SIZE);

  prepare_message(buffer, parse_message("RM hi.txt\n"));
  assert(string_equal(buffer, "RM hi.txt"));
  memset(buffer, '\0', MAX_COMMAND_SIZE);

  write_data_to_file("Hello, Jim!", "filesystems/client_fs/v1/jim.txt");

  client_message* pm = parse_message("WRITE v1/jim.txt  rw");
  prepare_message(buffer, pm);
  assert(string_equal(buffer, "WRITE v1/jim.txt rw Hello, Jim!"));
  memset(buffer, '\0', MAX_COMMAND_SIZE);
}

void test_validate_message() {
  assert(validate_message("WRITE hi.txt server_hi.txt ro\n") == 1);
  assert(validate_message("GET server_hi.txt client_hi.txt\n") == 1);
  assert(validate_message("WRITE hi.txt rw\n") == 1);
  assert(validate_message("GET server_hi.txt") == 1);
  assert(validate_message("RM server_hi.txt") == 1);

  assert(validate_message("RM priv/b.txt\n"));

  assert(validate_message("dummy command") == 0);
  assert(validate_message("dummy command") == 0);
  assert(validate_message("WRITE") == 0);
  assert(validate_message("GET") == 0);
  assert(validate_message("RM") == 0);
  assert(validate_message("rfs") == 0);
  assert(validate_message("") == 0);
  assert(validate_message("rf\ns WRITE hi.txt server_hi.txt") == 0);
}

void test_parse_message() {

  client_message* pm = parse_message("WRITE v1/jim.txt  rw");
  assert(string_equal(pm->command, "WRITE"));
  assert(string_equal(pm->localFilePath, "v1/jim.txt"));
  assert(pm->remoteFilePath == NULL);
  assert(string_equal(pm->permissions, "rw"));
}
void test_client_commands() {
  printf("Testing client commands . . .\n");
  clear_filesystems();
  test_parse_message();
  test_prepare_message();

  test_validate_message();
  printf("Client command tests passed ✔\n");
}

void test_remote_write(char* localText, char* localFile, char* remoteFile,
                       char* permissions) {
  /**
   * Tests that teh local file at <localFile>  with data <localText> is properly
   * written to the remote file at <remoteFile>.
   */
  clear_filesystems();

  char* passedRemoteFile = remoteFile;
  if (remoteFile == NULL) {
    passedRemoteFile = ""; // for testing when remote file is not provided
    remoteFile = localFile;
  }

  char* actualLocalFP = get_actual_fp(localFile, 0);
  write_data_to_file(localText, actualLocalFP);

  char* command = calloc(MAX_COMMAND_SIZE, sizeof(char));

  snprintf(command, MAX_COMMAND_SIZE, "WRITE %s %s %s", localFile,
           passedRemoteFile, permissions);

  assert(run_client(command) == 0);

  FILE* file = fopen(get_actual_fp(remoteFile, 1), "r");
  char* file_data = calloc(MAX_FILE_SIZE, sizeof(char));
  fread(file_data, sizeof(char), MAX_FILE_SIZE, file);
  fclose(file);
  assert(string_equal(file_data, localText));

  free(file_data);
  free(actualLocalFP);
}

void test_remote_get(char* remoteText, char* remoteFile, char* localFile) {
  /**
   * Tests that the remote file at <remoteFile> with data <remoteText> is
   * properly read and written to the local file at <localFile>.
   */
  clear_filesystems();

  char* passedLocalFile = localFile;
  if (localFile == NULL) {
    passedLocalFile = ""; // for testing when local file is not provided
    localFile = remoteFile;
  }

  write_data_to_file(remoteText, get_actual_fp(remoteFile, 1));

  char* command = calloc(MAX_COMMAND_SIZE, sizeof(char));

  snprintf(command, MAX_COMMAND_SIZE, "GET %s %s", remoteFile, passedLocalFile);

  assert(run_client(command) == 0);

  FILE* file = fopen(get_actual_fp(localFile, 0), "r");
  char* file_data = calloc(MAX_FILE_SIZE, sizeof(char));
  fread(file_data, sizeof(char), MAX_FILE_SIZE, file);
  fclose(file);
  assert(string_equal(file_data, remoteText));

  free(file_data);
}

void test_remote_delete() {
  // tests that the remote file is properly deleted
  clear_filesystems();

  write_data_to_file("Hello, World!", "filesystems/server_fs/server_hi.txt");

  assert(fileExists("filesystems/server_fs/server_hi.txt"));

  assert(run_client("RM server_hi.txt") == 0);

  assert(!fileExists("filesystems/server_fs/server_hi.txt"));
}

void test_remote_file_perms() {
  // tests that the remote file permissions is functional

  clear_filesystems();

  write_data_to_file("Hello, World!", "filesystems/client_fs/hi.txt");

  write_data_to_file("overwrite", "filesystems/client_fs/x.txt");

  assert(run_client("WRITE hi.txt server_hi.txt ro") == 0);

  metadata =
      load_dict_from_file(get_actual_fp(".metadata", 1)); // refresh metadata

  assert(string_equal(get_value_from_dict(metadata, "server_hi.txt"),
                      "ro")); // check that metadata is stored

  assert(run_client("WRITE x.txt server_hi.txt rw") ==
         1); // should fail since file is read-only

  FILE* file0 = fopen(get_actual_fp("server_hi.txt", 1), "r");
  char* file_data0 = calloc(MAX_FILE_SIZE, sizeof(char));
  fread(file_data0, sizeof(char), MAX_FILE_SIZE, file0);
  fclose(file0);
  assert(string_equal(file_data0,
                      "Hello, World!")); // making sure data was not overwritten
  free(file_data0);
  metadata =
      load_dict_from_file(get_actual_fp(".metadata", 1)); // refresh metadata

  assert(string_equal(get_value_from_dict(metadata, "server_hi.txt"),
                      "ro")); // check that metadata is stored

  assert(run_client("RM server_hi.txt") ==
         1); // should fail since file is read-only

  metadata =
      load_dict_from_file(get_actual_fp(".metadata", 1)); // refresh metadata

  assert(string_equal(get_value_from_dict(metadata, "server_hi.txt"),
                      "ro")); // check that metadata is stored

  assert(fileExists("filesystems/server_fs/server_hi.txt") == 1);

  assert(run_client("GET server_hi.txt client_hi.txt") == 0);

  FILE* file = fopen(get_actual_fp("client_hi.txt", 0), "r");
  char* file_data = calloc(MAX_FILE_SIZE, sizeof(char));
  fread(file_data, sizeof(char), MAX_FILE_SIZE, file);
  fclose(file);
  assert(string_equal(
      file_data,
      "Hello, World!")); // just making sure metadata is not stored in the file
  free(file_data);

  metadata =
      load_dict_from_file(get_actual_fp(".metadata", 1)); // refresh metadata

  assert(string_equal(get_value_from_dict(metadata, "server_hi.txt"),
                      "ro")); // check that metadat is stored

  clear_filesystems();

  write_data_to_file("Hello, World!", "filesystems/client_fs/hi.txt");

  write_data_to_file("overwrite", "filesystems/client_fs/x.txt");

  assert(run_client("WRITE hi.txt server_hi.txt rw") == 0);

  metadata =
      load_dict_from_file(get_actual_fp(".metadata", 1)); // refresh metadata

  assert(string_equal(get_value_from_dict(metadata, "server_hi.txt"),
                      "rw")); // check that metadat is stored

  assert(run_client("WRITE x.txt server_hi.txt ro") == 0); // should  succeed

  FILE* file2 = fopen(get_actual_fp("server_hi.txt", 1), "r");
  char* file_data2 = calloc(MAX_FILE_SIZE, sizeof(char));
  fread(file_data2, sizeof(char), MAX_FILE_SIZE, file2);
  fclose(file2);
  assert(string_equal(file_data2,
                      "overwrite")); // making sure data was overwritten
  free(file_data2);

  metadata =
      load_dict_from_file(get_actual_fp(".metadata", 1)); // refresh metadata

  assert(string_equal(get_value_from_dict(metadata, "server_hi.txt"),
                      "rw")); // check that metadat is stored

  assert(run_client("GET server_hi.txt client_hi.txt") == 0);

  assert(run_client("RM server_hi.txt") ==
         0); // should succeed since file is read-write

  assert(!fileExists("filesystems/server_fs/server_hi.txt"));

  metadata =
      load_dict_from_file(get_actual_fp(".metadata", 1)); // refresh metadata

  assert(get_value_from_dict(metadata, "server_hi.txt") ==
         NULL); // check that metadata is removed

  clear_filesystems();
}

void test_server() {

  clear_filesystems();

  // tests that the client can execute commands on the server
  printf("Testing server . . .\n");

  pid_t parent_pid = getpid();
  int server_pid = fork();

  if (server_pid == 0) {

    if (run_server() != 0) {
      printf("Error starting server\n");

      printf("Killing parent process . . .\n");

      // kill parent process
      kill(parent_pid, SIGKILL);

      exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);

  } else {
    sleep(1); // wait for server to start

    test_remote_file_perms();

    test_remote_delete();

    test_remote_write("Hello, World!", "hi.txt", "server_hi.txt", "rw");

    test_remote_write("Hello, Jim", "v1/jim.txt", "/jims_files/v1/jim.txt",
                      "ro");

    test_remote_write("Hello, Jim\nHello Bob!", "v1/jim.txt",
                      "/bobs_files/v1/jim.txt",
                      "rw"); // testing case where there are 2 lines

    test_remote_write("Hello, Jim_missing_remote!", "v1/jim.txt", NULL,
                      "rw"); // testing case where remote file is not provided

    char* large_buffer = calloc(MAX_FILE_SIZE, sizeof(char));

    for (int i = 0; i < MAX_FILE_SIZE; i++) {
      large_buffer[i] = 'a' + (i % 26);

      if (i == MAX_FILE_SIZE - 1) {
        large_buffer[i] = '\0';
      }
    }

    test_remote_write(large_buffer, "large.txt", "server_large.txt",
                      "rw"); // writing large amount of datat to server

    test_remote_get("Hello, World!", "server_hi.txt", "client_hi.txt");

    test_remote_get("Hello, Jim!", "/jims_files/v1/jim.txt", "client_jim.txt");

    test_remote_get("Hello, Jim_missing_local!", "/jims_files/v1/jim.txt",
                    NULL);

    test_remote_get("Hello, Jim!\n Hello Bob!", "/jims_files/v1/jim.txt",
                    "client_jim.txt"); // testing read with 2 lines

    clear_filesystems(); // clear filesystems before running tests

    // testing invalid commands

    assert(run_client("dummy command") == 1);

    assert(run_client("dummy command") == 1);

    kill(server_pid, SIGKILL); // kill the server
  }

  clear_filesystems(); // clear filesystems after running tests

  if (server_pid != 0) {
    // wait for all child processes to finish
    printf("Waiting for server to close . . .\n");
    wait(NULL);
    printf("Server closed\n");

    printf("Server test passed ✔\n");
  }
}

void test_metadata() {
  printf("Testing metadata . . .\n");

  clear_filesystems();

  assert(!key_in_dict(metadata, "hi.txt"));

  process_request("WRITE hi.txt ro Hello, World!"); // write a read-only file

  assert(key_in_dict(metadata, "hi.txt"));

  assert(string_equal(get_value_from_dict(metadata, "hi.txt"), "ro"));
}

void test_concurrency() {
  clear_filesystems();
  printf("Testing server's ability to handle concurrent requests . . .\n");

  pid_t parent_pid = getpid();

  int server_pid = fork();

  if (server_pid == 0) {
    printf("Starting server . . .\n");
    if (run_server() != 0) {
      printf("Error starting server\n");

      printf("Killing parent process . . .\n");
      // kill parent process
      kill(parent_pid, SIGKILL);
      exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);

  } else {
    sleep(1); // wait for server to start
  }

  generate_random_file(
      "filesystems/client_fs/hi.txt",
      MAX_FILE_SIZE); // create a random <N> MB file to write to the server

  int num_processes = NUM_CONCURRENT_REQUESTS;
  pid_t pids[num_processes];
  // create 10 child processes to send requests to the server concurrently
  for (int i = 0; i < num_processes; i++) {
    pids[i] = fork();
    if (pids[i] == 0) {
      // child process

      char* command = calloc(MAX_COMMAND_SIZE, sizeof(char));
      snprintf(command, MAX_COMMAND_SIZE, "WRITE hi.txt server_hi_%d.txt rw",
               i);

      if (run_client(command) != 0) {
        printf("Error running client\n");
        exit(EXIT_FAILURE);
      }
      exit(EXIT_SUCCESS); // so that the child process doesn't keep forking
    }
  }

  // Parent process waits for all child processes to terminate, to make sure all
  // processes ended successfully
  int success = 1;
  for (int i = 0; i < num_processes; i++) {
    int status;
    printf("Waiting for child process %d to finish . . .\n", pids[i]);
    waitpid(pids[i], &status, 0);
    assert(WIFEXITED(status));

    int exit_status = WEXITSTATUS(status);
    if (exit_status != EXIT_SUCCESS) {
      printf("Child process %d exited with status %d\n", pids[i], exit_status);
      success = 0;
    } else {
      printf("Child process %d succeeded!\n", pids[i]);
    }
  }

  printf("Closing the server . . .\n");
  // kill the server

  kill(server_pid, SIGKILL); // kill the server

  if (success) {
    printf("All child processes succeeded!\n");
  } else {
    printf("Some child processes failed\n");
    exit(1);
  }

  // verify that the files written to the server are correct

  for (int i = 0; i < num_processes; i++) {

    char* remoteFileName = calloc(MAX_COMMAND_SIZE, sizeof(char));
    snprintf(remoteFileName, MAX_COMMAND_SIZE, "server_hi_%d.txt", i);

    FILE* remoteFile = fopen(get_actual_fp(remoteFileName, 1), "r");
    char* remoteFileData = calloc(MAX_FILE_SIZE, sizeof(char));
    fread(remoteFileData, sizeof(char), MAX_FILE_SIZE, remoteFile);
    fclose(remoteFile);

    FILE* localFile = fopen("filesystems/client_fs/hi.txt", "r");
    char* localFileData = calloc(MAX_FILE_SIZE, sizeof(char));

    fread(localFileData, sizeof(char), MAX_FILE_SIZE, localFile);

    assert(string_equal(remoteFileData, localFileData));

    fclose(localFile);

    free(localFileData);
    free(remoteFileData);
  }

  // clear_filesystems();
  printf("Concurrency test passed ✔\n");
}

int main() {
  clear_filesystems();
  test_commands();
  test_client_commands();
  test_server();
  test_concurrency();

  return 0;
}