#include <stdio.h>

// Helper functions for checking if it is a lower case. 
int isLower(char c) {
    return c >= 'a' && c <= 'z';
}

// Helper functions for checking if it is a upper case.
int isUpper(char c) {
    return c >= 'A' && c <= 'Z';
}

// Helper functions for checking if it is a letter.
int isAlpha(char c) {
    return isLower(c) || isUpper(c);
}

// Helper to convert a char to upper case.
char toUpperCase(char c) {  
    if (isLower(c)) {
        return c - 32;
    }
    return c;
}

// Helper to convert a char to lower case.
char toLowerCase(char c) {
    if (isUpper(c)) {
        return c + 32;
    }
    return c;
}

// Helper to compare two strings.
int strCompare(char *str1, char *str2) {  
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            return str1[i] - str2[i];
        }
        i++;
    }
    return str1[i] - str2[i];
}

// Function to convert a string to upper case. -u
void ToUpperCase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toUpperCase(str[i]);
    }
}

// Function to convert a string to lower case. -l
void ToLowerCase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toLowerCase(str[i]);
    }
}

// Function to convert a string to capital case. -cap
void ToCap(char* str) {
    int first = 1;
    for (int i = 0; str[i]; i++) {
        if (isAlpha(str[i])) {
            if (first) {
                str[i] = toUpperCase(str[i]);
                first = 0;
            } else {
                str[i] = toLowerCase(str[i]);
            }
        } else {
            first = 1;
        }
    }
}

// Main function to check the arguments and call the helper functions.
int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Error: Too few arguments\n");
        return -1;
    }

    for (int i = 2; i < argc; i++) {
        if (strCompare(argv[1], "-u") == 0) {
            ToUpperCase(argv[i]);
        } else if (strCompare(argv[1], "-l") == 0) {
            ToLowerCase(argv[i]);
        } else if (strCompare(argv[1], "-cap") == 0) {
            ToCap(argv[i]);
        } else {
            fprintf(stderr, "Error: Invalid argument\n");
            return -1;
        }
        printf("%s ", argv[i]);
    }
    printf("\n");
    return 0;
}
