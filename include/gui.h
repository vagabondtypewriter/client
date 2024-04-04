//
// Created by kiefer on 2/20/24.
//

#ifndef CLIENT_SERVER_CHAT_SERVICE_GUI_H
#define CLIENT_SERVER_CHAT_SERVICE_GUI_H
#define MESSAGE_BUFFER_SIZE 10
#define HEIGHT 30
#define WIDTH 100
#define BEGIN_Y 0
#define BEGIN_X 0
#define OUTPUT_HEIGHT 10

#include <ncurses.h>

typedef struct
{
    const char *ip_address;
    const char *port;    // if port = 0, quit
} ServerInfo;

struct message_buffer
{
    char messages[MESSAGE_BUFFER_SIZE][WIDTH];
    int  start_index;
    int  count;
};

int        gui_test(void);
ServerInfo gui_main(int numServers, const char *servers[], const char *ports[]);
int        gui_server_list(int numServers, const char *servers[], const char *ports[]);
void       display_output(WINDOW *output_win, const char *new_message);
void       display_input(WINDOW *input_win, const char *prompt);
int        gui_client_receive(void);
int        gui_client_send(void);

#endif    // CLIENT_SERVER_CHAT_SERVICE_GUI_H
