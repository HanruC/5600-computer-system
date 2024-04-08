#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#define MAX_BUFFER_SIZE 8196
#define DEFAULT_PERMISSION 0644

void handle_client(int client_sock) {
    char client_message[MAX_BUFFER_SIZE];
    memset(client_message, 0, sizeof(client_message));

    // Receive client's message
    if (recv(client_sock, client_message, sizeof(client_message), 0) < 0) {
        perror("Couldn't receive");
        close(client_sock);
        return;
    }

    char command[20];
    sscanf(client_message, "%s", command);

    if (strcmp(command, "WRITE") == 0) {
        char local_file_path[256], remote_file_path[256];
        int permission = DEFAULT_PERMISSION;
        int append_flag = 0;
        sscanf(client_message, "%*s %s %s %o %d", local_file_path, remote_file_path, &permission, &append_flag);

        char *mode = append_flag ? "ab" : "wb";
        FILE *file = fopen(remote_file_path, mode);
        if (!file) {
            perror("Failed to open file for writing");
            printf("Remote file path: %s\n", remote_file_path);
            printf("Error opening file: %s\n", strerror(errno));
            send(client_sock, "Failed to open file for writing", strlen("Failed to open file for writing"), 0);
            close(client_sock);
            return;
        }

        char buffer[MAX_BUFFER_SIZE];
        int bytes_received;
        while ((bytes_received = recv(client_sock, buffer, sizeof(buffer), 0)) > 0) {
            if (fwrite(buffer, 1, bytes_received, file) != bytes_received) {
                perror("Failed to write to file");
                send(client_sock, "Failed to write to file", strlen("Failed to write to file"), 0);
                break;
            }
            fflush(file);
        }

        fclose(file);
        if (bytes_received >= 0) {
            chmod(remote_file_path, permission);
            send(client_sock, "File written successfully", strlen("File written successfully"), 0);
        } else {
            send(client_sock, "Failed to write file", strlen("Failed to write file"), 0);
        }
    } else if (strcmp(command, "GET") == 0) {
        char remote_file_path[256], local_file_path[256];
        int append_flag = 0;
        sscanf(client_message, "%*s %s %s %d", remote_file_path, local_file_path, &append_flag);

        FILE *file = fopen(remote_file_path, "rb");
        if (!file) {
            perror("Failed to open file for reading");
            send(client_sock, "Failed to open file for reading", strlen("Failed to open file for reading"), 0);
            close(client_sock);
            return;
        }

        char buffer[MAX_BUFFER_SIZE];
        int bytes_read;
        while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
            send(client_sock, buffer, bytes_read, 0);
        }
        
        fclose(file);
        // send(client_sock, "File sent successfully", strlen("File sent successfully"), 0);
    } else {
        send(client_sock, "Invalid command", strlen("Invalid command"), 0);
    }

    close(client_sock);
}

int main(void) {
    int socket_desc, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_size = sizeof(client_addr);

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        perror("Could not create socket");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2000);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    listen(socket_desc, 3);

    printf("Waiting for incoming connections...\n");
    while ((client_sock = accept(socket_desc, (struct sockaddr *)&client_addr, &client_size))) {
        printf("Connection accepted.\n");
        handle_client(client_sock);
    }

    if (client_sock < 0) {
        perror("Accept failed");
        return 1;
    }

    return 0;
}