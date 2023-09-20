#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

class Socket {
    private:
        int socket_fd;
        struct sockaddr_in server_addr;
        struct sockaddr_in client_addr;
        socklen_t client_addr_len;
        int port;
        int backlog;
        int max_connections;
        int max_buffer_size;
        char *buffer;

    public:
        Socket(int port, int backlog, int max_connections, int max_buffer_size);
        ~Socket();
        void run();
};