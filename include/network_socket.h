#ifndef CLIENT_SERVER_CHAT_SERVICE_NETWORK_SOCKET_H
#define CLIENT_SERVER_CHAT_SERVICE_NETWORK_SOCKET_H

#include <netinet/in.h>
#include <stdint.h>
#include <sys/socket.h>

struct client_information
{
    int                fd;
    struct sockaddr_in serverAddr;
};

int                       client_create(uint16_t port, const char *ip);
struct client_information socket_create(uint16_t port, const char *ip);
int                       socket_connect(struct client_information);
int                       handle_connection(void);

#endif    // CLIENT_SERVER_CHAT_SERVICE_NETWORK_SOCKET_H
