int fileExists(const char* path);

int string_equal(char* str1, char* str2);

char* get_actual_fp(char* filePath, int remote);

void strip_newline(char* str);
char** split_str(char* str, char* delim);

int directory_exists(char* dirPath);

void make_parent_dirs(char* filePath);

char* write_data_to_file(char* fileData, char* filePath);

void generate_random_file(char* filePath, int size);

long send_all(int s, char* buf, long len);
long receive_all(int s, char* buffer, long len);

// dictionary data structure + functions

typedef struct dict {
  int size;      // size of dictionary
  char** keys;   // array of keys
  char** values; // array of values
} dict;

void dict_to_file(dict* d, char* filePath);

dict* load_dict_from_file(char* filePath);

char* get_value_from_dict(dict* d, char* key);

void update_dict(dict* d, char* key, char* value);

void remove_key_from_dict(dict* d, char* key);
int key_in_dict(dict* d, char* key);