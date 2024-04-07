#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_BUFFER_SIZE 8196

int main(int argc, char *argv[]) {
    int socket_desc;
    struct sockaddr_in server_addr;
    char server_message[MAX_BUFFER_SIZE], client_message[MAX_BUFFER_SIZE];

    // Check if the correct number of arguments is provided
    if (argc < 3) {
        printf("Usage: %s WRITE local-file-path [remote-file-path] [permission]\n", argv[0]);
        return -1;
    }

    // Get the command, local file path, remote file path, and permission from command-line arguments
    char *command = argv[1];
    char *local_file_path = argv[2];
    char *remote_file_path = (argc > 3) ? argv[3] : local_file_path;
    char *permission = (argc > 4) ? argv[4] : "0644";

    // Clean buffers
    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));

    // Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc < 0) {
        printf("Unable to create socket\n");
        return -1;
    }
    printf("Socket created successfully\n");

    // Set port and IP the same as server-side
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Send connection request to server
    if (connect(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Unable to connect\n");
        return -1;
    }
    printf("Connected with server successfully\n");

    // Send the command, file paths, and permission to the server
    sprintf(client_message, "%s %s %s %s", command, local_file_path, remote_file_path, permission);
    if (send(socket_desc, client_message, strlen(client_message), 0) < 0) {
        printf("Unable to send message\n");
        return -1;
    }

    // Open the local file for reading
    FILE *file = fopen(local_file_path, "rb");
    if (file == NULL) {
        printf("Failed to open file: %s\n", local_file_path);
        close(socket_desc);
        return -1;
    }

    // Send the file data to the server
    char file_buffer[MAX_BUFFER_SIZE];
    int bytes_read;
    while ((bytes_read = fread(file_buffer, 1, sizeof(file_buffer), file)) > 0) {
        send(socket_desc, file_buffer, bytes_read, 0);
    }

    fclose(file);

    // Receive the confirmation message from the server
    char confirmation_message[MAX_BUFFER_SIZE];
    memset(confirmation_message, '\0', sizeof(confirmation_message));
    recv(socket_desc, confirmation_message, sizeof(confirmation_message), 0);

    // Provide user feedback based on the server's response
    if (strcmp(confirmation_message, "File successfully written") == 0) {
        printf("File transfer successful. File saved on the server.\n");
    } else if (strcmp(confirmation_message, "Failed to create file") == 0) {
        printf("File transfer failed. Unable to create the file on the server.\n");
    } else if (strcmp(confirmation_message, "Failed to set file permission") == 0) {
        printf("File transfer completed, but failed to set the file permission on the server.\n");
    } else if (strcmp(confirmation_message, "Invalid command") == 0) {
        printf("Invalid command. Please use the 'WRITE' command to transfer a file.\n");
    } else {
        printf("File transfer failed. Server response: %s\n", confirmation_message);
    }

    close(socket_desc);
    return 0;
}

