#include "../include/gui.h"
#include "../include/network_socket.h"
#include <asm-generic/errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define USAGE "Usage: -t type -i ip -p port\n"
#define BASE 10

// struct to hold the values for
struct arguments
{
    char *ip;
    char *port;
};

// checks that arguments are expected
struct arguments parse_args(int argc, char *argv[]);
// starts server or connects client to server
int handle_args(struct arguments args);

int main(int argc, char *argv[])
{
    // parse arguments
    if(handle_args(parse_args(argc, argv)))
    {
        // parsed, handled within handle_args
        // handles fail state
        // todo START GUI
        gui_test();
    }
    // exit back to menu??
    return EXIT_SUCCESS;
}

struct arguments parse_args(int argc, char *argv[])
{
    int              opt;
    struct arguments newArgs;

    // Set default values
    newArgs.ip   = NULL;
    newArgs.port = NULL;

    while((opt = getopt(argc, argv, "i:p:")) != -1)
    {
        switch(opt)
        {
            case 'i':
                newArgs.ip = optarg;
                break;
            case 'p':
                newArgs.port = optarg;
                break;
            default:
                fprintf(stderr, USAGE);
                exit(EXIT_FAILURE);
        }
    }

    // Check if required options are provided
    if(newArgs.ip == NULL || newArgs.port == NULL)
    {
        fprintf(stderr, USAGE);
        exit(EXIT_FAILURE);
    }

    return newArgs;
}

int handle_args(struct arguments passedArgs)
{
    uint16_t confirmed_port;
    char    *endptr;
    long     tmp_port;
    // convert using strtol
    tmp_port = strtol(passedArgs.port, &endptr, BASE);

    // Check for invalid input
    if(endptr == passedArgs.port)
    {
        fprintf(stderr, "No digits were found\n");
        return EXIT_FAILURE;
    }

    // Check for out-of-range values
    if(tmp_port < 0 || tmp_port > UINT16_MAX)
    {
        fprintf(stderr, "Value out of range for uint16_t\n");
        return EXIT_FAILURE;
    }

    // Convert the long integer to uint16_t
    confirmed_port = (uint16_t)tmp_port;
    printf("confirmed port: %i\n", confirmed_port);
    printf("%s\n", passedArgs.ip);
    // assuming args were successfully parsed:
    // --> start client with the correct args
    client_create(confirmed_port, passedArgs.ip);
    // return
    return 1;
}
