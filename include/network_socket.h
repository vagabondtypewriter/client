#ifndef CLIENT_SERVER_CHAT_SERVICE_NETWORK_SOCKET_H
#define CLIENT_SERVER_CHAT_SERVICE_NETWORK_SOCKET_H

#include <netinet/in.h>
#include <stdint.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024

struct client_information
{
    int                fd;
    struct sockaddr_in serverAddr;
};

struct message
{
    uint8_t  version;
    uint16_t content_size;
    char     content[BUFFER_SIZE];    // Subtracting 3 for version and content_size
};

int  client_create(uint16_t port, const char *ip);
void socket_create(struct client_information *, uint16_t port, const char *ip);
int  socket_connect(const struct client_information *);
int  handle_connection(struct client_information *);

#endif    // CLIENT_SERVER_CHAT_SERVICE_NETWORK_SOCKET_H
