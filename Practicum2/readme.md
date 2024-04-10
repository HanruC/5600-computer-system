# Remote File System (RFS)

The Remote File System (RFS) is a client-server application that allows clients to perform various file system operations on a remote server. The server manages the remote file system and handles client requests, while the client provides a command-line interface for interacting with the server.

## Features

- Write files from the client to the remote server
- Retrieve files from the remote server to the client
- Remove files from the remote server
- Check file permissions on the remote server
- Concurrent client support using multithreading

## Compile

To compile the client and server programs, use the following commands:

​
- server compile:
gcc s.c -o s -pthread    

- client compile:
gcc c.c -o c -pthread
​

### Running the Server

To start the server, run the following command:

​```
./s
​```
The server will listen for incoming client connections on port 2000.

### Running the Client

To run the client, use the following command:

​```
./c
​```
The client will connect to the server running on `127.0.0.1` (localhost) and port 2000.

### Client Commands

The client supports the following commands:

- `% rfs WRITE <local_file_path> <remote_file_path> [permission]`: Write a local file to the remote server.
- `% rfs GET <remote_file_path> <local_file_path>`: Retrieve a file from the remote server and save it locally.
- `% rfs RM <remote_file_path>`: Remove a file from the remote server.
- `% rfs LS <remote_file_path>`: Check the permission of a file on the remote server.
- `% rfs STOP`: STOP server.

The `[permission]` argument in the `WRITE` command is optional and specifies the file permission value for the remote file. If not provided, the default permission `0644` (rw-r--r--) will be used.

For specific test command, you can find in the test.txt and copy and paste to different terminal to test. 

## File Permissions

The server supports setting and checking file permissions. When a file is written to the server using the `WRITE` command, the client can specify the file permission value. The server will set the permissions of the created file accordingly.

The `LS` command allows the client to check the permissions of a file on the remote server. The server will respond with the file path and its permissions (read-write, read-only, or no read permission).

## Concurrency

The server supports concurrent client connections using multithreading. Each client connection is handled in a separate thread, allowing multiple clients to interact with the server simultaneously. The server uses a mutex lock to ensure thread safety when accessing shared resources, such as files.

## Error Handling

The client and server programs include error handling for various scenarios, such as file I/O errors, socket communication errors, and invalid commands. Appropriate error messages are displayed to the user, and the programs gracefully handle exceptions.

