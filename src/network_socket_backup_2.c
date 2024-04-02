#include "../include/network_socket.h"
#include "../include/gui.h"
#include <arpa/inet.h>
#include <ncurses.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define OUTPUT_HEIGHT 20
#define INPUT_HEIGHT 5
#define WIDTH 100
#define BEGIN_X 0
#define BEGIN_Y_OUTPUT 0
#define BEGIN_Y_INPUT OUTPUT_HEIGHT
#define MAX_MESSAGES 100

static struct received_message received_messages[MAX_MESSAGES];    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
static int                     num_received_messages = 0;          // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

void add_received_message(const char *message)
{
    if(num_received_messages < MAX_MESSAGES)
    {
        strncpy(received_messages[num_received_messages].content, message, WIDTH - 1);
        received_messages[num_received_messages].content[WIDTH - 1] = '\0';    // Ensure null-termination
        num_received_messages++;
    }
    else
    {
        for(int i = 0; i < MAX_MESSAGES - 1; i++)
        {
            strncpy(received_messages[i].content, received_messages[i + 1].content, WIDTH - 1);
            received_messages[i].content[WIDTH - 1] = '\0';    // Ensure null-termination
        }
        strncpy(received_messages[MAX_MESSAGES - 1].content, message, WIDTH - 1);
        received_messages[MAX_MESSAGES - 1].content[WIDTH - 1] = '\0';    // Ensure null-termination
    }
}

void display_received_messages(WINDOW *output_win)
{
    int start_y;
    werase(output_win);
    start_y = OUTPUT_HEIGHT - num_received_messages;
    if(start_y < 0)
    {
        start_y = 0;
    }
    for(int i = start_y; i < OUTPUT_HEIGHT; i++)
    {
        mvwprintw(output_win, i - start_y, 1, "%s", received_messages[i].content);
    }
    wrefresh(output_win);
}

void *receive_messages(void *socket_fd)
{
    int     server_socket = *((int *)socket_fd);
    WINDOW *output_win    = newwin(OUTPUT_HEIGHT, WIDTH, BEGIN_Y_OUTPUT, BEGIN_X);
    int     running       = 1;

    while(running)
    {
        struct message new_message;
        char           display_message[WIDTH];
        ssize_t        bytes_received = read(server_socket, &new_message.version, sizeof(new_message.version));
        if(bytes_received <= 0)
        {
            add_received_message("Error receiving message from server.");
            running = 0;
            continue;
        }

        bytes_received = read(server_socket, &new_message.content_size, sizeof(new_message.content_size));
        if(bytes_received <= 0)
        {
            add_received_message("Error receiving message from server.");
            running = 0;
            continue;
        }
        new_message.content_size = ntohs(new_message.content_size);

        bytes_received = read(server_socket, new_message.content, new_message.content_size);
        if(bytes_received <= 0)
        {
            add_received_message("Error receiving message from server.");
            running = 0;
            continue;
        }

        new_message.content[bytes_received] = '\0';

        snprintf(display_message, WIDTH, "Received content from server: %s", new_message.content);
        add_received_message(display_message);
        display_received_messages(output_win);
    }

    delwin(output_win);
    return NULL;
}

int client_create(uint16_t port, const char *ip)
{
    struct client_information client;
    printf("Connecting to: %i %s\n", port, ip);

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

int handle_connection(int server_socket)
{
    pthread_t receive_thread;
    pthread_t send_thread;

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

    pthread_join(receive_thread, NULL);
    pthread_join(send_thread, NULL);

    return 1;
}

void *send_messages(void *socket_fd)
{
    int server_socket = *((int *)socket_fd);

    WINDOW *input_win = newwin(INPUT_HEIGHT, WIDTH, BEGIN_Y_INPUT, BEGIN_X);

    while(1)
    {
        struct message msg;
        uint16_t       message_len;

        display_input(input_win, "Enter message:");

        echo();
        wgetnstr(input_win, msg.content, WIDTH);
        noecho();

        msg.version = 1;

        message_len      = (uint16_t)strlen(msg.content);
        msg.content_size = htons(message_len);

        write(server_socket, &msg.version, sizeof(msg.version));
        write(server_socket, &msg.content_size, sizeof(msg.content_size));
        write(server_socket, msg.content, message_len);
    }

    return NULL;
}
