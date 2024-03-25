
#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdbool.h>

#define MSG_SIZE 1024
#define CACHE_SIZE 16

typedef struct {
    char id[20];
    char time_sent[20];
    char sender[50];
    char receiver[50];
    char content[MSG_SIZE - 140];
    bool delivered;
} Message;

typedef struct {
    Message messages[CACHE_SIZE];
    int front;
    int rear;
    int count;
} MessageCache;

void init_cache(MessageCache* cache);
bool is_cache_full(MessageCache* cache);
bool is_cache_empty(MessageCache* cache);
void enqueue_message(MessageCache* cache, const Message* msg);
void dequeue_message(MessageCache* cache, Message* msg);
bool find_message_in_cache(MessageCache* cache, const char* id, Message* msg);
void replace_message_random(MessageCache* cache, const Message* msg);
void replace_message_lru(MessageCache* cache, const Message* msg);

void create_msg(Message* msg, const char* id, const char* time_sent, const char* sender,
                const char* receiver, const char* content, bool delivered);
void store_msg(const Message* msg, MessageCache* cache);
bool retrieve_msg(const char* id, Message* msg, MessageCache* cache);

#endif