// Set up the message struct.
// ID: the unique identifier of the message.
// time_sent: the time the message was sent.
// sender: the sender of the message.
// receiver: the receiver of the message.
// content: the content of the message.
// delivered: a boolean value indicating whether the message has been delivered.
#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdbool.h>

#define MAX_MESSAGE_SIZE 1024
#define CACHE_SIZE 16

// Message struct to store message information
typedef struct {
    int id; // 4 bytes
    char time_sent[20]; // 20 bytes
    char sender[50]; // 50 bytes
    char receiver[50]; // 50 bytes
    char content[MAX_MESSAGE_SIZE - 128]; 
    bool delivered; // 1 byte
} Message;

// Cache struct to store messages in memory
typedef struct {
    Message messages[CACHE_SIZE]; // Array of messages
    int front; // Index of the front of the cache (oldest)
    int rear; // Index of the rear of the cache (newest)
    int count; // Number of messages in the cache
} Cache;

Message* create_msg(int id, const char* time_sent, const char* sender, const char* receiver, const char* content, bool delivered);
void store_msg(Message* msg);
Message* retrieve_msg(int id);
void free_msg(Message* msg);

#endif