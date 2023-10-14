#include "Server.h"

#include "Message.h"
#include "utils.h"

extern "C" {
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>
}

Server::Server(std::string ip, int port, int maxWaiters) {
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

  int bind_result = bind(this->server_fd, (struct sockaddr *)&this->server_addr,
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

  struct epoll_event event;
  event.events = EPOLLIN;
  event.data.fd = this->server_fd;

  int ret = epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, this->server_fd, &event);
  if (ret < 0) {
    error("server epoll self setup failed: " << strerror(errno));
  }
}

Server::~Server(void) {
  close(this->server_fd);

  close(this->epoll_fd);
}

void Server::acceptNewClient(void) {
  struct epoll_event event;
  event.events = EPOLLIN;

  struct sockaddr client_addr;
  int client_fd;
  socklen_t client_size = sizeof(client_addr);

  // block and wait for new clients
  client_fd = accept(this->server_fd, &client_addr, &client_size);
  if (client_fd < 0) {
    error("server accept new client failed: " << strerror(errno));
  }

  debug("got new client with fd " << client_fd);

  // set the new client socket to non-blocking mode
  int flags = fcntl(client_fd, F_GETFL, 0);
  if (flags == -1) {
    error("fcntl get failed");
  }
  int non_block_result = fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
  if (non_block_result == -1) {
    error("fcntl set failed");
  }

  clients.insert(client_fd);

  // and add the client file descriptor to the epoll list
  event.data.fd = client_fd;
  int ret = epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
  if (ret < 0) {
    error("server accept new clients failed: " << strerror(errno));
  }
}

void Server::closeClient(struct epoll_event event) {
  debug("closing connection with client " << event.data.fd);

  clients.erase(event.data.fd);

  close(event.data.fd);
  epoll_ctl(epoll_fd, EPOLL_CTL_DEL, event.data.fd, &event);
}

void Server::run(void) {
  info("starting main server loop");

  struct epoll_event event;
  event.events = EPOLLIN;

  while (true) {
    // wait for client events
    epoll_wait(this->epoll_fd, &event, 1, -1);
    if (event.data.fd == this->server_fd) {
      acceptNewClient();
      continue;
    }

    debug("got epoll event from client " << event.data.fd);

    // read and process the client data
    bool eof;
    Message *m = Message::recv(event.data.fd, eof);
    if (eof) {
      closeClient(event);
    }
    if (m == nullptr) {
      continue;
    }

    for (auto c : clients) {
      while (m->send(c) == false)
        ;
    }

    delete m;
  }
}
