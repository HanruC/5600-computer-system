#include <stdlib.h> 
#include "message.h"
#include <string.h>

#include <time.h>

#define REPLACEMENT_POLICY_RANDOM 1
#define REPLACEMENT_POLICY_LRU 2

// Choose the replacement policy here
#define REPLACEMENT_POLICY REPLACEMENT_POLICY_LRU

/**
 * @brief Initializes the message cache with the specified replacement policy.
 * @param cache A pointer to the message cache to be initialized.
 * @param policy The replacement policy to be used by the cache.
 */
void init_cache(MessageCache* cache, int policy) {
    for (int i = 0; i < CACHE_SIZE; i++) {
        cache->entries[i].valid = 0;
    }
    cache->replacement_policy = policy;
    cache->time_counter = 0; // Initialize time counter for LRU
}

/**
 * @brief Finds the index of the least recently used (LRU) entry in the cache.
 * @param cache A pointer to the message cache.
 * @return The index of the LRU entry.
 */
int find_replacement_index_LRU(MessageCache* cache) {
    int lru_index = 0;
    for (int i = 1; i < CACHE_SIZE; i++) {
        if (cache->entries[i].access_time < cache->entries[lru_index].access_time) {
            lru_index = i;
        }
    }
    return lru_index;
}

/**
 * @brief Finds a random index for replacing an entry in the cache.
 * @return The randomly selected index.
 */
int find_replacement_index_random() {
    return rand() % CACHE_SIZE;
}

/**
 * @brief Stores a message in the cache.
 * @param cache A pointer to the message cache.
 * @param msg A pointer to the message to be stored.
 * @return 0 on success, -1 on failure.
 */
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

/**
 * @brief Retrieves a message from the cache based on its unique identifier.
 * @param cache A pointer to the message cache.
 * @param message_id The unique identifier of the message to be retrieved.
 * @return A pointer to the retrieved message, or NULL if the message is not found.
 */
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
