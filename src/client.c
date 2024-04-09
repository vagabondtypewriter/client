#include "../include/gui.h"
#include "../include/network_socket.h"
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define IP1 "192.168.0.165"
#define IP1 "192.168.0.165"
#define PORT1 "8080"
#define IP2 "192.168.0.241"
#define PORT2 "8036"
#define IP3 "192.168.0.247"
#define PORT3 "8081"
#define TESTIP "192.168.0.84"
#define TESTPORT "8080"
#define NUMSERVERS 4

#define BASE10 10

int main(void)
{
    char       *endptr;
    uint16_t    port;
    long        port_long;
    int         running       = 1;
    const char *serverIPs[]   = {IP1, IP2, IP3, TESTIP};
    const char *serverPorts[] = {PORT1, PORT2, PORT3, TESTPORT};

    while(running == 1)
    {
        ServerInfo server;
        server = gui_main(NUMSERVERS, serverIPs, serverPorts);
        if(strcmp(server.port, "quit") == 0)
        {
            running = 0;
            continue;
        }
        port_long = strtol(server.port, &endptr, BASE10);

        if((errno == ERANGE && (port_long == LONG_MAX || port_long == LONG_MIN)) || (errno != 0 && port_long == 0))
        {
            perror("strtol");
            return EXIT_FAILURE;
        }

        if(port_long < 0 || port_long > UINT16_MAX)
        {
            fprintf(stderr, "Port number out of range for uint16_t\n");
            return EXIT_FAILURE;
        }

        port = (uint16_t)port_long;

        if(client_create(port, server.ip_address) == 1)
        {
            printf("Success\n");
        }
        else
        {
            printf("Failed to create client\n");
        }
    }
    return EXIT_SUCCESS;
}
