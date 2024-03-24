#include "message.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Cache cache = {0}; // Initialize the cache with zeros

// Function to create a new message
Message* create_msg(int id, const char* time_sent, const char* sender, const char* receiver, const char* content, bool delivered) {
    Message* msg = (Message*)malloc(sizeof(Message));  // Allocate memory for the message
    msg->id = id;                                 
    strncpy(msg->time_sent, time_sent, sizeof(msg->time_sent));  // Copy the timestamp
    strncpy(msg->sender, sender, sizeof(msg->sender));           // Copy the sender
    strncpy(msg->receiver, receiver, sizeof(msg->receiver));     // Copy the receiver
    strncpy(msg->content, content, sizeof(msg->content));        // Copy the content
    msg->delivered = delivered;                            // Set the delivered flag to false
    return msg;                                        // Return the created message
}

// Function to store a message in the cache and on disk
void store_msg(Message* msg) {
    // Check if the message is already in the cache
    for (int i = 0; i < cache.count; i++) {
        int index = (cache.front + i) % CACHE_SIZE;
        if (cache.messages[index].id == msg->id) {
            // Move the message to the rear of the cache (most recently used)
            for (int j = index; j != cache.rear; j = (j + 1) % CACHE_SIZE) {
                cache.messages[j] = cache.messages[(j + 1) % CACHE_SIZE];
            }
            cache.messages[cache.rear] = *msg;
            cache.rear = (cache.rear + 1) % CACHE_SIZE;
            return;
        }
    }

    // If the cache is full, remove the least recently used message
    if (cache.count == CACHE_SIZE) {
        cache.front = (cache.front + 1) % CACHE_SIZE;
        cache.count--;
    }

    // Store the message in the cache
    cache.messages[cache.rear] = *msg;
    cache.rear = (cache.rear + 1) % CACHE_SIZE;
    cache.count++;

    // Store the message on disk
    char filename[256];
    sprintf(filename, "msg_%d.txt", msg->id);
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Unable to store message %d\n", msg->id);
        return;
    }
    fprintf(file, "id: %d\n", msg->id);
    fprintf(file, "time sent: %s\n", msg->time_sent);
    fprintf(file, "sender: %s\n", msg->sender);
    fprintf(file, "receiver: %s\n", msg->receiver);
    fprintf(file, "content: %s\n", msg->content);
    fprintf(file, "delivered or not: %d\n", msg->delivered);
    fclose(file);
}

// Function to retrieve a message from the cache or disk
Message* retrieve_msg(int id) {
    // Check if the message is in the cache
    for (int i = 0; i < cache.count; i++) {
        int index = (cache.front + i) % CACHE_SIZE;
        if (cache.messages[index].id == id) {
            // Move the message to the rear of the cache (most recently used)
            Message msg = cache.messages[index];
            for (int j = index; j != cache.rear; j = (j + 1) % CACHE_SIZE) {
                cache.messages[j] = cache.messages[(j + 1) % CACHE_SIZE];
            }
            cache.messages[cache.rear] = msg;
            cache.rear = (cache.rear + 1) % CACHE_SIZE;
            if (cache.count < CACHE_SIZE) {
                cache.count++;
            }
            cache.front = (cache.front + 1) % CACHE_SIZE;

            // Create a new dynamically allocated message and copy the data
            Message* result = (Message*)malloc(sizeof(Message));
            *result = msg;
            return result;
        }
    }

    // If the message is not in the cache, retrieve it from disk
    char filename[256];
    sprintf(filename, "msg_%d.txt", id);               // Generate the filename based on the message ID
    FILE* file = fopen(filename, "r");                 // Open the file for reading
    if (file == NULL) {                                // If the file cannot be opened
        printf("Error: Message %d not found\n", id);
        return NULL;
    }
    Message* msg = (Message*)malloc(sizeof(Message));  // Allocate memory for the message
    fread(msg, sizeof(Message), 1, file);              // Read the message from the file
    fclose(file);                                      // Close the file

    // Add the message to the cache
    store_msg(msg);                                    // Store the retrieved message in the cache

    return msg;                                        // Return the retrieved message
}

// Function to free the memory allocated for a message
void free_msg(Message* msg) {
    free(msg);
}