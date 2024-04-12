#include "../include/gui.h"
#include "../include/network_socket.h"
// #include <asm-generic/errno.h>
#include <getopt.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define USAGE "Usage: -t type -i ip -p port\n"
#define BASE 10

/**
 * TODO
 * HANDLE SEND/REC MSGS
 * START GUI
 * dup2 to redir input from the server to the GUI(?) << may be unecessary
 * Display messages on the gui
 */

// struct to hold the values for
struct arguments
{
    char *ip;
    char *port;
};

// checks that arguments are expected
struct arguments *parse_args(int argc, char *argv[]);
// starts server or connects client to server
int handle_args(struct arguments *args);

int main(int argc, char *argv[])
{
    // parse arguments
    if(handle_args(parse_args(argc, argv)))
    {
        // only runs when client is created and connected: i.e:
        //        gui_test();
    }
    // exit back to menu??
    return EXIT_SUCCESS;
}

struct arguments *parse_args(int argc, char *argv[])
{
    int               opt;
    struct arguments *newArgs;
    newArgs = (struct arguments *)malloc(sizeof(struct arguments));
    if(newArgs == NULL)
    {
        perror("Failed to allocate memory for newArgs");
        exit(EXIT_FAILURE);
    }

    // Initialize members of newArgs
    newArgs->ip   = NULL;
    newArgs->port = NULL;

    while((opt = getopt(argc, argv, "i:p:")) != -1)
    {
        switch(opt)
        {
            case 'i':
                newArgs->ip = optarg;
                break;
            case 'p':
                newArgs->port = optarg;
                break;
            default:
                fprintf(stderr, USAGE);
                free(newArgs);    // Free memory allocated for newArgs
                exit(EXIT_FAILURE);
        }
    }

    // Check if ip and port are provided
    if(newArgs->ip == NULL)
    {
        fprintf(stderr, USAGE);
        free(newArgs->port);
        free(newArgs->ip);
        free(newArgs);    // Free memory allocated for newArgs
        exit(EXIT_FAILURE);
    }
    if(newArgs->port == NULL)
    {
        fprintf(stderr, USAGE);
        free(newArgs->port);
        free(newArgs->ip);
        free(newArgs);    // Free memory allocated for newArgs
        exit(EXIT_FAILURE);
    }

    return newArgs;
}

int handle_args(struct arguments *passedArgs)
{
    uint16_t confirmed_port;
    char    *endptr;
    long     tmp_port;

    tmp_port = strtol(passedArgs->port, &endptr, BASE);

    if(endptr == passedArgs->port)
    {
        perror("No digits were found\n");
        free(passedArgs);
        return -1;
    }

    // Check for out-of-range values
    if(tmp_port < 0 || tmp_port > UINT16_MAX)
    {
        perror("Value out of range for uint16_t\n");
        free(passedArgs);

        return -1;
    }

    // convert to uint16_t
    confirmed_port = (uint16_t)tmp_port;
    printf("confirmed port: %i\n", confirmed_port);
    printf("%s\n", passedArgs->ip);

    // checks to see if the client is successfully created/connected
    if(client_create(confirmed_port, passedArgs->ip) == 1)
    {
        free(passedArgs);
        return 1;
    }
    free(passedArgs);
    return -1;
}
