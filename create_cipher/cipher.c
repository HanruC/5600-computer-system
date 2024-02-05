#include "polybius.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Input should be: %s <encode|decode> <plaintext|ciphertext>\n", argv[0]);
        return 1;
    }

    PolybiusTable_t table; 
    buildPolybiusTable(&table); // build the table 

    char *result = NULL;
    if (strcmp(argv[1], "-e") == 0) { // encoding
        result = pbEncode(argv[2], &table);
    } else if (strcmp(argv[1], "-d") == 0) { // decoding
        result = pbDecode(argv[2], &table);
    } else {
        fprintf(stderr, "Invalid option: %s\n", argv[1]);
        return 1;
    }

    if (result) {
        printf("%s\n", result);
        free(result);
    } else {
        fprintf(stderr, "Error occurred.\n");
    }

    return 0;
}