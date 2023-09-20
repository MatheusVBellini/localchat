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
#include "utils.h"

class Server {
    private:
        short  connections;
        short  maxConnections;
        int    socket_fd;
        struct sockaddr_in server_addr;

    public:

        // Initiate basic specifications of the server
        Server(std::string ip = "127.0.0.1", int port = 8080, short maxConnections = 5);

        // Close the socket
        ~Server();
        
        // Check any new connection attempts and handle it
        void checkConnections();

        // Return the total number of clients connected to host
        int getConnections() const;

};
