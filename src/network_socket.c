#include "../include/network_socket.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int client_create(uint16_t port, const char *ip)
{
    struct client_information client;
    printf("FROM network_socket.C %i %s\n", port, ip);
    //
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

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
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

int handle_connection(int server_socket)
{
    int running = 1;

    printf("Handle connection\n");

    while(running)
    {
        struct message new_message;

        ssize_t bytes_received = read(server_socket, &new_message.version, sizeof(new_message.version));
        if(bytes_received < 0)
        {
            perror("recv");
            running = 0;
        }
        if(bytes_received == 0)
        {
            printf("Server closed connection\n");
            running = 0;
        }

        bytes_received = read(server_socket, &new_message.content_size, sizeof(new_message.content_size));
        if(bytes_received < 0)
        {
            perror("recv");
            running = 0;
        }
        if(bytes_received == 0)
        {
            printf("Server closed connection\n");
            running = 0;
        }
        new_message.content_size = ntohs(new_message.content_size);

        bytes_received = read(server_socket, new_message.content, new_message.content_size);
        if(bytes_received < 0)
        {
            perror("read");
            running = 0;
        }
        if(bytes_received == 0)
        {
            printf("Server closed connection\n");
            running = 0;
        }
        if(running)
        {
            new_message.content[bytes_received] = '\0';
            printf("Stored in message struct: \n");
            printf("Received version from server: %i\n", new_message.version);
            printf("Received version from server: %i\n", new_message.content_size);
            printf("Received content from server: %s\n", new_message.content);
        }
        running = 0;
    }

    return 1;
}
