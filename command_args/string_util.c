
#include <stdio.h>
#include "string_util.h"



char capitalizeChar(char c){
    /**
     * Takes in a character and returns the capitalized version of it
     * args:
     *      c : char = character to be capitalized
     * returns:
     *      char : capitalized version of c
    */
    if (c >= 'a' && c <= 'z'){
        return c - 32;
    }
    return c;
}

char lowercaseChar(char c){
    /**
     * Takes in a character and returns the lowercase version of it
     * args:
     *      c : char = character to be capitalized
     * returns:
     *      char : capitalized version of c
    */
    if (c >= 'A' && c <= 'Z'){
        return c + 32;
    }
    return c;
}

int stringEquals(char* s1, char* s2){
    /**
     * Takes in two strings and returns 1 if they are equal and 0 if they are not
     * args:
     *      s1 : char* = first string
     *      s2 : char* = second string
     * returns:
     *      int : 1 if s1 == s2, 0 otherwise
    */
    int i = 0;
    while (s1[i] == s2[i] && s1[i] != '\0' && s2[i] != '\0'){
        i++;
    }

    return (s1[i] == '\0' && s2[i] == '\0');
}


char* upper(char* s){
    /**
     * Takes in a string and returns the uppercase version of it
     * args:
     *      s : char* = string to be capitalized
     * returns:
     *      char* : capitalized version of s
    */
    int i = 0;
    while (s[i] != '\0'){
        s[i] = capitalizeChar(s[i]);
        i++;
    }
    return s;
}

char* lower(char* s){
    /**
     * Takes in a string and returns the lowercase version of it
     * args:
     *      s : char* = string to be capitalized
     * returns:
     *      char* : capitalized version of s
    */
    int i = 0;
    while (s[i] != '\0'){
        if (s[i] >= 'A' && s[i] <= 'Z'){
            s[i] = lowercaseChar(s[i]);
        }
        i++;
    }
    return s;
}

char* capitalize(char* s){
    /**
     * Takes in a string and returns the capitalized version of it
     * args:
     *      s : char* = string to be capitalized
     * returns:
     *      char* : capitalized version of s
    */
    int i = 0;

    while (s[i] != '\0'){

        if (i == 0){
            s[i] = capitalizeChar(s[i]);
        }
        else{
            s[i] = lowercaseChar(s[i]);
        }
        i++;
    }

    return s;
}


void applyToSentence(stringFunc f, char* s[], int n){
    /**
     * Takes in a string and applies the function f to each word in the string, printing the resulting output
     * args:
     *      f : stringFunc = function to be applied to each word
     *      s : char* = string to be capitalized
     * returns:
     *      char* : capitalized version of s
    */

    for (int i = 0; i < n; i++){
        printf("%s ", f(s[i]));
    }
    printf("\n");
}
