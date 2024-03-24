#include "message.h"
#include <stdio.h>

int main() {
    // Create a few messages
    Message* msg1 = create_msg(1, "2023-06-08 10:00:00", "Alice", "Bob", "Hello, Bob!");
    Message* msg2 = create_msg(2, "2023-06-08 11:30:00", "Bob", "Alice", "Hi, Alice!");
    Message* msg3 = create_msg(3, "2023-06-08 12:15:00", "Charlie", "Justin", "Meeting at 2pm");

    // Store the messages
    store_msg(msg1);
    store_msg(msg2);
    store_msg(msg3);

    // Retrieve a message by ID
    int msg_id = 2;
    Message* retrieved_msg = retrieve_msg(msg_id);
    if (retrieved_msg != NULL) {
        printf("Retrieved message %d:\n", retrieved_msg->id);
        printf("Time sent: %s\n", retrieved_msg->time_sent);
        printf("Sender: %s\n", retrieved_msg->sender);
        printf("Receiver: %s\n", retrieved_msg->receiver);
        printf("Content: %s\n", retrieved_msg->content);
        printf("Delivered: %s\n", retrieved_msg->delivered ? "Yes" : "No");
        free_msg(retrieved_msg);
    }

    // Free the memory allocated for the messages
    free_msg(msg1);
    free_msg(msg2);
    free_msg(msg3);

    return 0;
}