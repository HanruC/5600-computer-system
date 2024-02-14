#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>

#define ARRAY_SIZE 5
int shared_array[ARRAY_SIZE] = {0, 0, 0, 0, 0};

void *engineer(void *data){
    for (int i = 0; i < ARRAY_SIZE; i++) {
        shared_array[i] = i * 2; // write data to shared array
        printf("Engineer done reading %d\n", shared_array[i]);
        usleep(50000); // wait for 50ms
    }
    return NULL;
}

void *manager(void *data){
    usleep(150000); // wait longer for the first data to be written
    for (int i = 0; i < ARRAY_SIZE; i++) {
        printf("Manager done reading %d\n", shared_array[i]);
    }
    return NULL;
}

int main(void) {
    pthread_t th1, th2;
    // Create two threads 
    pthread_create(&th1, NULL, engineer, NULL);
    pthread_create(&th2, NULL, manager, NULL);

    sleep(1); 

    // Cancel thread 2 -- manager
    printf("====> Cancelling Manager with ID %lu!!\n", (unsigned long)th2);
    pthread_cancel(th2);

    usleep(100000); 

    // Cancel thread 1 -- engineer
    printf("====> Cancelling Engineer with ID %lu!!\n", (unsigned long)th1);
    pthread_cancel(th1);

    // Wait for threads to finish after cancellation
    pthread_join(th1, NULL);
    pthread_join(th2, NULL);

    printf("Exiting from main program\n");
    return 0;
}

/*
Based on the output, I found inconsistent results. 
Engineer and Manager threads come to mixed order.
Outputs from the engineer and manager threads are interleaved and the manager
reads zeros for values that the engineer hasn't written yet. 
This indicates that the inconsistency in thread execution is due to the lack of synchronization mechanism.
This inconsistency is a fundamental problem in concurrent programming without control over the execution order of threads.

Reasons for the inconsistency: Without mechanisms to control access to shared resources (shared array in this case),
which means that the order in which threads will read from and write to these resources. 
The manager thread reads from the shared array potentially before the engineer thread has finished writing to it, 
leading to inconsistent reads.

Without synchronization, the inconsistency is expected. 
To address these issues, implementing synchronization like mutexes（mutual exclusion locks）
could control access to shared resources, 
preventing simultaneous read/write operations that lead to inconsistency. Mutexes can be something like semaphores. 
*/