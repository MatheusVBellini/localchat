#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <unordered_map>
#include <list>
#include <cstring>
#include "utils.h"
#include "message.hpp"

class Client {
    private:
        bool is_connected;
        int    client_socket;
        struct sockaddr_in server_addr;
        std::string username;

        void setUsername(std::string usr);
        int getClientSocket();
    public:
        Client(std::string username, std::string ip, int port);
        ~Client();
        void run();

};
