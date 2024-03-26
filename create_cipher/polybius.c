#include "polybius.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

void buildPolybiusTable(PolybiusTable_t *table) {
    char codes[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int index = 0;
    for (int i = 0; i < TABLE_SIZE; ++i) {
        for (int j = 0; j < TABLE_SIZE; ++j) {
            table->table[i][j] = codes[index++];
        }
    }
}

static int helperFindChar(char c, const PolybiusTable_t *table, int *row, int *col) {
    for (int i = 0; i < TABLE_SIZE; ++i) {
        for (int j = 0; j < TABLE_SIZE; ++j) {
            if (table->table[i][j] == c) {
                *row = i;
                *col = j;
                return 1;
            }
        }
    }
    return 0;
}

char* pbEncode(const char *plaintext, const PolybiusTable_t *table) {
    size_t len = strlen(plaintext);
    char *encoded = (char*)malloc(len * 2 + 1); //row and col for each char + 1 for null terminator
    if (encoded == NULL) {
        return NULL;
    }

    int row, col;
    size_t index = 0;
    for (size_t i = 0; i < len; i++) {
        char c = toupper(plaintext[i]); // convert to upper case 
        if (helperFindChar(c, table, &row, &col)) {
            encoded[index++] = row + '1'; // row originally 0-5 so add 1 to convert to char
            encoded[index++] = col + '1'; // col originally 0-5 so add 1 to convert to char
        }
    }
    encoded[index] = '\0';
    return encoded;
}

char* pbDecode(const char *ciphertext, const PolybiusTable_t *table) {
    size_t len = strlen(ciphertext); 
    if (len % 2 != 0) {
        fprintf(stderr, "Error: Ciphertext length is not even.\n");
        return NULL; // invalid length
    }

    char *decoded = (char*)malloc(len / 2 + 1); // half the length of ciphertext + 1 for null terminator
    if (decoded == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return NULL;
    }

    size_t index = 0;
    for (size_t i = 0; i < len; i += 2) {
        int row = ciphertext[i] - '1'; // convert to int by subtracting 1
        int col = ciphertext[i + 1] - '1'; // convert to int by subtracting 1
        if (row >= 0 && row < TABLE_SIZE && col >= 0 && col < TABLE_SIZE) {
            decoded[index++] = table->table[row][col];
        } else {
            free(decoded);
            fprintf(stderr, "Error: Invalid row or col in ciphertext.\n");
            return NULL; // invalid row or col
        }
    }
    decoded[index] = '\0';
    return decoded;
}