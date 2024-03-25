// message.h
#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdbool.h>

#define MESSAGE_SIZE 1024
#define CACHE_SIZE 10

typedef struct {
    int id;
    char time_sent[20];
    char sender[50];
    char receiver[50];
    char content[MESSAGE_SIZE - sizeof(int) - 20 - 50 - 50 - sizeof(bool)];
    bool delivered;
} Message;

typedef struct {
    Message messages[CACHE_SIZE];
    int front;
    int rear;
    int count;
} Cache;

Message* create_msg(int id, const char* time_sent, const char* sender, const char* receiver, const char* content, bool delivered);
void store_msg(Message* msg, int algorithm);
Message* retrieve_msg(int id);
void free_msg(Message* msg);
void init_cache();
bool cache_full();
void cache_insert(Message* msg, int algorithm);
Message* cache_retrieve(int id);
void random_replacement(Message* msg);
void lru_replacement(Message* msg);

#endif