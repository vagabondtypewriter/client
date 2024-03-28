#include "../include/gui.h"
#include "../include/network_socket.h"
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define IP1 "10.0.0.236"
#define PORT1 "8080"
#define IP2 "1.0.0.1"
#define PORT2 "9090"
#define IP3 "1.1.0.1"
#define PORT3 "9190"

#define BASE10 10

int main(void)
{
    ServerInfo  server;
    char       *endptr;
    uint16_t    port;
    long        port_long;
    const char *serverIPs[]   = {IP1, IP2, IP3};
    const char *serverPorts[] = {PORT1, PORT2, PORT3};
    server                    = gui_main(3, serverIPs, serverPorts);
    port_long                 = strtol(server.port, &endptr, BASE10);
    // Parse port string to an integer

    // Check for conversion errors
    if((errno == ERANGE && (port_long == LONG_MAX || port_long == LONG_MIN)) || (errno != 0 && port_long == 0))
    {
        perror("strtol");
        return EXIT_FAILURE;
    }

    // Check for out-of-range values
    if(port_long < 0 || port_long > UINT16_MAX)
    {
        fprintf(stderr, "Port number out of range for uint16_t\n");
        return EXIT_FAILURE;
    }

    // Convert to uint16_t
    port = (uint16_t)port_long;

    // Check if client is successfully created/connected
    if(client_create(port, server.ip_address) == 1)
    {
        printf("Success\n");
    }
    else
    {
        printf("Failed to create client\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
