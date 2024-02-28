#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "queue.h"
#include "polybius.h"


unsigned int seed; 

unsigned int LCG() {
    const unsigned int a = 1103515245;
    const unsigned int c = 12345;
    const unsigned int m = 2147483648;
    seed = (a * seed + c) % m;
    return seed;
}
                                            
int genRand(int min, int max) {
    return LCG() % (max - min + 1) + min;
}

char* genWord(int minLength, int maxLength) {
    int length = genRand(minLength, maxLength);// generate a random length
    char* word = (char*)malloc((length + 1) * sizeof(char)); // allocate memory for the word
    if (word == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1); 
    }

    for (int i = 0; i < length; i++) {
        char letter = genRand(0, 51); // 26 lowercase, 26 uppercase (0-51)
        if (letter < 26) {
            word[i] = 'a' + letter; // lowercase
        } else {
            word[i] = 'A' + letter - 26; // uppercase
        }
    }
    word[length] = '\0'; // null-terminate the string
    return word; 
}

// Initialize the queue
void initQueue(queue_t* queue) {
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
}

int main(int argc, char *argv[]) {
        if (argc < 3 || argc > 4) {
        fprintf(stderr, "Error: Incorrect number of arguments\n");
        fprintf(stderr, "Usage: %s <numOfWords> <fileName>\n", argv[0]);
        fprintf(stderr, "Usage for processing words: %s -p <fileName>\n", argv[0]);
        return 1;
    }

        const char *fileName = argv[2];
        int numOfWords = atoi(argv[1]);
        if (numOfWords < 10000) {
            fprintf(stderr, "Error: Number of words must be at least 10000\n");
            return 1;
        }

        // Task 1: write to the file 
        FILE *fp = fopen(fileName, "w");
        if (fp == NULL) {
            fprintf(stderr, "Cannot open file %s\n", fileName);
            return 1;
        }

        seed = time(NULL);
        for (int i = 0; i < numOfWords; i++) {
            char* word = genWord(1, 15); // Generate each word
            fprintf(fp, "%s\n", word); // Write each word to the file
            free(word); // Free the dynamically allocated word
        }
        fclose(fp); // Close the file after writing


        // task 2: read from the file 
        fp = fopen(fileName, "r");
        if (fp == NULL) {
            fprintf(stderr, "Cannot open file %s\n", fileName);
            return 1;
        }
        queue_t queue;
        initQueue(&queue); // Initialize the queue

        char wordBuffer[256]; // Buffer to store each word
        while (fscanf(fp, "%s", wordBuffer) == 1) {
            char* word = strdup(wordBuffer); // Duplicate the word
            if (!word) {
                fprintf(stderr, "Failed to allocate memory for word.\n");
                fclose(fp);
                return 1;
            }
            add2q(&queue, word); // Add the word to the queue
        }
        fclose(fp);

        // task 3 
        int batchSize = 100;
        int processCount = 0;
        while (qsize(&queue) > 0) {
            int pipefd[2];
            if (pipe(pipefd) < 0) {
                perror("pipe");
                exit(EXIT_FAILURE);
            }

            pid_t pid = fork();
            if (pid < 0) {
                perror("fork");
                exit(EXIT_FAILURE);
            }

            if (pid == 0) { // Child process
                close(pipefd[1]); // Close write end

                if (dup2(pipefd[0], STDIN_FILENO) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
                close(pipefd[0]); // Close read end after dup2

                char outputFile[100];
                snprintf(outputFile, sizeof(outputFile), "encrypted_output_%d.txt", getpid());
                freopen(outputFile, "w", stdout);

                if (execl("./polybius", "./polybius", (char *)NULL) == -1) {
                    perror("Failed to execute polybius");
                    exit(EXIT_FAILURE);
                }

            } else { // Parent process
                close(pipefd[0]); // Close read end

                // Writing words to the pipe
                for (int i = 0; i < batchSize && qsize(&queue) > 0; i++) {
                    char* word = popQ(&queue);
                    if (write(pipefd[1], word, strlen(word)) == -1) {
                        perror("write");
                        exit(EXIT_FAILURE);
                    }
                    if (write(pipefd[1], "\n", 1) == -1) {
                        perror("write");
                        exit(EXIT_FAILURE);
                    }
                    free(word);
                }
                close(pipefd[1]); // Signal EOF to the child
                processCount++;
            }
        }

        // Wait for child processes to finish
        while (processCount > 0) {
            wait(NULL);
            processCount--;
        }

        return 0;
    }