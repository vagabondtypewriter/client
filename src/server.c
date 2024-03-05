#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
#define MAX_PENDING_CONNECTIONS 5
#define BUFFER_SIZE 1024

static int server_fd;      // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
static int new_socket;     // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
static int running = 1;    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

static void sigint_handler(int signum)
{
    running = signum;
}

int main(void)
{
    struct sockaddr_in address;
    int                opt                 = 1;
    int                addrlen             = sizeof(address);
    char               buffer[BUFFER_SIZE] = {0};
    const char        *hello               = "Hello from server";

    // Register SIGINT handler
    signal(SIGINT, sigint_handler);

    // Creating socket file descriptor
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options to reuse address and port
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port        = htons(PORT);

    // Bind the socket to the specified port
    if(bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if(listen(server_fd, MAX_PENDING_CONNECTIONS) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    // Accept incoming connections
    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if(new_socket < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // Send a message to the client
    send(new_socket, hello, strlen(hello), 0);
    printf("Hello message sent to client\n");

    // Continuous loop for reading input and sending it to the client
    printf("Enter text to send to the client (Ctrl+C to exit):\n");
    while(running == 1)
    {
        // Read input from the keyboard
        fgets(buffer, BUFFER_SIZE, stdin);

        // Send input to the client
        send(new_socket, buffer, strlen(buffer), 0);
    }

    // Cleanup before exiting
    close(new_socket);
    close(server_fd);

    return 0;
}
