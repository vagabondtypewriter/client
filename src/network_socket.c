#include "../include/network_socket.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void *receive_messages(void *socket_fd);
void *send_messages(void *socket_fd);

int client_create(uint16_t port, const char *ip)
{
    struct client_information client;
    printf("FROM network_socket.C %i %s\n", port, ip);

    //     create socket
    client = socket_create(port, ip);
    if(socket_connect(client))
    {
        handle_connection(client.fd);
    }
    else
    {
        return -1;
    }
    printf("Client fd: %i\n", client.fd);
    return 1;
}

struct client_information socket_create(uint16_t port, const char *ip)
{
    int                       sockfd;
    struct sockaddr_in        server_addr;
    struct client_information newClient;

#ifndef SOCK_CLOEXEC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-macros"
    #define SOCK_CLOEXEC 0
    #pragma GCC diagnostic pop
#endif
    sockfd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);

    if(sockfd == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(port);
    if(inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0)
    {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    newClient.fd         = sockfd;
    newClient.serverAddr = server_addr;

    return newClient;
}

int socket_connect(struct client_information client)
{
    if(connect(client.fd, (struct sockaddr *)&(client.serverAddr), sizeof(client.serverAddr)) == -1)
    {
        perror("Connection failed");
        return -1;
    }

    printf("Connected to server\n");
    return 1;
}

//

int handle_connection(int server_socket)
{
    pthread_t receive_thread;
    pthread_t send_thread;

    // Create threads for receiving and sending messages
    if(pthread_create(&receive_thread, NULL, receive_messages, (void *)&server_socket))
    {
        perror("Could not create receive thread");
        exit(EXIT_FAILURE);
    }

    if(pthread_create(&send_thread, NULL, send_messages, (void *)&server_socket))
    {
        perror("Could not create send thread");
        exit(EXIT_FAILURE);
    }

    // Wait for threads to finish
    pthread_join(receive_thread, NULL);
    pthread_join(send_thread, NULL);

    return 1;
}

void *receive_messages(void *socket_fd)
{
    int server_socket = *((int *)socket_fd);
    //    int running       = 1;

    while(1)
    {
        struct message new_message;

        // Read the message version
        ssize_t bytes_received = read(server_socket, &new_message.version, sizeof(new_message.version));
        if(bytes_received <= 0)
        {
            if(bytes_received < 0)
            {
                perror("recv");
            }
            else
            {
                printf("Server closed connection\n");
            }
            break;
        }

        // Read the message content size
        bytes_received = read(server_socket, &new_message.content_size, sizeof(new_message.content_size));
        if(bytes_received <= 0)
        {
            if(bytes_received < 0)
            {
                perror("recv");
            }
            else
            {
                printf("Server closed connection\n");
            }
            break;
        }
        new_message.content_size = ntohs(new_message.content_size);    // Ensure network to host short conversion

        // Read the message content based on the received size
        bytes_received = read(server_socket, new_message.content, new_message.content_size);
        if(bytes_received <= 0)
        {
            if(bytes_received < 0)
            {
                perror("read");
            }
            else
            {
                printf("Server closed connection\n");
            }
            break;
        }

        // Null terminate the received string
        if(bytes_received < BUFFER_SIZE)
        {
            new_message.content[bytes_received] = '\0';
        }
        else
        {
            // Ensure the buffer is null-terminated in case of full buffer
            new_message.content[BUFFER_SIZE - 1] = '\0';
        }

        //         Print the received message
        printf("Received message:\n");
        printf("Version: %u\n", new_message.version);
        printf("Content Size: %u\n", new_message.content_size);
        printf("Content: %s\n", new_message.content);
    }

    // Close the socket and cleanup if necessary
    // close(server_socket);
    return NULL;
}

void *send_messages(void *socket_fd)
{
    int  server_socket = *((int *)socket_fd);
    char message[BUFFER_SIZE];    // Assuming BUFFER_SIZE is defined

    while(1)
    {
        printf("Enter message: ");
        fgets(message, BUFFER_SIZE, stdin);
        // Exit loop condition or continue to send messages
        write(server_socket, message, strlen(message));
    }
}
