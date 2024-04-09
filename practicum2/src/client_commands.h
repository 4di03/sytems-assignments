typedef struct client_message {
  char* command;
  char* localFilePath;
  char* remoteFilePath;
  char* permissions;
} client_message;

client_message* parse_message(char* message);

void prepare_message(char* buffer, client_message* cm);

int handle_response(char* server_message, char* client_message);

int run_client(char* given_command);

int validate_message(char* message);
