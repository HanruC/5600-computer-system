#include "message.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Creates a new message with the provided fields and returns a pointer to the dynamically allocated Message object.
Message* create_msg(int id, const char* sender, const char* receiver, const char* content) {
    // Dynamically allocate memory for the Message struct
    Message* msg = (Message*)malloc(sizeof(Message));
    
    // Set the fields of the Message struct using the provided arguments and placeholder values
    msg->id = id;
    msg->time_sent = strdup("placeholder_time");
    msg->sender = strdup(sender);
    msg->receiver = strdup(receiver);
    msg->content = strdup(content);
    msg->delivered = false;
    
    // Return the pointer to the created Message object
    return msg;
}

// Stores the message on disk using a binary file format and updates the index file.
void store_msg(Message* msg) {
    // Generate a unique filename based on the message ID
    char filename[256];
    sprintf(filename, "msg_%d.bin", msg->id);
    
    // Open the file in write mode
    FILE* file = fopen(filename, "wb");
    if (file == NULL) {
        printf("Error: Unable to store message %d\n", msg->id);
        return;
    }
    
    // Write the message fields to the binary file
    fwrite(&(msg->id), sizeof(int), 1, file);
    fwrite(msg->time_sent, sizeof(char), strlen(msg->time_sent) + 1, file);// include the null terminator
    fwrite(msg->sender, sizeof(char), strlen(msg->sender) + 1, file);
    fwrite(msg->receiver, sizeof(char), strlen(msg->receiver) + 1, file);
    fwrite(msg->content, sizeof(char), strlen(msg->content) + 1, file);
    fwrite(&(msg->delivered), sizeof(bool), 1, file);
    
    // Close the file
    fclose(file);
    
    // Open the index file in append mode
    FILE* index_file = fopen("index.txt", "a");
    if (index_file == NULL) {
        printf("Error: Unable to open index file\n");
        return;
    }
    
    // Write the message ID and file offset to the index file by using ftell
    // file offset is file position indicator which is the number of bytes from the beginning of the file
    // Index file serves as a lookup table for quick retrieval of messages
    // write the message ID and the file offset to the index file
    long offset = ftell(file);
    fprintf(index_file, "%d %ld\n", msg->id, offset);
    
    // Close the index file
    fclose(index_file);
}

// Retrieves a message from disk based on its identifier using the index file.
// using index file to quickly locate the message file 
// no longer need to read through all the message files to find the one with the matching ID
Message* retrieve_msg(int id) {
    // Open the index file in read mode
    FILE* index_file = fopen("index.txt", "r");
    if (index_file == NULL) {
        printf("Error: Unable to open index file\n");
        return NULL;
    }
    
    // Search for the message ID in the index file
    int msg_id;
    long offset;
    while (fscanf(index_file, "%d %ld\n", &msg_id, &offset) != EOF) {
        if (msg_id == id) {
            break;
        }
    }
    
    // Close the index file
    fclose(index_file);
    
    // If the message ID is not found in the index file, return NULL
    if (msg_id != id) {
        printf("Error: Message %d not found\n", id);
        return NULL;
    }
    
    // Generate the filename based on the message ID
    char filename[256];
    sprintf(filename, "msg_%d.bin", id);
    
    // Open the file in binary read mode
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Error: Message %d not found\n", id);
        return NULL;
    }
    
    // Move the file pointer to the offset obtained from the index file
    fseek(file, offset, SEEK_SET);
    
    // Dynamically allocate memory for the Message struct
    Message* msg = (Message*)malloc(sizeof(Message));
    
    // Read the message fields from the binary file
    fread(&(msg->id), sizeof(int), 1, file);
    
    // Dynamically allocate memory for the string fields and read them from the file
    msg->time_sent = (char*)malloc(100 * sizeof(char));
    fread(msg->time_sent, sizeof(char), 100, file);
    
    msg->sender = (char*)malloc(100 * sizeof(char));
    fread(msg->sender, sizeof(char), 100, file);
    
    msg->receiver = (char*)malloc(100 * sizeof(char));
    fread(msg->receiver, sizeof(char), 100, file);
    
    msg->content = (char*)malloc(1000 * sizeof(char));
    fread(msg->content, sizeof(char), 1000, file);
    
    fread(&(msg->delivered), sizeof(bool), 1, file);
    
    // Close the file
    fclose(file);
    
    // Return the pointer to the retrieved Message object
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