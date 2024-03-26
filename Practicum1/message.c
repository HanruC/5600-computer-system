#include "message.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void free_message(Message* msg) {
    if (msg) {
        free(msg);
    }
}
