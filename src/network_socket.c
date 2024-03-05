#include "../include/network_socket.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

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
    // client connected
    // read/write concurrency
    // what type? -->
    int  running = 1;
    char buffer[BUFFER_SIZE];

    printf("Handle connection\n");

    // continuously read from the server, handle accordingly
    while(running)
    {
        // Read data from the server
        ssize_t bytes_received = recv(server_socket, buffer, BUFFER_SIZE, 0);
        if(bytes_received < 0)
        {
            perror("recv");
            running = 0;    // err
        }
        else if(bytes_received == 0)
        {
            printf("Server closed connection\n");
            running = 0;    // client was closed by server
        }
        else
        {
            // Print received data to the console
            printf("Received from server: %.*s\n", (int)bytes_received, buffer);

            // --> parse the string using binary
            // --> 3 bits for version, 6 bits for content size, then content

        }
    }

    return 1;
}

// todo handle the following:
//      - msg send
//      - msg rec
//      - commands
//      - username
