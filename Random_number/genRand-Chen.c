#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


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

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Error: Incorrect number of arguments\n");
        fprintf(stderr, "Correct format: ./genRand <numOfInt> <fileName> [-a]\n");
        return 1;
    }

    int numOfInt = atoi(argv[1]);
    const char *fileName = argv[2];
    int append = (argc == 4 && strcmp(argv[3], "-a") == 0); // check whether -a is provided 

    FILE *fp = fopen(fileName, append ? "a" : "w"); // open file if -a, or in write mode. 
    if (fp == NULL) {
        fprintf(stderr, "Cannot open file %s\n", fileName);
        return 1;
    }

    seed = time(NULL);

    int minRange = 1;
    int maxRange = 100;

    for (int i = 0; i < numOfInt; i++) {
        int num = genRand(minRange, maxRange);
        fprintf(fp, "%d\n", num);
    }

    fclose(fp);
    return 0;
}