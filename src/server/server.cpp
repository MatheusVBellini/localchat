#include "../../include/server.h"

Server::Server(std::string ip, int port, short maxConnections) {
    this->connections = 0;
    this->maxConnections = maxConnections;

    // Socket configuration
    this->socket_fd = socket(AF_INET, SOCK_STREAM, 0); // AF_INET = IPv4, SOCK_STREAM = TCP, 0 = IP
    if (this->socket_fd < 0) {
        error("Failed to create socket");
    }

    // Set socket to non-blocking mode
    int flags = fcntl(this->socket_fd, F_GETFL, 0);
    if (flags == -1) {
        error("fcntl get failed");
    }
    int non_block_result = fcntl(this->socket_fd, F_SETFL, flags | O_NONBLOCK);
    if (non_block_result == -1) {
        error("fcntl set failed");
    }

    this->server_addr.sin_family = AF_INET;
    this->server_addr.sin_addr.s_addr = INADDR_ANY;
    this->server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &this->server_addr.sin_addr); // Convert IPv4 and IPv6 addresses from text to binary form

    int bind_result = bind(this->socket_fd, (struct sockaddr *) &this->server_addr, sizeof(this->server_addr));
    if (bind_result < 0) {
        error("Failed to bind socket");
    }

    int listen_result = listen(this->socket_fd, this->maxConnections);
    if (listen_result < 0) {
        error("Failed to listen to socket");
    }

}

Server::~Server() {
    close(this->socket_fd);
}

void Server::checkConnections() {
    struct sockaddr client_addr;
    int client_fd = accept(this->socket_fd, &client_addr, NULL);
    if (client_fd >= 0) {
        this->connections++;
    }
}

int Server::getConnections() const {
    return this->connections;
}
