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

void *handle_client(void *arg) {
    int client_sock = *((int *) arg);
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
    //printf("command:%s local_file_path:%s remote_file_path:%s permission:%d \n", command, local_file_path, remote_file_path, permission);
    if (strcmp(command, "WRITE") == 0) {
        write_file(client_sock, local_file_path, remote_file_path, permission);
    } else if (strcmp(command, "GET") == 0) {
        get_file(client_sock, remote_file_path, local_file_path);
    } else if (strcmp(command, "RM") == 0) {
        remove_file(client_sock, local_file_path);
    } else {
        printf("Invalid command: %s\n", command);
    }

    close(client_sock);
    pthread_exit(NULL);
}

void write_file(int client_sock, char *local_file_path, char *remote_file_path, int permission) {
    //printf("client_sock(%d) write_file local_file_path(%s) remote_file_path(%s) permission(%d)\n",client_sock,local_file_path,remote_file_path,permission);
    // If remote file path is omitted, use the local file path
    
    if (strlen(remote_file_path) == 0) {
        strcpy(remote_file_path, local_file_path);
    }

    // If permission is not provided, use the default permission
    if (permission == 0) {
        permission = DEFAULT_PERMISSION;
    }

    pthread_mutex_lock(&file_mutex);
    // Open the file for writing (overwrite if exists)
    FILE *file = fopen(remote_file_path, "wb");
    if (file == NULL) {
        printf("Failed to create file: %s\n", remote_file_path);
        close(client_sock);
        pthread_mutex_unlock(&file_mutex); // unlock mutex before return
        return;
    }
    printf("open file success\n");
    // Receive the file data from the client
    char file_buffer[MAX_BUFFER_SIZE];
    int bytes_received;
    if ((bytes_received = recv(client_sock, file_buffer, sizeof(file_buffer),0)) > 0) {
        printf("bytes_received[%d] file_buffer[%s]\n",bytes_received,file_buffer);
        fwrite(file_buffer, 1, bytes_received, file);
    }

    printf("fclose file success\n");
    fclose(file);

    // Set the file permission
    if (chmod(remote_file_path, permission) < 0) {
        printf("Failed to set file permission: %s\n", remote_file_path);
    }

    printf("File received and saved: %s\n", remote_file_path);

    pthread_mutex_unlock(&file_mutex); // unlock mutex after writing to the file
}

void get_file(int client_sock, char *remote_file_path, char *local_file_path) {
    // If local file path is omitted, use current folder
    if (strlen(local_file_path) == 0) {
        strcpy(local_file_path, "./");
    }

    FILE *file = fopen(remote_file_path, "rb");
    if (file == NULL) {
        printf("Failed to open file: %s\n", remote_file_path);
        close(client_sock);
        return;
    }

    // Send file data to the client
    char file_buffer[MAX_BUFFER_SIZE];
    int bytes_read;
    while ((bytes_read = fread(file_buffer, 1, sizeof(file_buffer), file)) > 0) {
        ssize_t sent_bytes = send(client_sock, file_buffer, bytes_read, 0);
        printf("bytes_read %d sent_bytes %ld\n",bytes_read,sent_bytes);
        if (sent_bytes < 0) {
              // sent failed
              perror("Error: Failed to send data");
        } 
    }

    fclose(file);
}

void remove_file(int client_sock, char *remote_file_path) {
    printf("remote_file_path %s\n",remote_file_path);
    if (remove(remote_file_path) != 0) {
        printf("Failed to remove file: %s\n", remote_file_path);
        perror("remove");
        send(client_sock, "FAIL", 4, 0);
    } else {
        printf("File removed: %s\n", remote_file_path);
        send(client_sock, "OK", 2, 0);
    }
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
    
    // setsockopt otherwise sometime will bind failed
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
    if (bind(socket_desc, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {        
	printf("Bind failed\n");
        return -1;
    }
    printf("Bind done\n");

    // Listen for incoming connections
    listen(socket_desc, 3);

    printf("Waiting for incoming connections port 2000...\n");

    // Accept incoming connection
    client_size = sizeof(struct sockaddr_in);
    pthread_t thread_id;
    while (1) {
        client_sock = accept(socket_desc, (struct sockaddr *) &client_addr, &client_size);
        if (client_sock < 0) {
            printf("Can't accept\n");
            close(socket_desc);
            return -1;
        }
        printf("Client connected client_sock(%d) at IP: %s and port: %i\n", client_sock,inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        // Create a new thread for handling client
        if (pthread_create(&thread_id, NULL, handle_client, (void *) &client_sock) < 0) {
            printf("Could not create thread\n");
            return -1;
        }
    }

    if (client_sock < 0) {
        printf("Accept failed\n");
        return -1;
    }

    return 0;
}
       
