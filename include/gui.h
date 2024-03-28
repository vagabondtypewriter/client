//
// Created by kiefer on 2/20/24.
//

#ifndef CLIENT_SERVER_CHAT_SERVICE_GUI_H
#define CLIENT_SERVER_CHAT_SERVICE_GUI_H

typedef struct
{
    const char *ip_address;
    const char *port;
} ServerInfo;

int        gui_test(void);
ServerInfo gui_main(int numServers, const char *servers[], const char *ports[]);
int        gui_server_list(int numServers, const char *servers[], const char *ports[]);

#endif    // CLIENT_SERVER_CHAT_SERVICE_GUI_H
