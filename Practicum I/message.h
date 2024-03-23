#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdbool.h>

// Set up the message struct.
// ID: the unique identifier of the message.
// time_sent: the time the message was sent.
// sender: the sender of the message.
// receiver: the receiver of the message.
// content: the content of the message.
// delivered: a boolean value indicating whether the message has been delivered.
typedef struct {
    int id;
    char* time_sent;
    char* sender;
    char* receiver;
    char* content;
    bool delivered;
} Message;


Message* create_msg(int id, const char* time_sent, const char* sender, const char* receiver, const char* content);

void store_msg(Message* msg);

Message* retrieve_msg(int id);

void free_msg(Message* msg);

#endif