#include "../include/gui.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// #define INPUT_HEIGHT 5

int gui_test(void)
{
    printf("From gui.c\n");
    return 1;
}

ServerInfo gui_main(int numServers, const char *servers[], const char *ports[])
{
    WINDOW     *menu_win;
    ServerInfo  selected_server;
    const char *options[]      = {"Join Server", "Quit"};
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
        int res;
        box(menu_win, 0, 0);
        for(int i = 0; i < 2; i++)
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
            case 'w':
                current_option = (current_option - 1 + 2) % 2;
                break;
            case 's':
                current_option = (current_option + 1) % 2;
                break;
            case 'q':
                delwin(menu_win);
                endwin();
                exit(EXIT_SUCCESS);
            case '\n':    // enter
                switch(current_option)
                {
                    case 0:
                        wclear(menu_win);
                        wrefresh(menu_win);
                        endwin();
                        res = gui_server_list(numServers, servers, ports);
                        if(res != -1)
                        {
                            selected_server.ip_address = servers[res];
                            selected_server.port       = ports[res];
                            werase(menu_win);
                            refresh();
                            endwin();
                            return selected_server;
                        }
                        initscr();
                        cbreak();
                        noecho();
                        keypad(stdscr, TRUE);
                        menu_win = newwin(HEIGHT, WIDTH, BEGIN_Y, BEGIN_X);
                        wclear(menu_win);
                        box(menu_win, 0, 0);
                        break;
                    case 1:
                        // quit logic
                        selected_server.ip_address = servers[0];
                        selected_server.port       = "quit";
                        werase(menu_win);
                        refresh();
                        endwin();
                        return selected_server;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
}

int gui_server_list(int numServers, const char *servers[], const char *ports[])
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
        for(int i = 0; i < numServers + 1; i++)
        {
            if(i == current_option)
            {
                wattron(menu_win, A_STANDOUT);
            }
            if(i < numServers)
            {
                mvwprintw(menu_win, i + 1, 2, "Server %d: %s:%s", i + 1, servers[i], ports[i]);
            }
            else
            {
                mvwprintw(menu_win, i + 1, 2, "%s", back_option);
            }
            if(i == current_option)
            {
                wattroff(menu_win, A_STANDOUT);
            }
        }

        wrefresh(menu_win);
        ch = wgetch(menu_win);
        wclear(menu_win);
        mvwprintw(menu_win, HEIGHT - 1, 2, "Pressed key: %d", ch);
        switch(ch)
        {
            case 'w':    // w
                current_option = (current_option - 1 + numServers + 1) % (numServers + 1);
                break;
            case 's':    // s
                current_option = (current_option + 1) % (numServers + 1);
                break;
            case '\n':    // enter
                if(current_option < numServers)
                {
                    delwin(menu_win);
                    endwin();
                    return current_option;
                }
                else if(current_option == numServers)
                {
                    // back
                    delwin(menu_win);
                    endwin();
                    return -1;
                }
                break;
            default:
                break;
        }
    }
}

// gui client receive --> returns (?)
// displays received information
// keeps buffer for messages (?)
// updates position for message positions (?)
int gui_client_receive(void)
{
    int     running = 0;
    WINDOW *display_menu_win;
    WINDOW *input_menu_win;
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    display_menu_win = newwin(HEIGHT, WIDTH, BEGIN_Y, BEGIN_X);
    input_menu_win   = newwin(OUTPUT_HEIGHT, WIDTH, HEIGHT, BEGIN_X);
    wclear(display_menu_win);
    wclear(input_menu_win);
    box(display_menu_win, 0, 0);
    box(input_menu_win, 0, 0);
    wrefresh(display_menu_win);
    wrefresh(input_menu_win);
    while(running < MESSAGE_BUFFER_SIZE)
    {
        box(display_menu_win, 0, 0);
        box(input_menu_win, 0, 0);
        running++;
        sleep(MESSAGE_BUFFER_SIZE);
    }
    return 1;
}

// gui client send --> returns (?)
// keeps the current types characters until '\n' is sent
// how can I do this ??
// keep fgets, live update?
int gui_client_send(void)
{
    int     running = 0;
    WINDOW *menu_win;

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    menu_win = newwin(OUTPUT_HEIGHT, WIDTH, HEIGHT, BEGIN_X);
    wclear(menu_win);
    box(menu_win, 0, 0);
    wrefresh(menu_win);
    while(running < MESSAGE_BUFFER_SIZE)
    {
        box(menu_win, 0, 0);
        running++;
        sleep(MESSAGE_BUFFER_SIZE);
    }
    return 1;
}

// todo update the positioning of each new message, keep buffer (?) to hold old messages so they get printed to scrn?
void display_output(WINDOW *output_win, const char *message)
{
    werase(output_win);
    mvwprintw(output_win, 1, 1, "%s", message);
    wrefresh(output_win);
}

// todo change (? or it might be ok? depends if I want to keep a buffer or not)
void display_input(WINDOW *input_win, const char *prompt)
{
    werase(input_win);
    mvwprintw(input_win, 1, 1, "%s", prompt);
    wrefresh(input_win);
}
