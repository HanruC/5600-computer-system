
#include "message.h"
#include <stdio.h>

int store_msg(Message* msg) {
    FILE* file = fopen("messages.txt", "a");
    if (!file) return -1;

    fprintf(file, "ID: %d\n", msg->unique_id);
    fprintf(file, "Time Sent: %s", ctime(&msg->time_sent));
    fprintf(file, "Sender: %s\n", msg->sender);
    fprintf(file, "Receiver: %s\n", msg->receiver);
    fprintf(file, "Content: %s\n", msg->content);
    fprintf(file, "Delivered: %s\n", msg->delivered_flag ? "Yes" : "No");
    fprintf(file, "------------------------\n");

    fclose(file);
    return 0;
}