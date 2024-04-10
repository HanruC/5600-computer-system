/*
 * client.c - Client program for remote file system operations
 *
 * This program implements a client that connects to a remote server and performs
 * various file system operations such as writing files, retrieving files,
 * removing files, and checking file permissions.
 *
 * The client communicates with the server using TCP sockets and sends commands
 * and file data based on user input from the command line interface.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 2000
#define MAX_BUFFER_SIZE 8196
#define DEFAULT_PERMISSION 0644  // rw-r--r--

void send_command(int socket_desc, const char *command, const char *local_file_path, const char *remote_file_path, int permission);
void write_file(int socket_desc, const char *local_file_path, const char *remote_file_path, int permission);
void get_file(int socket_desc, const char *remote_file_path, const char *local_file_path);
void remove_file(int socket_desc, const char *remote_file_path);
void check_permission(int socket_desc, const char *remote_file_path);
void stop_server(int socket_desc);

/*
 * Function: main
 * --------------
 * The main function of the client program.
 *
 * It establishes a connection with the server, provides a command line interface
 * for the user to enter commands, and calls the appropriate functions based on
 * the user's input.
 *
 * The available commands are:
 * - WRITE: Write a local file to the remote server.
 * - GET: Retrieve a file from the remote server and save it locally.
 * - RM: Remove a file from the remote server.
 * - LS: Check the permission of a file on the remote server.
 *
 * The command format is: % rfs <command> <local_file_path> <remote_file_path> [permission]
 *
 * Returns 0 on successful execution, -1 on failure.
 */
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
    int n = 1;
    while (n--) {
        memset(command, 0, MAX_BUFFER_SIZE);
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

                // Check the operation type for write
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

                    token = strtok(NULL, " ");
                    int permission = (token != NULL) ? strtol(token, NULL, 8) : DEFAULT_PERMISSION;

                    // Call write_file function
                    write_file(socket_desc, local_file_path, remote_file_path, permission);

                    // check the command type for get
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

                    // check the command type for remove
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

                    // check the command type for check permission
                } else if (strcmp(token, "LS") == 0) {
                    // Parse remote file path
                    token = strtok(NULL, " ");
                    if (token == NULL) {
                        printf("Invalid command\n");
                        continue;
                    }
                    char *remote_file_path = token;

                    // Call check_permission function
                    check_permission(socket_desc, remote_file_path);
                } else if (strcmp(token, "STOP") == 0) {
                    stop_server(socket_desc);
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

/*
 * Function: send_command
 * ----------------------
 * Sends a command to the server along with the necessary parameters.
 *
 * socket_desc: The socket descriptor for the client-server connection.
 * command: The command to be sent to the server.
 * local_file_path: The path of the local file (if applicable).
 * remote_file_path: The path of the remote file (if applicable).
 * permission: The file permission value (if applicable).
 */
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

/*
 * Function: write_file
 * --------------------
 * Writes a local file to the remote server.
 *
 * socket_desc: The socket descriptor for the client-server connection.
 * local_file_path: The path of the local file to be written.
 * remote_file_path: The path of the remote file to be created or overwritten.
 * permission: The file permission value for the remote file.
 */
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
        // printf("sent success %s",file_buffer);
    }

    fclose(file);
}

/*
 * Function: get_file
 * ------------------
 * Retrieves a file from the remote server and saves it locally.
 *
 * socket_desc: The socket descriptor for the client-server connection.
 * remote_file_path: The path of the remote file to be retrieved.
 * local_file_path: The path of the local file to be created or overwritten.
 */
void get_file(int socket_desc, const char *remote_file_path, const char *local_file_path) {
    send_command(socket_desc, "GET", local_file_path, remote_file_path, 0);
    printf("get_file local_file_path %s\n", local_file_path);
    FILE *file = fopen(local_file_path, "wb");
    if (file == NULL) {
        printf("Failed to create file: %s\n", local_file_path);
        return;
    }

    char file_buffer[MAX_BUFFER_SIZE];
    int bytes_received;
    while ((bytes_received = recv(socket_desc, file_buffer, sizeof(file_buffer), 0)) > 0) {
        fwrite(file_buffer, 1, bytes_received, file);
        printf("get_file success \n");
    }

    fclose(file);
}

/*
 * Function: remove_file
 * ---------------------
 * Removes a file from the remote server.
 *
 * socket_desc: The socket descriptor for the client-server connection.
 * remote_file_path: The path of the remote file to be removed.
 */
void remove_file(int socket_desc, const char *remote_file_path) {
    send_command(socket_desc, "RM", "", remote_file_path, 0);

    char server_reply[MAX_BUFFER_SIZE];
    int bytes_received = recv(socket_desc, server_reply, sizeof(server_reply), 0);
    if (bytes_received <= 0) {
        printf("recv server remove_file msg failed \n");
    }
    if (strcmp(server_reply, "OK") == 0) {
        printf("File removed successfully: %s\n", remote_file_path);
    } else {
        printf("Failed to remove file: %s\n", remote_file_path);
    }
}

/*
 * Function: check_permission
 * --------------------------
 * Checks the permission of a file on the remote server.
 *
 * socket_desc: The socket descriptor for the client-server connection.
 * remote_file_path: The path of the remote file to check the permission.
 */
void check_permission(int socket_desc, const char *remote_file_path) {
    send_command(socket_desc, "LS", "", remote_file_path, 0);

    char server_reply[MAX_BUFFER_SIZE];
    int bytes_received = recv(socket_desc, server_reply, sizeof(server_reply), 0);
    if (bytes_received <= 0) {
        printf("Failed to receive server response\n");
    } else {
        printf("%s\n", server_reply);
    }
}

void stop_server(int socket_desc) {
    send_command(socket_desc, "STOP", "", "", 0);
    printf("Server stopped\n");
}