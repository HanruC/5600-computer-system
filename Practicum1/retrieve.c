// retrieve.c
#include "message.h"
#include <stdio.h>
#include <stdlib.h>


Message* retrieve_msg(int unique_id) {
    char filename[256];
    sprintf(filename, "message_%d.dat", unique_id);

    FILE* file = fopen(filename, "rb");
    if (!file) return NULL;

    Message* msg = malloc(sizeof(Message));
    if (fread(msg, sizeof(Message), 1, file) != 1) {
        free(msg);
        fclose(file);
        return NULL;
    }

    fclose(file);
    return msg;
}
