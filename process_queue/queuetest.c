#include "queue.h"

// Helper function to create a process
process_t* create_process(int identifier, const char* name, long runtime, int priority) {
    process_t* new_process = (process_t*)malloc(sizeof(process_t));
    if (!new_process) {
        perror("Failed to allocate memory for process");
        exit(EXIT_FAILURE);
    }
    new_process->identifier = identifier;
    new_process->name = strdup(name); // Ensure you free this memory later!
    new_process->cumulative_runtime = runtime;
    new_process->priority = priority;
    return new_process;
}

// Helper function to print a process
void print_process(const process_t* process) {
    printf("Process ID: %d, Name: %s, Runtime: %ld, Priority: %d\n", process->identifier, process->name, process->cumulative_runtime, process->priority);
}

// Helper function to print the current state of the queue
void print_queue_state(queue_t* queue) {
    Node* current = queue->head;
    while (current != NULL) {
        process_t* process = (process_t*)current->data;
        print_process(process);
        current = current->next;
    }
    printf("\n");
}

int main() {
    // Initialize the queue
    queue_t myQueue = {0};

    // Create and add processes to the queue
    printf("Adding processes to the queue...\n");
    add2q(&myQueue, create_process(1, "Process 1", 100, 3));
    add2q(&myQueue, create_process(2, "Process 2", 200, 1));
    add2q(&myQueue, create_process(3, "Process 3", 300, 2));
    printf("Initial queue state:\n");
    print_queue_state(&myQueue);

    // Remove the front process
    printf("Removing the front process...\n");
    process_t* removedProcess = popQ(&myQueue);
    printf("Removed process:\n");
    print_process(removedProcess);
    free(removedProcess->name); // Don't forget to free the name!
    free(removedProcess); // And the process itself
    printf("Queue state after popping the front process:\n");
    print_queue_state(&myQueue);

    // Remove the highest priority process
    printf("Removing the highest priority process...\n");
    removedProcess = rmProcess(&myQueue);
    printf("Removed highest priority process:\n");
    print_process(removedProcess);
    free(removedProcess->name);
    free(removedProcess);
    printf("Queue state after removing the highest priority process:\n");
    print_queue_state(&myQueue);

    // Check the size of the queue
    printf("Current queue size: %d\n", qsize(&myQueue));

    while (myQueue.size > 0) {
        removedProcess = popQ(&myQueue);
        free(removedProcess->name);
        free(removedProcess);
    }

    return 0;
}
