#include "message.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_MESSAGES 1000
#define NUM_ACCESSES 10000

int cache_hits = 0;
int cache_misses = 0;

void generate_messages();
void test_algorithm(int algorithm);

int main() {
    srand(time(NULL)); // Initialize the random seed
    init_cache();
    generate_messages();

    printf("Testing Random Replacement Algorithm:\n");
    cache_hits = 0;
    cache_misses = 0;
    test_algorithm(0);

    init_cache();

    printf("Testing LRU Replacement Algorithm:\n");
    cache_hits = 0;
    cache_misses = 0;
    test_algorithm(1);

    return 0;
}

void generate_messages() {
    char content[MESSAGE_SIZE - sizeof(int) - 20 - 50 - 50 - sizeof(bool)] = "Test message content";
    for (int i = 0; i < NUM_MESSAGES; i++) {
        char time_sent[20];
        sprintf(time_sent, "%d", i);
        Message* msg = create_msg(i, time_sent, "sender", "receiver", content, false);
        store_msg(msg, -1); // -1 indicates no replacement algorithm needed
        free_msg(msg);
    }
}

void test_algorithm(int algorithm) {
    for (int i = 0; i < NUM_ACCESSES; i++) {
        int msg_id;
        if (i % 100 < 90) {
            // 90% of the accesses are to the first 10% of the messages
            msg_id = rand() % (NUM_MESSAGES / 10);
        } else {
            // 10% of the accesses are to the remaining messages
            msg_id = rand() % NUM_MESSAGES;
        }
        Message* msg = retrieve_msg(msg_id);
        if (msg != NULL) {
            cache_hits++;
            free_msg(msg);
        } else {
            cache_misses++;
        }
    }
    printf("Cache Hits: %d\n", cache_hits);
    printf("Cache Misses: %d\n", cache_misses);
    printf("Cache Hit Ratio: %.2f%%\n", (float)cache_hits / (cache_hits + cache_misses) * 100);
}