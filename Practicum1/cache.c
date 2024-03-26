#include <stdlib.h> // Include this line at the top of cache.c
#include "message.h"
#include <string.h>

#include <time.h>

#define REPLACEMENT_POLICY_RANDOM 1
#define REPLACEMENT_POLICY_LRU 2

// Choose the replacement policy here
#define REPLACEMENT_POLICY REPLACEMENT_POLICY_LRU

void init_cache(MessageCache* cache, int policy) {
    for (int i = 0; i < CACHE_SIZE; i++) {
        cache->entries[i].valid = 0;
    }
    cache->replacement_policy = policy;
    cache->time_counter = 0; // Initialize time counter for LRU
}

int find_replacement_index_LRU(MessageCache* cache) {
    int lru_index = 0;
    for (int i = 1; i < CACHE_SIZE; i++) {
        if (cache->entries[i].access_time < cache->entries[lru_index].access_time) {
            lru_index = i;
        }
    }
    return lru_index;
}

int find_replacement_index_random() {
    return rand() % CACHE_SIZE;
}

int store_message_in_cache(MessageCache* cache, Message* msg) {
    int index = -1;
    for (int i = 0; i < CACHE_SIZE; i++) {
        if (!cache->entries[i].valid) {
            index = i;
            break;
        }
    }

    if (index == -1) { // Cache is full, find a slot to replace
        if (REPLACEMENT_POLICY == REPLACEMENT_POLICY_RANDOM) {
            index = find_replacement_index_random();
        } else if (REPLACEMENT_POLICY == REPLACEMENT_POLICY_LRU) {
            index = find_replacement_index_LRU(cache);
        }
    }

    memcpy(cache->entries[index].data, (char*)msg, sizeof(Message));
    cache->entries[index].valid = 1;
    cache->entries[index].message_id = msg->unique_id;
    cache->entries[index].access_time = cache->time_counter++; // Update access time for LRU

    return 0; // Success
}

Message* retrieve_message_from_cache(MessageCache* cache, int message_id) {
    for (int i = 0; i < CACHE_SIZE; i++) {
        if (cache->entries[i].valid && cache->entries[i].message_id == message_id) {
            Message* msg = (Message*)malloc(sizeof(Message));
            memcpy((char*)msg, cache->entries[i].data, sizeof(Message));
            return msg;
        }
    }
    return NULL; // Not found in cache
}
