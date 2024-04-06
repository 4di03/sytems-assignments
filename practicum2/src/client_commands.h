
void prepare_message(char* buffer, char* raw_message);

void handle_response(char* server_message , char* client_message);

int run_client(char* given_command);

int validate_message(char* message);