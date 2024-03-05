#include "../include/gui.h"
#include "../include/network_socket.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char *argv[])
{
    int res;
    int res2;
    printf("WHEEE\n");
    res  = client_create();
    res2 = gui_test();
    printf("Res: %i\n", res);
    printf("Res2: %i\n", res2);
    printf("Argc: %i, %s\n", argc, argv[0]);
    return EXIT_SUCCESS;
}
