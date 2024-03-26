#include "message.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Creates a new message.
 * @param unique_id The unique identifier of the message.
 * @param time_sent The timestamp when the message was sent.
 * @param sender The sender of the message.
 * @param receiver The receiver of the message.
 * @param content The content of the message.
 * @param delivered_flag Flag indicating if the message has been delivered.
 * @return A pointer to the newly created message, or NULL if memory allocation fails.
 */
Message* create_msg(int unique_id, time_t time_sent, const char* sender, const char* receiver, const char* content, int delivered_flag) {
    Message* msg = (Message*)malloc(sizeof(Message));
    if (!msg) return NULL;

    msg->unique_id = unique_id;
    msg->time_sent = time_sent;
    strncpy(msg->sender, sender, sizeof(msg->sender) - 1);
    strncpy(msg->receiver, receiver, sizeof(msg->receiver) - 1);
    strncpy(msg->content, content, sizeof(msg->content) - 1);
    msg->delivered_flag = delivered_flag;

    return msg;
}

/**
 * @brief Frees the memory allocated for a message.
 * @param msg A pointer to the message to be freed.
 */
void free_message(Message* msg) {
    if (msg) {
        free(msg);
    }
}
