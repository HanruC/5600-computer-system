/*
 * server.c - Server program for remote file system operations
 *
 * This program implements a server that handles client requests for
 * various file system operations such as writing files, retrieving files,
 * removing files, and checking file permissions.
 *
 * The server uses TCP sockets to communicate with clients and supports
 * concurrent client connections using multithreading.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>

#define MAX_BUFFER_SIZE 8196
#define DEFAULT_PERMISSION 0644  // rw-r--r--

pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;
void *handle_client(void *arg);
void write_file(int client_sock, char *local_file_path, char *remote_file_path, int permission);
void get_file(int client_sock, char *remote_file_path, char *local_file_path);
void remove_file(int client_sock, char *remote_file_path);
void check_file_permission(int client_sock, char *remote_file_path);

/*
 * Function: main
 * --------------
 * The main function of the server program.
 *
 * It creates a socket, binds it to a specified IP address and port, and listens
 * for incoming client connections. It handles each client connection in a separate
 * thread using the handle_client function.
 *
 * Returns 0 on successful execution, -1 on failure.
 */
int main(void) {
    int socket_desc, client_sock;
    socklen_t client_size;
    struct sockaddr_in server_addr, client_addr;

    // Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc < 0) {
        printf("Error while creating socket\n");
        return -1;
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Setsockopt failed");
        exit(0);
    }

    printf("Socket created successfully\n");

    // Set port and IP
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Bind to the set port and IP
    if (bind(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        printf("Bind failed\n");
        return -1;
    }
    printf("Bind done\n");

    // Listen for incoming connections
    listen(socket_desc, 3);

    printf("Waiting for incoming connections on port 2000...\n");

    // Accept incoming connection
    client_size = sizeof(struct sockaddr_in);
    pthread_t thread_id;
    while (1) {
        client_sock = accept(socket_desc, (struct sockaddr *)&client_addr, &client_size);
        if (client_sock < 0) {
            printf("Can't accept\n");
            close(socket_desc);
            return -1;
        }
        printf("Client connected: client_sock(%d) at IP: %s and port: %i\n", client_sock, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Create a new thread for handling the client
        if (pthread_create(&thread_id, NULL, handle_client, (void *)&client_sock) < 0) {
            printf("Could not create thread\n");
            close(socket_desc);
            break;
        }
    }

    if (client_sock < 0) {
        printf("Accept failed\n");
        return -1;
    }

    return 0;
}

/*
 * Function: handle_client
 * -----------------------
 * Handles the communication with a client in a separate thread.
 *
 * arg: A pointer to the client socket descriptor.
 *
 * Returns: NULL.
 */
void *handle_client(void *arg) {
    int client_sock = *((int *)arg);
    char client_message[MAX_BUFFER_SIZE];
    memset(client_message, '\0', sizeof(client_message));

    // Receive client's message
    if (recv(client_sock, client_message, sizeof(client_message), 0) < 0) {
        printf("Couldn't receive\n");
        close(client_sock);
        pthread_exit(NULL);
    }

    // Parse the client's message
    char command[20];
    char local_file_path[256];
    char remote_file_path[256];
    int permission;
    sscanf(client_message, "%s %s %s %o", command, local_file_path, remote_file_path, &permission);

    if (strcmp(command, "WRITE") == 0) {
        write_file(client_sock, local_file_path, remote_file_path, permission);
    } else if (strcmp(command, "GET") == 0) {
        get_file(client_sock, remote_file_path, local_file_path);
    } else if (strcmp(command, "RM") == 0) {
        remove_file(client_sock, local_file_path);
    } else if (strcmp(command, "LS") == 0) {
        check_file_permission(client_sock, local_file_path);
    } else if (strcmp(command, "STOP") == 0){
        printf("Received STOP command. Closing connection.\n");
        close(client_sock);
        exit(0);
    } else {
        printf("Invalid command: %s\n", command);
    }

    close(client_sock);
    pthread_exit(NULL);
}

/*
 * Function: write_file
 * --------------------
 * Handles the client's request to write a file to the server.
 *
 * client_sock: The socket descriptor for the client connection.
 * local_file_path: The path of the local file on the client side.
 * remote_file_path: The path of the file to be created or overwritten on the server.
 * permission: The file permission value for the remote file.
 */
void write_file(int client_sock, char *local_file_path, char *remote_file_path, int permission) {
    struct stat file_stat;
    if (stat(remote_file_path, &file_stat) == 0) {
        if ((file_stat.st_mode & S_IWUSR) == 0) {
            printf("Cannot write to read-only file: %s\n", remote_file_path);
            send(client_sock, "Cannot write to read-only file", 30, 0);
            return;
        }
    }

    if (strlen(remote_file_path) == 0) {
        strcpy(remote_file_path, local_file_path);
    }

    if (permission == 0) {
        permission = DEFAULT_PERMISSION;
    }

    pthread_mutex_lock(&file_mutex);
    FILE *file = fopen(remote_file_path, "wb");
    if (file == NULL) {
        printf("Failed to create file: %s\n", remote_file_path);
        close(client_sock);
        pthread_mutex_unlock(&file_mutex);
        return;
    }

    char file_buffer[MAX_BUFFER_SIZE];
    int bytes_received;
    while ((bytes_received = recv(client_sock, file_buffer, sizeof(file_buffer), 0)) > 0) {
        fwrite(file_buffer, 1, bytes_received, file);
    }

    fclose(file);
    printf("Setting file permission to: %o\n", permission);
    if (chmod(remote_file_path, permission) < 0) {
        printf("Failed to set file permission: %s\n", remote_file_path);
    } else {
        printf("Permissions set to %o for file: %s\n", permission, remote_file_path);
    }

    pthread_mutex_unlock(&file_mutex);
}

/*
 * Function: get_file
 * ------------------
 * Handles the client's request to retrieve a file from the server.
 *
 * client_sock: The socket descriptor for the client connection.
 * remote_file_path: The path of the file to be retrieved from the server.
 * local_file_path: The path of the local file on the client side.
 */
void get_file(int client_sock, char *remote_file_path, char *local_file_path) {
    if (strlen(local_file_path) == 0) {
        strcpy(local_file_path, "./");
    }

    FILE *file = fopen(remote_file_path, "rb");
    if (file == NULL) {
        printf("Failed to open file: %s\n", remote_file_path);
        close(client_sock);
        return;
    }

    char file_buffer[MAX_BUFFER_SIZE];
    int bytes_read;
    while ((bytes_read = fread(file_buffer, 1, sizeof(file_buffer), file)) > 0) {
        ssize_t sent_bytes = send(client_sock, file_buffer, bytes_read, 0);
        if (sent_bytes < 0) {
            perror("Error: Failed to send data");
        }
    }

    fclose(file);
}

/*
 * Function: remove_file
 * ---------------------
 * Handles the client's request to remove a file from the server.
 *
 * client_sock: The socket descriptor for the client connection.
 * remote_file_path: The path of the file to be removed from the server.
 */
void remove_file(int client_sock, char *remote_file_path) {
    struct stat file_stat;
    printf("remote_file_path %s\n", remote_file_path);

    if (stat(remote_file_path, &file_stat) < 0) {
        printf("Failed to get file stats: %s\n", remote_file_path);
        perror("stat");
        send(client_sock, "FAIL", 4, 0);
        return;
    }

    if ((file_stat.st_mode & S_IWUSR) == 0) {
        printf("Cannot delete read-only file: %s\n", remote_file_path);
        send(client_sock, "FAIL", 4, 0);
        return;
    }

    if (remove(remote_file_path) != 0) {
        printf("Failed to remove file: %s\n", remote_file_path);
        perror("remove");
        send(client_sock, "FAIL", 4, 0);
    } else {
        printf("File removed: %s\n", remote_file_path);
        send(client_sock, "OK", 2, 0);
    }
}

/*
 * Function: check_file_permission
 * -------------------------------
 * Handles the client's request to check the permission of a file on the server.
 *
 * client_sock: The socket descriptor for the client connection.
 * remote_file_path: The path of the file to check the permission on the server.
 */
void check_file_permission(int client_sock, char *remote_file_path) {
    struct stat file_stat;
    if (stat(remote_file_path, &file_stat) == 0) {
        char permission_str[MAX_BUFFER_SIZE];
        if ((file_stat.st_mode & S_IRUSR) && (file_stat.st_mode & S_IWUSR)) {
            sprintf(permission_str, "%s: read-write", remote_file_path);
        } else if (file_stat.st_mode & S_IRUSR) {
            sprintf(permission_str, "%s: read-only", remote_file_path);
        } else {
            sprintf(permission_str, "%s: no read permission", remote_file_path);
        }
        send(client_sock, permission_str, strlen(permission_str), 0);
    } else {
        char error_msg[MAX_BUFFER_SIZE];
        sprintf(error_msg, "File not found: %s", remote_file_path);
        send(client_sock, error_msg, strlen(error_msg), 0);
    }
}