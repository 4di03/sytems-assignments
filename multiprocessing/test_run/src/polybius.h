/*
 * polybius.h / Create Cipher
 *
 * Adithya Palle / CS5600 / Northeastern University
 * Spring 2024 / Jan 23, 2024
 *
 */

typedef struct table_t
{
    /**
     * 5x5 polybius square representing the the 26 alphabetic letters (i and j are equivalent)
     *
     * If a string with j is encoded, it will be read back as i when decoding, as the table treats i and j as the same character.
     * Characters that are not letters will be left untouched when encoding, and will not appear in any decoded result.
     *
     * The decoded numbers are in range of [1,5]
     */

    int table[5][5];

} table_t;


static table_t shuffledTable = {{{'f', 'x', 'a', 'm', 'c'},
                                 {'i', 'd', 'b', 'g', 'e'},
                                 {'k', 'y', 'h', 'n', 'o'},
                                 {'p', 'q', 'w', 's', 't'},
                                 {'u', 'v', 'r', 'l', 'z'}}}; // default table for polybius cipher

char lowercase(char c);

int charToInt(char digitChar);

char intToChar(int n);

char *encodeChar(const table_t table, const char letter);

char decodeNumber(const table_t table, const char numberString[2]);

int getStringSize(const char *str);

char *pbEncode(const char *plaintext, table_t table);

int isDigit(char c);

int isLetter(char c);

char *pbDecode(const char *ciphertext, table_t table);

void testCipher(); // function to run tests for all cipher functions