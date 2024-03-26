#include "queue.h"

void add2q(queue_t *queue, void *element) {
    Node* newNode = (Node*)malloc(sizeof(Node)); 
    newNode->data = element;
    newNode->next = NULL;
    newNode->prev = queue->tail; // add to the end of the queue 

    if (queue->tail != NULL) {
        queue->tail->next = newNode;
    } else {
        queue->head = newNode; // if the queue is empty, the new node is the head
    }

    queue->tail = newNode; // the new node is the tail
    queue->size++;
}

void* popQ(queue_t *queue) {
    if (queue->head == NULL) {
        return NULL;
    }
    Node* head_node = queue->head; // set the head node to the head of the queue
    void* data = head_node->data;
    queue->head = head_node->next; // set the head to the next node of the head node 
    if (queue->head != NULL) {
        queue->head->prev = NULL; // if the head is not NULL, set the next node's previous to NULL
    } else {
        queue->tail = NULL; // if the head is NULL, set the tail to NULL 
    }

    free(head_node);
    queue->size--;
    return data;
}

process_t* rmProcess(queue_t* queue) {
    if (queue->head == NULL) {
        return NULL;
    }
    Node* temp = queue->head; //traverse from the head
    Node *removed = NULL; // set the removed node to NULL
    process_t* highestPP = NULL; // set the highest priority process to NULL (node data)
    int highestPriority = -1;  // set the highest priority to -1
    while (temp != NULL) {
        process_t* curProcess = (process_t*)temp->data; 
        // set the current process to the data of the current node 
        if (curProcess->priority > highestPriority) {
            highestPriority = curProcess->priority; // set the highest priority to the current process priority
            highestPP = curProcess; // set the highest priority process to the current process
            removed = temp; // set the removed node to the current node
        } 
        temp = temp->next; // move to the next node 
    }

    // remove the node with the highest priority
    if (removed) {
        if (removed->prev) {
            removed->prev->next = removed->next;
            // set the previous node's next to the removed node's next node
        } else {
            queue->head = removed->next; 
            // if the previous node is NULL, set the head to the removed node's next node
        }
        if (removed->next) {
            removed->next->prev = removed->prev; 
            // set the removed node's next node's previous to the removed node's previous
        } else {
            queue->tail = removed->prev;
            // if the next node is NULL, set the tail to the removed node's previous 
        }
        free(removed);
        queue->size--;
    }
    return highestPP;
}

int qsize(queue_t *queue) {
    return queue->size;
}