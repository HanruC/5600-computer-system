#!/bin/bash

# Wait for the server to start
sleep 1

# Client 1: Write a file
{
./client WRITE local_file1.txt remote_file.txt -a
} &

# Client 2: Write to the same file
{
./client WRITE local_file2.txt remote_file.txt -a
} &

# Client 3: Read the file
{
./client GET remote_file.txt downloaded_file.txt
} &

# Wait for all client commands to finish
wait

# Terminate the server
pkill -f ./server