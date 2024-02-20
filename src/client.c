#include "../include/network_socket.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int res;
    printf("WHEEE\n");
    res = client_create();
    printf("Res: %i", res);
    return EXIT_SUCCESS;
}
