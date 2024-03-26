#include <stdlib.h>
#include <stdio.h>

// Static segment: 5 MB static array
static char staticMemory[5 * 1024 * 1024];

void allocateOnStack() {
    // 1 MB on the stack
    char stackMemory[1 * 1024 * 1024];
    stackMemory[0] = 'a';
    printf("Addess of stackMemory: %p\n", (void*)stackMemory);
}

int main() {
    // allocate 10 MB on the heap
    char *heapMemory = (char *)malloc(10 * 1024 * 1024);

    // if heapMemory allocation failed
    if (heapMemory == NULL) {
        fprintf(stderr, "Heap allocation failed\n");
        exit(EXIT_FAILURE);
    }

    heapMemory[0] = 'a';
    printf("Address of heapMemory: %p\n", (void*)heapMemory);
    staticMemory[0] = 'a';
    printf("Address of staticMemory: %p\n", (void*)staticMemory);
    // At this point, memory is allocated on the heap, static segment, and stack. 

    allocateOnStack(); // allocate 1 MB on the stack

    free(heapMemory); // free heap memory

    // after free, heap memory has been freed and returned to the OS
    // but static memory is still allocated until the program exits
    // The stack memory is deallocated when the function returns 

    return 0; 

}