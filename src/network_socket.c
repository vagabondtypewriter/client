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
    // client connected
    // read/write concurrency
    // what type? -->
    int running = 1;

    printf("Handle connection\n");

    // continuously read from the server, handle accordingly
    while(running)
    {
        uint8_t  version;
        uint16_t content_size;
        char     content[BUFFER_SIZE];

        ssize_t bytes_received = read(server_socket, &version, sizeof(version));
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
        printf("Received version from server: %u\n", version);

        bytes_received = read(server_socket, &content_size, sizeof(content_size));
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
        content_size = ntohs(content_size);
        printf("Received content size from server: %u\n", content_size);

        bytes_received = read(server_socket, content, content_size);
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
        if(running)
        {
            content[bytes_received] = '\0';
            printf("Received content from server: %s\n", content);
        }
        running = 0;
    }

    return 1;
}
