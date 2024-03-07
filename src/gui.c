// FILE TO HANDLE GUI COMPONENTS
// EXAMPLE GUI TO GO OVER TOGETHER IN THE DRIVE
#include "../include/gui.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

int gui_test(void)
{
    printf("From gui.c\n");
    return 1;
}

int gui_start(void)
{
    initscr();

    // Print something to the screen
    printw("Hello, ncurses!");

    // Refresh the screen to show the changes
    refresh();

    // Wait for a key press
    getch();

    // End ncurses mode
    endwin();
    return 1;
}
