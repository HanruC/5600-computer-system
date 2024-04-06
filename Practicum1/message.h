/**
 * @file message.h
 * @brief Header file for message and cache management.
 */
#ifndef MESSAGE_H
#define MESSAGE_H

#include <time.h>

/**
 * @def MESSAGE_SIZE
 * @brief The size of each message in bytes.
 */
#define MESSAGE_SIZE 1024
#define CACHE_SIZE 64 // 64, 16, 32 Cache size tested with different values

#ifndef CACHE_POLICY_H
#define CACHE_POLICY_H

/**
 * @def REPLACEMENT_POLICY_RANDOM
 * @brief Constant representing the random replacement policy.
 */
#define REPLACEMENT_POLICY_RANDOM 1
/**
 * @def REPLACEMENT_POLICY_LRU
 * @brief Constant representing the least recently used (LRU) replacement policy.
 */
#define REPLACEMENT_POLICY_LRU 2

#endif 

/**
 * @struct CacheEntry
 * @brief Represents an entry in the cache.
 */
typedef struct {
    int valid;
    char data[MESSAGE_SIZE];
    int message_id;
    int access_time; // For LRU: Lower means older
} CacheEntry;

/**
 * @struct MessageCache
 * @brief Represents the cache for storing messages.
 */
typedef struct {
    CacheEntry entries[CACHE_SIZE];
    int replacement_policy;
    int time_counter; // Incremented with each cache access
} MessageCache;

/**
 * @struct Message
 * @brief Represents a message.
 */
typedef struct {
    int unique_id;
    time_t time_sent;
    char sender[256]; // Assuming a fixed size for simplicity
    char receiver[256];
    char content[512]; // Adjust sizes as needed
    int delivered_flag;
} Message;


Message* create_msg(int unique_id, time_t time_sent, const char* sender, const char* receiver, const char* content, int delivered_flag);
void free_message(Message* msg);
int store_msg(Message* msg);
void init_cache(MessageCache* cache, int policy);
int store_message_in_cache(MessageCache* cache, Message* msg);
Message* retrieve_message_from_cache(MessageCache* cache, int message_id);

#endif // MESSAGE_H
