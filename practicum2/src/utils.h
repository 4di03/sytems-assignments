
int string_equal(char* str1, char* str2);

char* get_actual_fp(char* filePath, int remote);


char** split_str(char* str, char* delim);

int directory_exists(char* dirPath);

void make_parent_dirs(char* filePath);


char* write_data_to_file(char* fileData, char* filePath);