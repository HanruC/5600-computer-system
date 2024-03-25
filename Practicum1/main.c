// main.c
#include "message.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_MESSAGES 1000
#define NUM_ACCESSES 1000

void generate_messages();
void test_algorithm(int algorithm);

int main() {
    init_cache();

    generate_messages();

    printf("Testing Random Replacement Algorithm:\n");
    test_algorithm(0);

    init_cache();

    printf("Testing LRU Replacement Algorithm:\n");
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
    srand(time(NULL));
    int cache_hits = 0;
    int cache_misses = 0;
    for (int i = 0; i < NUM_ACCESSES; i++) {
        int msg_id = rand() % NUM_MESSAGES;
        Message* msg = retrieve_msg(msg_id);
        if (msg != NULL) {
            cache_hits++;
        } else {
            cache_misses++;
        }
    }
    printf("Cache Hits: %d\n", cache_hits);
    printf("Cache Misses: %d\n", cache_misses);
    printf("Cache Hit Ratio: %.2f%%\n", (float)cache_hits / NUM_ACCESSES * 100);
}