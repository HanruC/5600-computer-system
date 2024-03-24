#include "message.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Creates a new message with the provided fields and returns a pointer to the dynamically allocated Message object.
Message* create_msg(int id, const char* time_sent, const char* sender, const char* receiver, const char* content) {
    Message* msg = (Message*)malloc(sizeof(Message));
    msg->id = id;
    msg->time_sent = strdup(time_sent);
    msg->sender = strdup(sender);
    msg->receiver = strdup(receiver);
    msg->content = strdup(content);
    msg->delivered = false;
    return msg;
}

// Stores the message in a message store on disk using a simple file format.
void store_msg(Message* msg) {
    char filename[256];
    sprintf(filename, "msg_%d.txt", msg->id);
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Unable to store message %d\n", msg->id);
        return;
    }
    fprintf(file, "%d\n%s\n%s\n%s\n%s\n%d\n", msg->id, msg->time_sent, msg->sender, msg->receiver, msg->content, msg->delivered ? 1 : 0);
    fclose(file);
}

// Retrieves a message from the message store on disk based on its identifier.
Message* retrieve_msg(int id) {
    char filename[256];
    sprintf(filename, "msg_%d.txt", id);
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Message %d not found\n", id);
        return NULL;
    }
    Message* msg = (Message*)malloc(sizeof(Message));
    fscanf(file, "%d\n", &(msg->id));
    msg->time_sent = (char*)malloc(256 * sizeof(char));
    fgets(msg->time_sent, 256, file);
    msg->time_sent[strcspn(msg->time_sent, "\n")] = '\0';
    msg->sender = (char*)malloc(256 * sizeof(char));
    fgets(msg->sender, 256, file);
    msg->sender[strcspn(msg->sender, "\n")] = '\0';
    msg->receiver = (char*)malloc(256 * sizeof(char));
    fgets(msg->receiver, 256, file);
    msg->receiver[strcspn(msg->receiver, "\n")] = '\0';
    msg->content = (char*)malloc(1024 * sizeof(char));
    fgets(msg->content, 1024, file);
    msg->content[strcspn(msg->content, "\n")] = '\0';
    int delivered;
    fscanf(file, "%d\n", &delivered);
    msg->delivered = (delivered == 1);
    fclose(file);
    return msg;
}

// Frees the dynamically allocated memory used by the Message object and its string fields.
void free_msg(Message* msg) {
    free(msg->time_sent);
    free(msg->sender);
    free(msg->receiver);
    free(msg->content);
    free(msg);
}