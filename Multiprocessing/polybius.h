#ifndef POLYBIUS_H
#define POLYBIUS_H

#define TABLE_SIZE 6

typedef struct {
    char table[TABLE_SIZE][TABLE_SIZE];
} PolybiusTable_t;

void buildPolybiusTable(PolybiusTable_t *table);
char* pbEncode(const char *plaintext, const PolybiusTable_t *table);
char* pbDecode(const char *ciphertext, const PolybiusTable_t *table);

#endif // POLYBIUS_H
