#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Process struct
typedef struct {
    int identifier;
    char* name;
    long cumulative_runtime;
    int priority;
} process_t; 

typedef struct Node {
    void* data;
    struct Node* next;
    struct Node* prev;
} Node;

typedef struct {
    Node* head;
    Node* tail;
    int size;
} queue_t;

void add2q(queue_t* queue, void* element);
void* popQ(queue_t* queue);
process_t* rmProcess(queue_t* queue);
int qsize(queue_t* queue);

#endif