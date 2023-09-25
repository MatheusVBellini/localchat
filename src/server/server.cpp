#include "server.h"

#include "utils.h"

#include <thread>

extern "C" {
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
}

Server::Server(std::string ip, int port, int maxWaiters) {
    this->connections = 0;

    // create the server socket
    this->server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (this->server_fd < 0) {
        error("Failed to create socket");
    }

    // add server properties
    this->server_addr.sin_family = AF_INET;
    this->server_addr.sin_addr.s_addr = INADDR_ANY;
    this->server_addr.sin_port = htons(port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    inet_pton(AF_INET, ip.c_str(), &this->server_addr.sin_addr);

    int bind_result = bind(this->server_fd,
                           (struct sockaddr *) &this->server_addr,
                           sizeof(this->server_addr));

    if (bind_result < 0) {
        error("Failed to bind socket");
    }

    int listen_result = listen(this->server_fd, maxWaiters);
    if (listen_result < 0) {
        error("Failed to listen to socket");
    }

    // create epoll file descriptor for client handling
    this->epoll_fd = epoll_create1(0);
    if (this->epoll_fd < 0) {
      error("epoll creation failed");
    }
}

Server::~Server(void) {
    close(this->server_fd);

    close(this->epoll_fd);
}

void acceptNewClients(Server* server) {
  struct epoll_event event;
  event.events = EPOLLIN;

  while (true) {
    struct sockaddr client_addr;
    int client_fd;
    socklen_t client_size = sizeof(client_addr);

    // block and wait for new clients
    client_fd = accept(server->server_fd, &client_addr, &client_size);
    if (client_fd < 0) {
      break;
    }

    // set the new client socket to non-blocking mode
    int flags = fcntl(client_fd, F_GETFL, 0);
    if (flags == -1) {
        error("fcntl get failed");
    }
    int non_block_result = fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
    if (non_block_result == -1) {
        error("fcntl set failed");
    }

    server->connections++;

    // and add the client file descriptor to the epoll list
    event.data.fd = client_fd;
    int ret = epoll_ctl(server->epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
    if(ret < 0) {
      break;
    }
  }

  error("server accept new clients failed: " << strerror(errno));
}

void Server::run(void) {
  // create the new client listener
  std::thread server_reader(acceptNewClients, this);

  struct epoll_event event;
  event.events = EPOLLIN;

  while(true) {
    // wait for client events
    epoll_wait(this->epoll_fd, &event, 1, -1);

    char buffer[4096];
    int bytes_read;

    // read and process the client data
    while ((bytes_read = read(event.data.fd, buffer, 4095)) > 0) {
      buffer[bytes_read] = '\0';
      std::cout << buffer << std::endl;
    }

    // EOF received
    if(bytes_read == 0) {
      error("not implemented");
    }

    // the file descriptor would block, meaning we should wait for new data 
    // from clients
    if (errno == EWOULDBLOCK) {
      continue;
    }

    // another error: panic
    error("client read: " << strerror(errno));
  }
}

int Server::getConnections(void) const {
    return this->connections;
}
