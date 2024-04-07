#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAX_BUFFER_SIZE 8196
#define DEFAULT_PERMISSION 0644

void handle_client(int client_sock) {
    char client_message[MAX_BUFFER_SIZE];
    memset(client_message, '\0', sizeof(client_message));

    // Receive client's message
    if (recv(client_sock, client_message, sizeof(client_message), 0) < 0) {
        printf("Couldn't receive\n");
        close(client_sock);
        return;
    }

    // Parse the client's message
    char command[20];
    char local_file_path[256];
    char remote_file_path[256];
    int permission = DEFAULT_PERMISSION;
    sscanf(client_message, "%s %s %s %o", command, local_file_path, remote_file_path, &permission);

    if (strcmp(command, "WRITE") == 0) {
        // If remote file path is omitted, use the local file path
        if (strlen(remote_file_path) == 0) {
            strcpy(remote_file_path, local_file_path);
        }

        // If permission is not provided, use the default permission
        if (permission == 0) {
            permission = DEFAULT_PERMISSION;
        }

        // Open the file for writing (overwrite if exists)
        FILE *file = fopen(remote_file_path, "wb");
        if (file == NULL) {
            printf("Failed to create file: %s\n", remote_file_path);
            char *error_message = "Failed to create file";
            send(client_sock, error_message, strlen(error_message), 0);
            close(client_sock);
            return;
        }

        // Receive the file data from the client
        char file_buffer[MAX_BUFFER_SIZE];
        int bytes_received;
        while ((bytes_received = recv(client_sock, file_buffer, sizeof(file_buffer), 0)) > 0) {
            fwrite(file_buffer, 1, bytes_received, file);
            fflush(file); // Flush the buffer to disk after each write
        }

        fclose(file);

        // Set the file permission
        if (chmod(remote_file_path, permission) < 0) {
            printf("Failed to set file permission: %s\n", remote_file_path);
            char *error_message = "Failed to set file permission";
            send(client_sock, error_message, strlen(error_message), 0);
        } else {
            printf("File received and saved: %s\n", remote_file_path);
            char *success_message = "File successfully written";
            send(client_sock, success_message, strlen(success_message), 0);
        }
    } else {
        printf("Invalid command: %s\n", command);
        char *error_message = "Invalid command";
        send(client_sock, error_message, strlen(error_message), 0);
    }

    close(client_sock);
}

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
    printf("Socket created successfully\n");

    // Set port and IP
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Bind to the set port and IP
    if (bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Couldn't bind to the port\n");
        return -1;
    }
    printf("Done with binding\n");

    // Listen for clients
    if (listen(socket_desc, 1) < 0) {
        printf("Error while listening\n");
        close(socket_desc);
        return -1;
    }
    printf("\nListening for incoming connections.....\n");

    while (1) {
        // Accept an incoming connection
        client_size = sizeof(client_addr);
        client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);
        if (client_sock < 0) {
            printf("Can't accept\n");
            close(socket_desc);
            return -1;
        }
        printf("Client connected at IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Handle the client connection in a separate function
        handle_client(client_sock);
    }

    close(socket_desc);
    return 0;
}


