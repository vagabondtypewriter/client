#ifndef CLIENT_SERVER_CHAT_SERVICE_NETWORK_SOCKET_H
#define CLIENT_SERVER_CHAT_SERVICE_NETWORK_SOCKET_H

#include <ncurses.h>
#include <netinet/in.h>
#include <stdint.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024
#define QUIT_COMMAND "/q\n"

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

struct received_message
{
    char content[UINT16_MAX];
};

int                       client_create(uint16_t port, const char *ip);
struct client_information socket_create(uint16_t port, const char *ip);
int                       socket_connect(struct client_information);
int                       handle_connection(int server_socket);
void                      add_received_message(const char *message);
void                      display_received_messages(WINDOW *output_win);
void                     *receive_messages(void *socket_fd);
void                     *send_messages(void *socket_fd);

#endif    // CLIENT_SERVER_CHAT_SERVICE_NETWORK_SOCKET_H
