#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 2000
#define MAX_BUFFER_SIZE 8196

void send_command(int socket_desc, const char *command, const char *local_file_path, const char *remote_file_path, int permission);

void write_file(int socket_desc, const char *local_file_path, const char *remote_file_path, int permission);

void get_file(int socket_desc, const char *remote_file_path, const char *local_file_path);

void remove_file(int socket_desc, const char *remote_file_path);

int main() {
    int socket_desc;
    struct sockaddr_in server;
    char server_reply[MAX_BUFFER_SIZE];

    // Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        printf("Could not create socket\n");
        return -1;
    }

    // Set up server information
    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);

    // Connect to server
    if (connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Connect failed\n");
        return -1;
    }

    // Command line interface
    char command[MAX_BUFFER_SIZE];
    int n=1;
    while (n--) {
        memset(command,0,MAX_BUFFER_SIZE);
        printf("Enter command: ");
        fgets(command, sizeof(command), stdin);

        // Remove newline character
        command[strcspn(command, "\n")] = '\0';

        // Tokenize the command
        char *token = strtok(command, " ");
        if (token == NULL) {
            printf("Invalid command\n");
            continue;
        }

        if (strcmp(token, "%") == 0) {
            // Parse the command type
            token = strtok(NULL, " ");
            if (token == NULL) {
                printf("Invalid command\n");
                continue;
            }

            if (strcmp(token, "rfs") == 0) {
                // Parse the operation
                token = strtok(NULL, " ");
                if (token == NULL) {
                    printf("Invalid command\n");
                    continue;
                }

                if (strcmp(token, "WRITE") == 0) {
                    // Parse local file path
                    token = strtok(NULL, " ");
                    if (token == NULL) {
                        printf("Invalid command\n");
                        continue;
                    }
                    char *local_file_path = token;

                    // Parse remote file path
                    token = strtok(NULL, " ");
                    if (token == NULL) {
                        printf("Invalid command\n");
                        continue;
                    }
                    char *remote_file_path = token;

                    // Call write_file function
                    write_file(socket_desc, local_file_path, remote_file_path, 0644);
                } else if (strcmp(token, "GET") == 0) {
                    // Parse remote file path
                    token = strtok(NULL, " ");
                    if (token == NULL) {
                        printf("Invalid command\n");
                        continue;
                    }
                    char *remote_file_path = token;

                    // Parse local file path
                    token = strtok(NULL, " ");
                    if (token == NULL) {
                        printf("Invalid command\n");
                        continue;
                    }
                    char *local_file_path = token;

                    // Call get_file function
                    get_file(socket_desc, remote_file_path, local_file_path);
                } else if (strcmp(token, "RM") == 0) {
                    // Parse remote file path
                    token = strtok(NULL, " ");
                    if (token == NULL) {
                        printf("Invalid command\n");
                        continue;
                    }
                    char *remote_file_path = token;

                    // Call remove_file function
                    remove_file(socket_desc, remote_file_path);
                } else {
                    printf("Invalid operation\n");
                    continue;
                }
            } else {
                printf("Invalid command\n");
                continue;
            }
        } else {
            printf("Invalid command\n");
            continue;
        }
    }

    // Close the socket
    close(socket_desc);

    return 0;
}

void send_command(int socket_desc, const char *command, const char *local_file_path, const char *remote_file_path, int permission) {
    char message[MAX_BUFFER_SIZE];
    memset(message, 0, sizeof(message));
    if (permission != 0) {
        sprintf(message, "%s %s %s %o", command, local_file_path, remote_file_path, permission);
    } else {
        sprintf(message, "%s %s %s", command, local_file_path, remote_file_path);
    }
    send(socket_desc, message, strlen(message), 0);
}

void write_file(int socket_desc, const char *local_file_path, const char *remote_file_path, int permission) {
    FILE *file = fopen(local_file_path, "rb");
    if (file == NULL) {
        printf("Failed to open file: %s\n", local_file_path);
        return;
    }

    send_command(socket_desc, "WRITE", local_file_path, remote_file_path, permission);

    char file_buffer[MAX_BUFFER_SIZE];
    int bytes_read;
    while ((bytes_read = fread(file_buffer, 1, sizeof(file_buffer), file)) > 0) {
        send(socket_desc, file_buffer, bytes_read, 0);
        printf("sent success %s",file_buffer);
    }

    fclose(file);
}

void get_file(int socket_desc, const char *remote_file_path, const char *local_file_path) {
    send_command(socket_desc, "GET", local_file_path, remote_file_path, 0);
    printf("get_file local_file_path %s\n",local_file_path);
    FILE *file = fopen(local_file_path, "wb");
    if (file == NULL) {
        printf("Failed to create file: %s\n", local_file_path);
        return;
    }

    char file_buffer[MAX_BUFFER_SIZE];
    int bytes_received;
    while ((bytes_received = recv(socket_desc, file_buffer, sizeof(file_buffer),0)) > 0) {
        fwrite(file_buffer, 1, bytes_received, file);
        printf("get_file success \n");
    }

    fclose(file);
}

void remove_file(int socket_desc, const char *remote_file_path) {
    send_command(socket_desc, "RM", "", remote_file_path, 0);

    char server_reply[MAX_BUFFER_SIZE];
    int bytes_received = recv(socket_desc, server_reply, sizeof(server_reply), 0);
    if(bytes_received<=0){printf("recv server remove_file msg failed \n");}
    if (strcmp(server_reply, "OK") == 0) {
        printf("File removed successfully: %s\n", remote_file_path);
    } else {
        printf("Failed to remove file: %s\n", remote_file_path);
    }
}
