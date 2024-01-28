



char capitalizeChar(char c);

char lowercaseChar(char c);


int stringEquals(char* s1, char* s2);


char* upper(char* s);

char* lower(char* s);

char* capitalize(char* s);

typedef char* (*stringFunc)(char*);

void applyToSentence(stringFunc f, char* s[], int n);
