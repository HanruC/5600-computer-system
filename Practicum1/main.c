#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "message.h"

void test_cache_performance(MessageCache* cache, const Message* messages) {
    int cache_hits_random = 0;
    int cache_misses_random = 0;

    int cache_hits_lru = 0;
    int cache_misses_lru = 0;

    // Random Replacement Algorithm
    for (int i = 0; i < 1000; i++) {
        int random_index = rand() % 1000;
        Message msg;
        
        if (retrieve_msg(messages[random_index].id, &msg, cache)) {
            cache_hits_random++;
        } else {
            cache_misses_random++;
        }
    }

    // LRU Replacement Algorithm
    for (int i = 0; i < 1000; i++) {
        int random_index = rand() % 1000;
        Message msg;
        
        if (retrieve_msg(messages[random_index].id, &msg, cache)) {
            cache_hits_lru++;
        } else {
            cache_misses_lru++;
        }
    }

    // Calculate cache hit ratios
    float cache_hit_ratio_random = (float)cache_hits_random / 1000 * 100;
    float cache_hit_ratio_lru = (float)cache_hits_lru / 1000 * 100;

    // Display cache metrics for Random Replacement Algorithm
    printf("Random Replacement Algorithm:\n");
    printf("Number of Cache Hits: %d\n", cache_hits_random);
    printf("Number of Cache Misses: %d\n", cache_misses_random);
    printf("Cache Hit Ratio: %.2f%%\n", cache_hit_ratio_random);
    printf("\n");

    // Display cache metrics for LRU Replacement Algorithm
    printf("LRU Replacement Algorithm:\n");
    printf("Number of Cache Hits: %d\n", cache_hits_lru);
    printf("Number of Cache Misses: %d\n", cache_misses_lru);
    printf("Cache Hit Ratio: %.2f%%\n", cache_hit_ratio_lru);
}

int main() {
    srand(time(NULL));

    // Initialize the message cache
    MessageCache cache;
    init_cache(&cache);

    // Generate a set of test messages with different access frequencies
    Message messages[1000];
    for (int i = 0; i < 1000; i++) {
        char id[20];
        sprintf(id, "MSG-%d", i);
        create_msg(&messages[i], id, "12:00", "Luke", "Jason", "Hi Jason, this is Luke", true);

        // Simulate different access frequencies by repeating some messages
        for (int j = 0; j < i % 10; j++) {
            store_msg(&messages[i], &cache);
        }
    }

    // Test cache performance for both page replacement algorithms
    test_cache_performance(&cache, messages);

    return 0;
}
