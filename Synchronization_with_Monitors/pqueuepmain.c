#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 15

int buffer[BUF_SIZE];
int num = 0;
int totalProduced = 0;
int totalConsumed = 0;
pthread_mutex_t mut;
pthread_cond_t cond;

void *producer(void *param);
void *consumer(void *param);

int main(int argc, char *argv[])
{
  pthread_t t1, t2, t3, t4;

  printf("Initializing mutex and condition variable...\n");
  if (pthread_mutex_init(&mut, NULL) != 0){
    perror("pthread_mutex_init");
    exit(1);
  }
  if (pthread_cond_init(&cond, NULL) != 0){
    perror("pthread_cond_init");
    exit(1);
  }

  // Creating two producers
  printf("Creating two producers and two consumers...\n");
  if (pthread_create(&t1, NULL, producer, NULL) != 0 || 
      pthread_create(&t3, NULL, producer, NULL) != 0){
    perror("pthread_create");
    exit(1);
  }

    // Creating two consumers
  if (pthread_create(&t2, NULL, consumer, NULL) != 0 ||
      pthread_create(&t4, NULL, consumer, NULL) != 0){
      perror("pthread_create");
      exit(1);
  }

  printf("Waiting for threads to finish...\n");
  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  pthread_join(t3, NULL);
  pthread_join(t4, NULL);

  printf("All threads completed. We have done. Program exiting.\n");
}

// Producer thread
void *producer(void *param) {
  printf("producer %ld: Starting...\n", (long)pthread_self());
  int i = 0, wait_status, broadcast_status;
  // wait_status and broadcast_status are used to store the return value
  while (1) {
    pthread_mutex_lock(&mut);
    printf("producer %ld: Lock acquired...\n", (long)pthread_self());
    if (totalProduced >= 30) {
      printf("Producer %ld: 30 items have been produced. Exiting...\n", (long)pthread_self());
      pthread_mutex_unlock(&mut);
      pthread_exit(NULL);
    }
    while (num == BUF_SIZE) {
      wait_status = pthread_cond_wait(&cond, &mut);
      if (wait_status != 0) {
        fprintf(stderr, "Producer %ld: Error waiting on condition variable: %s\n", (long)pthread_self(), strerror(wait_status));
        pthread_mutex_unlock(&mut); // Let mutex unlocked before terminating
        exit(wait_status); // Exit the program with error code
      }
    }
    buffer[num] = i;
    num++;
    totalProduced++;
    printf("Producer %ld: Produced item %d, Buffer Size: %d, Total Produced: %d\n", (long)pthread_self(), i, num, totalProduced);
    i++;
    pthread_mutex_unlock(&mut);
    broadcast_status = pthread_cond_broadcast(&cond); // wake waiting threads
    if (broadcast_status != 0) {
      fprintf(stderr, "Producer %ld: Error broadcasting condition variable: %s\n", (long)pthread_self(), strerror(broadcast_status));
      exit(broadcast_status);  // Exit the program with error code
    }
    sleep(3);
  }
}

// Consumer thread
void *consumer(void *param) {
  printf("consumer %ld: Starting...\n", (long)pthread_self());
  int i = 0, wait_status, broadcast_status; 
  // wait_status and broadcast_status are used to store the return value 
  // of pthread_cond_wait and pthread_cond_broadcast
  while (1) {
    pthread_mutex_lock(&mut);
    printf("consumer %ld: Lock acquired...\n", (long)pthread_self());
    if (totalConsumed >= 30) {
      printf("Consumer %ld: 30 items have been consumed. Exiting...\n", (long)pthread_self());
      pthread_mutex_unlock(&mut);
      pthread_exit(NULL);
    }

    if (num < 0)
      exit(1);
    while (num == 0) {
      wait_status = pthread_cond_wait(&cond, &mut);
      if (wait_status != 0) {
        fprintf(stderr, "Consumer %ld: Error waiting on condition variable: %s\n", (long)pthread_self(), strerror(wait_status));
        pthread_mutex_unlock(&mut); 
        exit(wait_status); // Exit the program with error code
      }
    }
    i = buffer[--num]; 
    totalConsumed++; 
    printf("Consumer %ld: Consumed item %d, Buffer Size: %d, Total Consumed: %d\n", (long)pthread_self(), i, num, totalConsumed);
    pthread_mutex_unlock(&mut);
    broadcast_status = pthread_cond_broadcast(&cond); // wake waiting threads
    if (broadcast_status != 0) {
      fprintf(stderr, "Consumer %ld: Error broadcasting condition variable: %s\n", (long)pthread_self(), strerror(broadcast_status));
      exit(broadcast_status); // Exit the program with error code
    }
    sleep(8);
  }
}
