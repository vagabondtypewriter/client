// FILE TO HANDLE GUI COMPONENTS
// EXAMPLE GUI TO GO OVER TOGETHER IN THE DRIVE
#include "../include/gui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// macros for dims
#define HEIGHT 30
#define WIDTH 100
#define BEGIN_Y 0
#define BEGIN_X 0

int gui_test(void)
{
    printf("From gui.c\n");
    return 1;
}

int gui_main(void)
{
    WINDOW     *menu_win;
    const char *options[]      = {"Join Server", "Choose Name", "Options", "Quit"};
    int         current_option = 0;
    initscr();
    cbreak();
    noecho();
    //    keypad(stdscr, TRUE);    // Enable special keys

    menu_win = newwin(HEIGHT, WIDTH, BEGIN_Y, BEGIN_X);
    wclear(menu_win);
    box(menu_win, 0, 0);
    while(1)
    {
        int ch;
        // Print menu items
        box(menu_win, 0, 0);
        for(int i = 0; i < 4; i++)
        {
            if(i == current_option)
            {
                wattron(menu_win, A_STANDOUT);
            }
            mvwprintw(menu_win, i + 1, 2, "%s", options[i]);
            if(i == current_option)
            {
                wattroff(menu_win, A_STANDOUT);
            }
        }

        wrefresh(menu_win);
        ch = wgetch(menu_win);
        wclear(menu_win);
        switch(ch)
        {
            case 'w':
                current_option = (current_option - 1 + 4) % 4;
                refresh();
                break;
            case 's':
                current_option = (current_option + 1) % 4;
                refresh();
                break;
            case 'q':
                delwin(menu_win);
                endwin();
                return 0;
            case 'e':
                // Print the selected option
                mvwprintw(menu_win, HEIGHT / 2, (WIDTH - strlen("Selected ")) / 2, "Selected ");
                wattron(menu_win, A_BOLD);    // Enable bold text
                wprintw(menu_win, "%s", options[current_option]);
                wattroff(menu_win, A_BOLD);    // Disable bold text
                wrefresh(menu_win);
                break;
            default:
                break;
        }
    }
}
