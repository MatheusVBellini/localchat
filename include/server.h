#pragma once

extern "C" {
#include <netinet/in.h>
}

#include <string>

class Server {
    private:
        short  connections;
        int    server_fd;
        int    epoll_fd;
        struct sockaddr_in server_addr;

        friend void acceptNewClients(Server* server);

    public:

        // Initiate basic specifications of the server
        Server(std::string ip = "127.0.0.1", int port = 8080, int maxWaiters = 5);

        // Close the socket
        ~Server(void);

        // main server loop: accepts new connections
        void run(void);

        // Return the total number of clients connected to host
        int getConnections(void) const;
};
