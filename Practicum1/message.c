
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "message.h"

void init_cache(MessageCache* cache) {
    cache->front = -1;
    cache->rear = -1;
    cache->count = 0;
}

bool is_cache_full(MessageCache* cache) {
    return cache->count == CACHE_SIZE;
}

bool is_cache_empty(MessageCache* cache) {
    return cache->count == 0;
}

void enqueue_message(MessageCache* cache, const Message* msg) {
    if (is_cache_full(cache)) {
        // Replace an existing message using a replacement algorithm
        replace_message_lru(cache, msg);
    } else {
        cache->rear = (cache->rear + 1) % CACHE_SIZE;
        cache->messages[cache->rear] = *msg;
        cache->count++;

        if (cache->front == -1) {
            cache->front = cache->rear;
        }
    }
}

void dequeue_message(MessageCache* cache, Message* msg) {
    if (is_cache_empty(cache)) {
        printf("Cache is empty. Cannot dequeue message.\n");
        return;
    }

    *msg = cache->messages[cache->front];
    cache->front = (cache->front + 1) % CACHE_SIZE;
    cache->count--;

    if (cache->count == 0) {
        cache->front = -1;
        cache->rear = -1;
    }
}

bool find_message_in_cache(MessageCache* cache, const char* id, Message* msg) {
    for (int i = 0; i < cache->count; i++) {
        int index = (cache->front + i) % CACHE_SIZE;
        if (strcmp(cache->messages[index].id, id) == 0) {
            *msg = cache->messages[index];
            return true;
        }
    }
    return false;
}

void replace_message_random(MessageCache* cache, const Message* msg) {
    srand(time(NULL));
    int index = rand() % cache->count;
    cache->messages[(cache->front + index) % CACHE_SIZE] = *msg;
}

void replace_message_lru(MessageCache* cache, const Message* msg) {
    dequeue_message(cache, &(cache->messages[cache->front]));
    enqueue_message(cache, msg);
}

void create_msg(Message* msg, const char* id, const char* time_sent, const char* sender,
                const char* receiver, const char* content, bool delivered) {
    strncpy(msg->id, id, sizeof(msg->id));
    strncpy(msg->time_sent, time_sent, sizeof(msg->time_sent));
    strncpy(msg->sender, sender, sizeof(msg->sender));
    strncpy(msg->receiver, receiver, sizeof(msg->receiver));
    strncpy(msg->content, content, sizeof(msg->content));
    msg->delivered = delivered;
}

void store_msg(const Message* msg, MessageCache* cache) {
    FILE* fp = fopen("message_store.txt", "a");
    if (fp == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }

    fprintf(fp, "%s,%s,%s,%s,%s,%d\n", msg->id, msg->time_sent, msg->sender,
            msg->receiver, msg->content, msg->delivered);
    fclose(fp);

    enqueue_message(cache, msg);
}

bool retrieve_msg(const char* id, Message* msg, MessageCache* cache) {
    if (find_message_in_cache(cache, id, msg)) {
        return true;
    }

    FILE* fp = fopen("message_store.txt", "r");
    if (fp == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }

    char line[MSG_SIZE];
    while (fgets(line, sizeof(line), fp)) {
        char* token = strtok(line, ",");
        if (strcmp(token, id) == 0) {
            strncpy(msg->id, token, sizeof(msg->id));
            strncpy(msg->time_sent, strtok(NULL, ","), sizeof(msg->time_sent));
            strncpy(msg->sender, strtok(NULL, ","), sizeof(msg->sender));
            strncpy(msg->receiver, strtok(NULL, ","), sizeof(msg->receiver));
            strncpy(msg->content, strtok(NULL, ","), sizeof(msg->content));
            msg->delivered = atoi(strtok(NULL, ","));
            fclose(fp);

            enqueue_message(cache, msg);
            return true;
        }
    }

    fclose(fp);
    return false;
}