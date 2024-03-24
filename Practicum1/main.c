#include "message.h"
#include <stdio.h>
#include <time.h>

int main() {
    // Create and store messages
    for (int i = 1; i <= 20; i++) {
        char time_sent[20];
        sprintf(time_sent, "2023-06-09 %02d:00:00", i);
        char sender[50];
        sprintf(sender, "User%d", i);
        char receiver[50];
        sprintf(receiver, "User%d", i + 1);
        char content[100];
        sprintf(content, "Message from User%d to User%d", i, i + 1);
        Message* msg = create_msg(i, time_sent, sender, receiver, content, false);
        store_msg(msg);
        free_msg(msg);
    }

    // Test retrieving a message that is already in the cache
    clock_t start = clock();
    Message* cached_msg = retrieve_msg(10);
    clock_t end = clock();
    double cached_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Retrieving a cached message took %f seconds.\n", cached_time);
    free_msg(cached_msg);

    // Test retrieving a message that is not in the cache
    start = clock();
    Message* non_cached_msg = retrieve_msg(25);
    end = clock();
    double non_cached_time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Retrieving a non-cached message took %f seconds.\n", non_cached_time);
    free_msg(non_cached_msg);

    // Test cache replacement policy
    for (int i = 21; i <= 30; i++) {
        char time_sent[20];
        sprintf(time_sent, "2023-06-09 %02d:00:00", i);
        char sender[50];
        sprintf(sender, "User%d", i);
        char receiver[50];
        sprintf(receiver, "User%d", i + 1);
        char content[100];
        sprintf(content, "Message from User%d to User%d", i, i + 1);
        Message* msg = create_msg(i, time_sent, sender, receiver, content, false);
        store_msg(msg);
        free_msg(msg);
    }

    // Retrieve messages in a specific order to verify the cache replacement policy
    int retrieval_order[] = {30, 29, 28, 27, 26, 25, 24, 23, 22, 21};
    for (int i = 0; i < 10; i++) {
        Message* msg = retrieve_msg(retrieval_order[i]);
        if (msg != NULL) {
            printf("Retrieved message with ID %d\n", msg->id);
            free_msg(msg);
        }
    }

    return 0;
}