#include "../include/gui.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEIGHT 30
#define WIDTH 100
#define BEGIN_Y 0
#define BEGIN_X 0

int gui_test(void)
{
    printf("From gui.c\n");
    return 1;
}

ServerInfo gui_main(int numServers, const char *servers[], const char *ports[])
{
    WINDOW *menu_win;
    //    ServerInfo  selected_server;
    const char *options[]      = {"Join Server", "Choose Name", "Options", "Quit"};
    int         current_option = 0;
    //    int         selected_server_index = -1;    // Initialize to -1 as an invalid index

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    menu_win = newwin(HEIGHT, WIDTH, BEGIN_Y, BEGIN_X);
    wclear(menu_win);
    box(menu_win, 0, 0);

    while(1)
    {
        int ch;
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
        mvwprintw(menu_win, HEIGHT - 1, 2, "Pressed key: %d", ch);    // Print pressed key
        switch(ch)
        {
            case 'w':    // w
                current_option = (current_option - 1 + 4) % 4;
                break;
            case 's':    // s
                current_option = (current_option + 1) % 4;
                break;
            case 'q':
                delwin(menu_win);
                endwin();    // Add this line to clean up ncurses
                exit(EXIT_SUCCESS);
            case '\n':    // Enter key
                switch(current_option)
                {
                    case 0:    // join
                        wclear(menu_win);
                        wrefresh(menu_win);
                        endwin();
                        return (gui_server_list(numServers, servers, ports));
                    case 1:                                                 // choosename
                        mvwprintw(menu_win, HEIGHT - 1, 2, "Nickname?");    // Print pressed key
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
}

ServerInfo gui_server_list(int numServers, const char *servers[], const char *ports[])
{
    WINDOW     *menu_win;
    const char *back_option    = "Back";
    int         current_option = 0;

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    menu_win = newwin(HEIGHT, WIDTH, BEGIN_Y, BEGIN_X);
    wclear(menu_win);
    box(menu_win, 0, 0);

    while(1)
    {
        int ch;
        box(menu_win, 0, 0);
        for(int i = 0; i < numServers; i++)
        {
            if(i == current_option)
            {
                wattron(menu_win, A_STANDOUT);
            }
            mvwprintw(menu_win, i + 1, 2, "Server %d: %s:%s", i + 1, servers[i], ports[i]);
            if(i == current_option)
            {
                wattroff(menu_win, A_STANDOUT);
            }
        }
        mvwprintw(menu_win, numServers + 2, 2, "%s", back_option);

        wrefresh(menu_win);
        ch = wgetch(menu_win);
        wclear(menu_win);
        mvwprintw(menu_win, HEIGHT - 1, 2, "Pressed key: %d", ch);    // Print pressed key
        switch(ch)
        {
            case 'w':    // w
                current_option = (current_option - 1 + numServers + 1) % (numServers);
                break;
            case 's':    // s
                current_option = (current_option + 1) % (numServers);
                break;
            case '\n':    // Enter key
                if(current_option < numServers)
                {
                    ServerInfo selected_server;
                    selected_server.ip_address = servers[current_option];
                    selected_server.port       = ports[current_option];
                    delwin(menu_win);
                    endwin();
                    return selected_server;
                }
                else if(current_option == numServers)
                {
                    delwin(menu_win);
                    endwin();
                    exit(EXIT_SUCCESS);
                }
                break;
            default:
                break;
        }
    }
}
