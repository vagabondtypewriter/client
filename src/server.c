// #include <arpa/inet.h>
// #include <netinet/in.h>
// #include <signal.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
//
// #define PORT 8080
// #define MAX_PENDING_CONNECTIONS 5
//// #define BUFFER_SIZE 1024
//
// static int server_fd;      // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
// static int new_socket;     // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
// static int running = 1;    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
//
// static void sigint_handler(int signum)
//{
//    running = signum;
//}
//
// int main(void)
//{
//    struct sockaddr_in address;
//    int                opt          = 1;
//    int                addrlen      = sizeof(address);
//    uint8_t            version      = 1;
//    const char        *message      = "Hello";
//    uint16_t           content_size = (uint16_t)strlen(message);
//    uint16_t           content_size_network;
//    // Register SIGINT handler
//    signal(SIGINT, sigint_handler);
//
//    // Creating socket file descriptor
//    server_fd = socket(AF_INET, SOCK_STREAM, 0);
//    if(server_fd == 0)
//    {
//        perror("socket failed");
//        exit(EXIT_FAILURE);
//    }
//
//    // Set socket options to reuse address and port
////    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
////    {
////        perror("setsockopt");
////        exit(EXIT_FAILURE);
////    }
//
//    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
//            {
//                perror("setsockopt");
//                exit(EXIT_FAILURE);
//            }
//
//    address.sin_family      = AF_INET;
//    address.sin_addr.s_addr = INADDR_ANY;
//    address.sin_port        = htons(PORT);
//
//    // Bind the socket to the specified port
//    if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
//    {
//        perror("bind failed");
//        exit(EXIT_FAILURE);
//    }
//
//    // Listen for incoming connections
//    if(listen(server_fd, MAX_PENDING_CONNECTIONS) < 0)
//    {
//        perror("listen");
//        exit(EXIT_FAILURE);
//    }
//
//    printf("Server listening on port %d\n", PORT);
//
//    // Accept incoming connections
//    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
//    if(new_socket < 0)
//    {
//        perror("accept");
//        exit(EXIT_FAILURE);
//    }
//
//    // Send version to client
//    send(new_socket, &version, sizeof(version), 0);
//
//    // Convert content size to network byte order (big-endian)
//    content_size_network = htons(content_size);
//
//    // Send content size to client
//    send(new_socket, &content_size_network, sizeof(content_size_network), 0);
//
//    // Send message to the client
//    send(new_socket, message, strlen(message), 0);
//    printf("Message sent to client: %s\n", message);
//
//    // Close the server after sending the message
//    close(new_socket);
//    close(server_fd);
//
//    return 0;
//}
//
//
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8079
#define MAX_PENDING_CONNECTIONS 5
#define BUFFER_SIZE 1024

static int server_fd;      // NOLINT(cppcoreguidelines-avoid-non-const-global-variables
static int running = 1;    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

void *client_handler(void *socket_fd)
{
    int client_socket = *((int *)socket_fd);
    free(socket_fd);    // Clean up allocated memory

    char buffer[BUFFER_SIZE];

    while(1)
    {
        ssize_t bytes_read = read(client_socket, buffer, BUFFER_SIZE - 1);

        if(bytes_read > 0)
        {
            buffer[bytes_read] = '\0';    // Null-terminate the string
            printf("Message from client: %s\n", buffer);
            // Echo the message back to the client, casting bytes_read to size_t
            send(client_socket, buffer, (size_t)bytes_read, 0);
        }
        else if(bytes_read == 0)
        {
            printf("Client disconnected\n");
            break;    // Break the loop if client has disconnected
        }
        else
        {
            perror("recv error");
            break;    // Break the loop on error
        }

        memset(buffer, 0, BUFFER_SIZE);    // Clear the buffer for the next message
    }

    close(client_socket);
    return NULL;
}

static void sigint_handler(int signum)
{
    (void)signum;
    running = 0;
    close(server_fd);    // Close the server socket to break out of the accept call
}

int main(void)
{
    struct sockaddr_in address;
    int                opt     = 1;
    int                addrlen = sizeof(address);

    signal(SIGINT, sigint_handler);

#ifndef SOCK_CLOEXEC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-macros"
    #define SOCK_CLOEXEC 0
    #pragma GCC diagnostic pop
#endif
    server_fd = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
    if(server_fd == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port        = htons(PORT);

    if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if(listen(server_fd, MAX_PENDING_CONNECTIONS) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while(1)
    {
        int *client_socket = malloc(sizeof(int));    // Allocate memory for the client socket
        *client_socket     = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);

        if(*client_socket < 0)
        {
            if(running)
            {    // Check if the server is stopping
                perror("accept");
            }
            free(client_socket);    // Clean up allocated memory
            break;
        }

        pthread_t thread_id;
        if(pthread_create(&thread_id, NULL, client_handler, (void *)client_socket) != 0)
        {
            perror("could not create thread");
            free(client_socket);    // Clean up allocated memory
            continue;
        }

        pthread_detach(thread_id);    // Don't wait for the thread; let it clean up after itself
    }

    return 0;
}
