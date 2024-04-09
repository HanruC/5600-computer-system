#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 8196

void process_write_command(int socket_desc, char *local_file_path, char *remote_file_path, char *permission, int append_flag) {
    char client_message[MAX_BUFFER_SIZE];
    sprintf(client_message, "WRITE %s %s %s %d", local_file_path, remote_file_path, permission, append_flag);
    send(socket_desc, client_message, strlen(client_message), 0);

    FILE *file = fopen(local_file_path, "rb");
    if (file == NULL) {
        perror("Failed to open file");
        exit(1);
    }

    char file_buffer[MAX_BUFFER_SIZE];
    int bytes_read;
    while ((bytes_read = fread(file_buffer, 1, sizeof(file_buffer), file)) > 0) {
        send(socket_desc, file_buffer, bytes_read, 0);
    }

    fclose(file);
    shutdown(socket_desc, SHUT_WR);  // Signal end of file transfer

    char server_response[MAX_BUFFER_SIZE];
    recv(socket_desc, server_response, sizeof(server_response), 0);
    printf("Server response: %s\n", server_response);
}

void process_get_command(int socket_desc, char *remote_file_path, char *local_file_path, int append_flag) {
    char client_message[MAX_BUFFER_SIZE];
    sprintf(client_message, "GET %s %d", remote_file_path, append_flag);
    send(socket_desc, client_message, strlen(client_message), 0);

    FILE *file = fopen(local_file_path, append_flag ? "ab" : "wb");
    if (file == NULL) {
        perror("Failed to create file");
        exit(1);
    }

    char file_buffer[MAX_BUFFER_SIZE];
    int bytes_received;
    while ((bytes_received = recv(socket_desc, file_buffer, sizeof(file_buffer), 0)) > 0) {
        fwrite(file_buffer, 1, bytes_received, file);
        fflush(file);
    }

    fclose(file);
    printf("File received and saved: %s\n", local_file_path);
}

int main(int argc, char *argv[]) {
    int socket_desc;
    struct sockaddr_in server_addr;

    if (argc < 2) {
        printf("Usage: %s COMMAND [arguments...]\n", argv[0]);
        return 1;
    }

    char *command = argv[1];
    char *local_file_path = "";
    char *remote_file_path = "";
    char *permission = "0644";
    int append_flag = 0;

    if (strcmp(command, "WRITE") == 0) {
        if (argc < 4) {
            printf("Usage: %s WRITE local-file-path remote-file-path [permission] [-a]\n", argv[0]);
            return 1;
        }
        local_file_path = argv[2];
        remote_file_path = argv[3];
        permission = (argc > 4 && argv[4][0] != '-') ? argv[4] : "0644";
        append_flag = (argc > 5 && strcmp(argv[5], "-a") == 0) || (argc > 4 && strcmp(argv[4], "-a") == 0);
    } else if (strcmp(command, "GET") == 0) {
        if (argc < 3) {
            printf("Usage: %s GET remote-file-path [local-file-path] [-a]\n", argv[0]);
            return 1;
        }
        remote_file_path = argv[2];
        local_file_path = (argc > 3 && argv[3][0] != '-') ? argv[3] : "downloaded_file";
        append_flag = (argc > 4 && strcmp(argv[4], "-a") == 0) || (argc > 3 && strcmp(argv[3], "-a") == 0);
    } else {
        printf("Invalid command: %s\n", command);
        return 1;
    }

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc < 0) {
        perror("Unable to create socket");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Unable to connect");
        return 1;
    }

    printf("Connected with server successfully\n");

    if (strcmp(command, "WRITE") == 0) {
        process_write_command(socket_desc, local_file_path, remote_file_path, permission, append_flag);
    } else if (strcmp(command, "GET") == 0) {
        process_get_command(socket_desc, remote_file_path, local_file_path, append_flag);
    }

    close(socket_desc);
    return 0;
}