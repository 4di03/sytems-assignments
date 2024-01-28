/*
 * polybius.c / Create Cipher
 *
 * Adithya Palle / CS5600 / Northeastern University
 * Spring 2024 / Jan 23, 2024
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "polybius.h"


int charToInt(char digitChar) {
    /**
     * Converts a character representing a digit to an integer.
     * 
    */
    if (digitChar >= '0' && digitChar <= '9') {
        return digitChar - '0';
    } else {
        // Handle error or return a default value
        fprintf(stderr, "Error: Not a valid digit character: %c \n", digitChar);
        exit(1); // Return a default value or indicate an error
    }
}


char intToChar(int n){
    /**
     * Converts an integer to a character representing a digit.
     * args:
     *      n, int: integer to convert to character
     * returns:
     *     char: character representing digit
    */
    return n + '0';
}

char lowercase(char c){
    /**
     * Converts a character to lowercase.
     * args:
     *      c, char: character to convert to lowercase
     * returns:
     *     char: lowercase version of c
    */
    if (c >= 'A' && c <= 'Z'){
        return c + 32;
    }
    return c;

}

char* encodeChar(const table_t table, const char letter){
    /**
     * Encodes a character using a polybius table.
     * args:
     *      table , table_t: Polybius table for encoding
     *      letter, char: character to encode, if the letter is j it will be treated as i
     * returns:
     *     char*: string of length 2 which represents a number to decode using teh table, if character is not recognized, return empty string
    */

    char lower = lowercase(letter);

    if (lower == 'j'){
        lower = 'i'; // treat j as i
    }

    for (int i = 0; i < 5 ; i++){
        for (int j =0 ; j < 5; j++){
            if (table.table[i][j] == lower){
                char*  ret = malloc(sizeof(char)*2);
                sprintf(ret, "%d%d",i+1,j+1);
                return ret;
            }
        }
    }

    return ""; // if character is not recognized, ignore it and return empty string
}

char decodeNumber(const table_t table, const char numberString[2]){
    /**
     * args:
     *      table , table_t: Polybius table for decoding
     *      numberString, char*: string of length 2 which represents a number to decode using teh table
     * returns:
     *      char : character that is decoded form of number
    */

    return table.table[charToInt(numberString[0])-1][charToInt(numberString[1])-1];
}

int getStringSize(const char* str){
    /**
     * Gets the length of a string.
     * args:
     *     str, char*: string to get size of
     * returns:
     *    int: size of string
    */
    int i = 0;
    while (str[i] != '\0'){
        i++;
    }

    return i;
}

char* pbEncode(const char* plaintext, table_t table){
    /**
     * Encodes a string using a polybius table.
     * args:
     *     table , table_t: Polybius table for encoding
     *     plaintext, char*: string to encode
     * returns:
     *   char*: encoded string
    */

    char* ciphertext = malloc(sizeof(char)*getStringSize(plaintext) * 2 + 1);// because each char can be decoded into 2 digit numbers
    char curChar;
    int i = 0;
    int j = 0;
    while (plaintext[i] != '\0'){

        char nextChar;
        char* encoded = encodeChar(table,plaintext[i]);

        if ((plaintext[i] == ' ') || encoded == ""){
            ciphertext[j] = plaintext[i]; // if its space or an unknown character, just append that character to decoded result
            j += 1;
        } else{
            ciphertext[j] = encoded[0];
            ciphertext[j+1] = encoded[1];
            j += 2;
        }
        i++;
    }

    ciphertext[j]  = '\0';
    j++;

    ciphertext = realloc(ciphertext, sizeof(char)*j);


    return ciphertext;
}


int isDigit(char c){
    /**
     * Checks if a character is a digit.
     * args:
     *     c, char: character to check
     * returns:
     *    int: 1 if c is a digit, 0 otherwise
    */
    return c >= '0' && c <= '9';
}

int isLetter(char c){
    /**
     * Checks if a character is a letter.
     * args:
     *     c, char: character to check
     * returns:
     *    int: 1 if c is a letter, 0 otherwise
    */
    return (lowercase(c) >= 'a' && lowercase(c) <= 'z');
}

char* pbDecode(const char* ciphertext, table_t table){
    /**
     * Decodes a string using a polybius table.
     * args:
     *     table , table_t: Polybius table for decoding
     *     ciphertext, char*: string to decode
     * returns:
     *   char*: decoded string
    */

    char* plaintext = malloc(sizeof(char)*getStringSize(ciphertext));
    int i = 0;
    int j = 0;
    while (ciphertext[i] != '\0'){
        if (isDigit(ciphertext[i])){

            if (!isDigit(ciphertext[i+1])){
                printf("Error: Invalid ciphertext: %s, ciphertext should have an even number of digits that are 2-consecutive\n", ciphertext);
                exit(1);
            }


            char curNum[3] = {ciphertext[i], ciphertext[i+1], '\0'};
            plaintext[j] = decodeNumber(table,curNum);
            i+=2;

        }else if (isLetter(ciphertext[i])){
            printf("Error: Invalid character in ciphertext: %c, ciphertext should not contain letters\n", ciphertext[i]);
            exit(1);
        } else{ // non-letter, non-digit characters should be left as is when decoding
            plaintext[j] = ciphertext[i];
            i++;
        }
        j++;


    }

    plaintext[i] = '\0';
    i++;

    plaintext = realloc(plaintext, sizeof(char)*i);

    return plaintext;

}

void testCipher(){

    table_t table  = {
        {
            {'a','b','c','d','e'},
            {'f','g','h','i','k'},
            {'l','m','n','o','p'},
            {'q','r','s','t','u'},
            {'v','w','x','y','z'}
        }
    };

    table_t shuffledTable = {
        {
            {'f','x','a','m','c'},
            {'i','d','b','g','e'},
            {'k','y','h','n','o'},
            {'p','q','w','s','t'},
            {'u','v','r','l','z'}
        }
    };

    assert(isLetter('a')==1);

    char* plaintext = "hello world";
    char* ciphertext = pbEncode(plaintext, table);
    char* decoded = pbDecode(ciphertext, table);
    assert(strcmp(plaintext, decoded) == 0);
    
    assert(strcmp(pbEncode(plaintext, table), pbEncode(plaintext, shuffledTable)) != 0);


    assert(strcmp(pbEncode("j",table) , pbEncode("i",table))== 0);
    assert(strcmp(pbDecode("24",table) ,"i")==0);

    assert(strcmp(pbEncode("hey!", table) ,"231554!") == 0);

    assert(strcmp(pbEncode("hey!", shuffledTable) ,"332532!") == 0); // testing non-letter characters
    assert(strcmp(pbDecode("332532!", shuffledTable) , "hey!") == 0);


    pbEncode("asdjkadaksljdalfjalsfjalgsalkfjla aslkdj alskdja ldjalksdj alksdjlaks jdlkadalkdjmalksjdalksdjlaksjdlkajdlkajsdlkasjdlkajdslkasd,mxmc,lamndlkan",table); //testing long strings

    printf("All test cases passed!\n");
}