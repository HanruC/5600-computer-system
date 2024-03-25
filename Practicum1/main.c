#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "message.h"

void test_cache_performance(MessageCache* cache, const Message* messages) {
    int cache_hits = 0;
    int cache_misses = 0;

    // Test the cache performance for 1000 random message accesses
    for (int i = 0; i < 1000; i++) {
        int random_index = rand() % 1000;
        Message msg;
        
        if (retrieve_msg(messages[random_index].id, &msg, cache)) {
            cache_hits++;
        } else {
            cache_misses++;
        }
    }

    // Calculate cache hit ratio
    float cache_hit_ratio = (float)cache_hits / 1000 * 100;

    // Display cache metrics for the algorithm
    printf("Number of Cache Hits: %d\n", cache_hits);
    printf("Number of Cache Misses: %d\n", cache_misses);
    printf("Cache Hit Ratio: %.2f%%\n", cache_hit_ratio);
    printf("\n");
}

int main() {
    srand(time(NULL));

    // Initialize the message cache
    MessageCache cache;
    init_cache(&cache);

    // Generate a representative message set for testing
    Message messages[1000];
    for (int i = 0; i < 1000; i++) {
        char id[20];
        sprintf(id, "MSG-%d", i);
        create_msg(&messages[i], id, "12:00", "Sender", "Receiver", "Sample content", false);
    }

    // Test cache performance for Random Replacement Algorithm
    printf("Testing Random Replacement Algorithm:\n");
    test_cache_performance(&cache, messages);

    // Test cache performance for LRU Replacement Algorithm
    printf("Testing LRU Replacement Algorithm:\n");
    test_cache_performance(&cache, messages);

    return 0;
}