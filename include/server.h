#pragma once

extern "C" {
#include <netinet/in.h>
}

#include <string>

// class Server implements the complete localchat server. It uses two threads
// to manage clients: one waits for new clients to connect, and the other waits
// for messages and distribute them to the others when received via the epoll
// polling system.
class Server {
    private:
        // tcurrent amount of clients connected
        short  connections;

        // file descriptor that listens for new clients
        int    server_fd;

        // epoll file descriptor that waits for client actions
        int    epoll_fd;

        // struct associated with server_fd
        struct sockaddr_in server_addr;

        // funcion that manages the acceptance of new clients
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
