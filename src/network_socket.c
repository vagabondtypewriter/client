#include "../include/network_socket.h"
#include "../include/gui.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define INPUT_WINDOW_HEIGHT 3
#define OUTPUT_WINDOW_HEIGHT (LINES - INPUT_WINDOW_HEIGHT - 1)
#define OUTPUT_WINDOW_WIDTH COLS
#define BACKSPACE 127
static WINDOW *input_window;     // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
static WINDOW *output_window;    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

static volatile int receive_thread_running = 1;    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

int client_create(uint16_t port, const char *ip)
{
    struct client_information client;
    client = socket_create(port, ip);
    if(socket_connect(client) == 1)
    {
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);

        output_window = newwin(OUTPUT_WINDOW_HEIGHT, OUTPUT_WINDOW_WIDTH, 0, 0);
        //        box(output_window, 0, 0);
        scrollok(output_window, TRUE);
        wrefresh(output_window);

        input_window = newwin(INPUT_WINDOW_HEIGHT, OUTPUT_WINDOW_WIDTH, LINES - INPUT_WINDOW_HEIGHT, 0);
        box(input_window, 0, 0);
        wrefresh(input_window);

        handle_connection(client.fd);
        wrefresh(input_window);
        wrefresh(output_window);
        refresh();
    }
    else
    {
        printf("Error while connecting to server!\n");
        return -1;
    }
    printf("End\n");
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

void *receive_messages(void *socket_fd)
{
    int server_socket = *((int *)socket_fd);

    while(receive_thread_running)
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
            break;
        }
        bytes_received = read(server_socket, &new_message.content_size, sizeof(new_message.content_size));
        if(bytes_received < 0)
        {
            break;
        }
        if(bytes_received == 0)
        {
            break;
        }
        new_message.content_size = ntohs(new_message.content_size);

        bytes_received = read(server_socket, new_message.content, new_message.content_size);
        if(bytes_received < 0)
        {
            break;
        }
        if(bytes_received == 0)
        {
            break;
        }
        new_message.content[bytes_received] = '\0';
        wscrl(output_window, 1);
        wmove(output_window, OUTPUT_WINDOW_HEIGHT - 2, 1);
        wprintw(output_window, "%s\n", new_message.content);
        wrefresh(output_window);
    }
    return NULL;
}

void *send_messages(void *socket_fd)
{
    int            server_socket = *((int *)socket_fd);
    struct message msg;
    keypad(input_window, TRUE);

    while(1)
    {
        int  i = 0;
        char input_buffer[BUFFER_SIZE];

        wmove(input_window, 1, 1);
        wprintw(input_window, "> ");

        wrefresh(input_window);

        wclrtoeol(input_window);
        wrefresh(input_window);

        while(1)
        {
            int ch;

            ch = wgetch(input_window);
            if(ch == '\n')
            {
                if(i == 0)
                {
                    continue;
                }
                input_buffer[i] = '\0';
                break;
            }
            if(ch == ERR)
            {
                continue;
            }
            if(ch == KEY_BACKSPACE || ch == BACKSPACE)
            {
                if(i > 0)
                {
                    i--;
                    waddch(input_window, '\b');
                    waddch(input_window, ' ');
                    waddch(input_window, '\b');
                }
            }
            else
            {
                if(i < BUFFER_SIZE - 1)
                {
                    input_buffer[i++] = (char)ch;
                    waddch(input_window, (chtype)ch);
                }
            }
        }

        strcpy(msg.content, input_buffer);

        if(strcmp(msg.content, QUIT_COMMAND) == 0)
        {
            wclear(input_window);
            wclear(output_window);
            wrefresh(input_window);
            wrefresh(output_window);
            close(server_socket);
            receive_thread_running = 0;
            printf("WHEE\n\n");
            return NULL;
        }

        msg.version      = 1;
        msg.content_size = (uint16_t)strlen(msg.content);

        if(write(server_socket, &msg.version, sizeof(msg.version)) < 0 || write(server_socket, &msg.content_size, sizeof(msg.content_size)) < 0 || write(server_socket, msg.content, msg.content_size) < 0)
        {
            perror("Error sending message to server");
            break;
        }
    }

    return NULL;
}
