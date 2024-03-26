#include "message.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TOTAL_ACCESSES 1000
#define TOTAL_MESSAGES 1000

void init_cache(MessageCache* cache, int policy);
void simulate_accesses(MessageCache* cache, int num_accesses, int replacementPolicy);

/**
 * @brief Main function for running the message caching simulations.
 * @return 0 on successful execution.
 */
int main() {
    // Seed the random number generator
    srand((unsigned int)time(NULL));

    printf("Simulation with LRU Policy:\n");
    simulate_accesses(&(MessageCache){0}, TOTAL_ACCESSES, REPLACEMENT_POLICY_LRU);

    printf("\nSimulation with Random Replacement Policy:\n");
    simulate_accesses(&(MessageCache){0}, TOTAL_ACCESSES, REPLACEMENT_POLICY_RANDOM);

    return 0;
}

/**
 * @brief Simulates message retrieval and evaluates cache performance.
 * @param cache A pointer to the message cache.
 * @param num_accesses The number of message accesses to simulate.
 * @param replacementPolicy The replacement policy used by the cache.
 */
void simulate_accesses(MessageCache* cache, int num_accesses, int replacementPolicy) {
    // Initialize the cache with the given policy
    init_cache(cache, replacementPolicy);

    // Create a set of messages and add them to the cache
    for (int i = 0; i < TOTAL_MESSAGES; ++i) {
        Message* new_message = create_msg(i, time(NULL), "Luke", "Jason", "Hi, Jason!", 0);
        if (new_message != NULL) {
            store_message_in_cache(cache, new_message);
            store_msg(new_message);
            free_message(new_message);
        }
    }

    // Simulate message retrieval and evaluate cache performance
    int hits = 0, misses = 0;
    for (int i = 0; i < num_accesses; ++i) {
        int message_id = rand() % TOTAL_MESSAGES;
        Message* message = retrieve_message_from_cache(cache, message_id);
        if (message != NULL) {
            ++hits;
        } else {
            ++misses;
        }
    }

    // Output cache performance statistics
    printf("Policy: %s\n", replacementPolicy == REPLACEMENT_POLICY_LRU ? "LRU" : "Random");
    printf("Cache Hits: %d\n", hits);
    printf("Cache Misses: %d\n", misses);
    double hit_ratio = (double)hits / (hits + misses);
    printf("Cache Hit Ratio: %.2f%%\n", hit_ratio * 100);
    // Consider freeing allocated resources here or resetting the cache for accuracy in repeated simulations
}
