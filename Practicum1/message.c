#include "message.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

Cache cache;

Message* create_msg(int id, const char* time_sent, const char* sender, const char* receiver, const char* content, bool delivered) {
    Message* msg = (Message*)malloc(sizeof(Message));
    msg->id = id;
    strncpy(msg->time_sent, time_sent, sizeof(msg->time_sent));
    strncpy(msg->sender, sender, sizeof(msg->sender));
    strncpy(msg->receiver, receiver, sizeof(msg->receiver));
    strncpy(msg->content, content, sizeof(msg->content));
    msg->delivered = delivered;
    return msg;
}

void store_msg(Message* msg, int algorithm) {
    char filename[256];
    sprintf(filename, "msg_%d.txt", msg->id);
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Unable to store message %d\n", msg->id);
        return;
    }
    fwrite(msg, sizeof(Message), 1, file);
    fclose(file);

    if (algorithm != -1) {
        cache_insert(msg, algorithm);
    }
}

Message* retrieve_msg(int id) {
    Message* msg = cache_retrieve(id);
    if (msg != NULL) {
        return msg;
    }

    char filename[256];
    sprintf(filename, "msg_%d.txt", id);
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Message %d not found\n", id);
        return NULL;
    }
    msg = (Message*)malloc(sizeof(Message));
    fread(msg, sizeof(Message), 1, file);
    fclose(file);

    cache_insert(msg, 1); // Use LRU replacement algorithm when inserting into cache

    return msg;
}

void free_msg(Message* msg) {
    free(msg);
}

void init_cache() {
    cache.front = 0;
    cache.rear = -1;
    cache.count = 0;
}

bool cache_full() {
    return cache.count == CACHE_SIZE;
}

void cache_insert(Message* msg, int algorithm) {
    if (cache_full()) {
        if (algorithm == 0) {
            random_replacement(msg);
        } else if (algorithm == 1) {
            lru_replacement(msg);
        }
    } else {
        cache.rear = (cache.rear + 1) % CACHE_SIZE;
        cache.messages[cache.rear] = *msg;
        cache.count++;
    }
}

Message* cache_retrieve(int id) {
    for (int i = 0; i < cache.count; i++) {
        int index = (cache.front + i) % CACHE_SIZE;
        if (cache.messages[index].id == id) {
            // Move the accessed message to the rear (most recently used)
            Message temp = cache.messages[index];
            for (int j = index; j != cache.rear; j = (j + 1) % CACHE_SIZE) {
                cache.messages[j] = cache.messages[(j + 1) % CACHE_SIZE];
            }
            cache.messages[cache.rear] = temp;

            // Create a dynamically allocated copy of the message
            Message* msg_copy = (Message*)malloc(sizeof(Message));
            *msg_copy = cache.messages[cache.rear];
            return msg_copy;
        }
    }
    return NULL;
}

void random_replacement(Message* msg) {
    int index = rand() % CACHE_SIZE;
    cache.messages[index] = *msg;
}

void lru_replacement(Message* msg) {
    cache.messages[cache.front] = *msg;
    cache.front = (cache.front + 1) % CACHE_SIZE;
    cache.rear = (cache.front + cache.count - 1) % CACHE_SIZE;
}