#include "../include/network_socket.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int client_create(uint16_t port, const char *ip)
{
    struct client_information client;
    client = socket_create(port, ip);
    // todo start displays
    if(socket_connect(client))
    {
        return 1;
        //        handle_connection(client.fd);
    }
    //    else
    //    {
    //        printf("Error while connecting to server!\n");
    //        return -1;
    //    }
    //    printf("End\n");
    //    // todo display cleanup
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

        ssize_t bytes_received = read(server_socket, &new_message.version, sizeof(new_message.version));
        if(bytes_received < 0)
        {
            perror("recv");
            break;
        }
        if(bytes_received == 0)
        {
            printf("Server closed connection\n");
            break;
        }

        bytes_received = read(server_socket, &new_message.content_size, sizeof(new_message.content_size));
        if(bytes_received < 0)
        {
            perror("recv");
            break;
        }
        if(bytes_received == 0)
        {
            printf("Server closed connection\n");
            break;
        }
        new_message.content_size = ntohs(new_message.content_size);

        bytes_received = read(server_socket, new_message.content, new_message.content_size);
        if(bytes_received < 0)
        {
            perror("read");
            break;
        }
        if(bytes_received == 0)
        {
            printf("Server closed connection\n");
            break;
        }
        new_message.content[bytes_received] = '\0';
        printf("Received version from server: %i\n", new_message.version);
        printf("Received content from from server: %i\n", new_message.content_size);
        // todo print to ncurses win
        printf("Received content from server: %s\n", new_message.content);
    }

    return NULL;
}

void *send_messages(void *socket_fd)
{
    uint16_t       net_content_size;
    int            server_socket = *((int *)socket_fd);
    struct message msg;

    while(1)
    {
        uint16_t message_len;
        printf("Enter message:\n");
        // todo mod this to use the ncurses window input (make a buffer that contains the characters typed and handle the last character == '\n' differently(?))
        // todo handle the /q command for quitting back to main
        if(fgets(msg.content, BUFFER_SIZE, stdin) == NULL)
        {
            // Handle error or end of input
            break;
        }

        msg.version = 1;

        //                uint16_t message_len = htons((uint16_t)strlen(msg.content) + 1);
        message_len = (uint16_t)strlen(msg.content);
        printf("%hu\n", message_len);

        if(msg.content[message_len - 1] == '\n')
        {
            msg.content[message_len - 1] = '\0';
        }

        // Ensure the message doesn't exceed maximum allowed content size
        if(message_len > BUFFER_SIZE - 1)
        {
            printf("Error: Message exceeds maximum allowed size.\n");
        }

        msg.content_size = message_len;
        net_content_size = htons(msg.content_size);

        // Send the message components
        write(server_socket, &msg.version, sizeof(msg.version));
        write(server_socket, &net_content_size, sizeof(net_content_size));
        write(server_socket, msg.content, message_len);
    }
    return NULL;
}
