#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    int *data;
    size_t size;
} dynBlock; 

dynBlock* allocDynBlock(size_t size) {
    dynBlock *block = malloc(sizeof(dynBlock));
    if (block == NULL) {
        return NULL;
    }

    block->data = malloc(size * sizeof(int));

    if (block->data == NULL) {
        free(block);
        return NULL;
    }

    block->size = size;
    return block;
}

void storeMem2Blk(dynBlock *block, int *array, size_t size) {
    if (block == NULL || block->data == NULL || size > block->size) {
        return;
    }
    for (size_t i = 0; i < size; i++) {
        block->data[i] = array[i];
    }
}

void freeDynBlock(dynBlock *block) {
    if (block == NULL) {
        return;
    }
    free(block->data);
    free(block);
}

void parseLine(char *line) {
    size_t count = 0;
    char *token, *endptr;
    char *tempLine = strdup(line); // duplicate line to avoid modifying original line

    token = strtok(tempLine, " \n");

    // count the number of int in each line
    while (token != NULL) {
        count++;
        token = strtok(NULL, " \n");
    }
    free(tempLine);

    if (count == 0) return; // skip empty line if possible. 

    //allocate dynBlock for counted int 
    dynBlock *block = allocDynBlock(count);
    if (block == NULL) {
        fprintf(stderr, "Failed to allocate dynBlock\n");
        return;
    }

    // store int in dynBlock
    token = strtok(line, " \n");
    for (size_t i = 0; i < count; i++) {
        block->data[i] = strtol(token, &endptr, 10);
        if (token == endptr) {  // Check for conversion errors
            fprintf(stderr, "Conversion error occurred\n");
            freeDynBlock(block);
            return;
        }
        token = strtok(NULL, " \n");
    }

    for (size_t i = 0; i < block->size; i++) {
        printf("%d ", block->data[i]);
    }

    printf("\n");

    freeDynBlock(block);
}

int main() {
    FILE *file = fopen("blocks.data", "r");
    if (file == NULL) {
        perror("Failed to open file");
        return -1;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        parseLine(line);
    }

    fclose(file);
    return 0;
}
