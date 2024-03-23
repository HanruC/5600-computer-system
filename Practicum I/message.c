#include "message.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h> 

// Creates a new message with the provided fields and returns a pointer
Message* create_msg(int id, const char* time_sent, const char* sender, const char* receiver, const char* content) {
    // Allocate memory for the message
    Message* msg = (Message*)malloc(sizeof(Message));

    // set the fields of the message struct using the provided arguments
    msg->id = id;
    msg->time_sent = strdup(time_sent);
    msg->sender = strdup(sender);
    msg->receiver = strdup(receiver);
    msg->content = strdup(content);
    msg->delivered = false;

    // return the pointer to the message
    return msg;
}

// Stores the message on disk using a unique filename based on the message id
void store_msg(Message* msg) {
    // Generate a unique filename based on the message id
    char filename[20];
    sprintf(filename, "msg_%d.txt", msg->id);

    // Open the file for writing mode
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Unable to store message %d\n", msg->id);
        return;
    }

    // Write the message fields to the file
    fprintf(file, "%d\n%s\n%s\n%s\n%s\n%d\n", msg->id, msg->time_sent, msg->sender, msg->receiver, msg->content, msg->delivered);
    fclose(file);
}

// Retrieves a message from disk based on the message id and returns a pointer
Message* retrieve_msg(int id) {
    // Generate the filename based on the message id
    char filename[20];
    sprintf(filename, "msg_%d.txt", id);

    // Open the file for reading mode
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Message %d not found\n", id);
        return NULL;
    }

    // Allocate memory for the message
    Message* msg = (Message*)malloc(sizeof(Message));

    // Read the message fields from the file
    char delivered[10];
    fscanf(file, "%d\n", &(msg->id));

    //allocate memory for the strings fields and read them from the file
    msg->time_sent = (char*)malloc(100 * sizeof(char));
    fgets(msg->time_sent, 100, file);
    msg->time_sent[strcspn(msg->time_sent, "\n")] = '\0';

    msg->sender = (char*)malloc(100 * sizeof(char));
    fgets(msg->sender, 100, file);
    msg->sender[strcspn(msg->sender, "\n")] = '\0';

    msg->receiver = (char*)malloc(100 * sizeof(char));
    fgets(msg->receiver, 100, file);
    msg->receiver[strcspn(msg->receiver, "\n")] = '\0';

    msg->content = (char*)malloc(1000 * sizeof(char));
    fgets(msg->content, 1000, file);
    msg->content[strcspn(msg->content, "\n")] = '\0';

    fscanf(file, "%s\n", delivered);
    msg->delivered = (strcmp(delivered, "1") == 0);

    fclose(file);

    return msg;
}

// Frees the memory allocated for the message
void free_msg(Message* msg) {
    free(msg->time_sent);
    free(msg->sender);
    free(msg->receiver);
    free(msg->content);
    free(msg);
}