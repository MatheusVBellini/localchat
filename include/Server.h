#pragma once

extern "C" {
#include <netinet/in.h>
}

#include <string>
#include <set>

// class Server implements the complete localchat server. It uses two threads
// to manage clients: one waits for new clients to connect, and the other waits
// for messages and distribute them to the others when received via the epoll
// polling system.
class Server {
private:
  // file descriptor that listens for new clients
  int server_fd;

  // epoll file descriptor that waits for client actions
  int epoll_fd;

  // struct associated with server_fd
  struct sockaddr_in server_addr;

  std::set<int> clients;

  // acceptNewClient accepts a single client, should be only called when
  // server_fd is ready for reading
  void acceptNewClient(void);

  // closeClient deletes a client from the epoll list and closes the connection
  // with it.
  void closeClient(struct epoll_event e);

public:
  // Initiate basic specifications of the server
  Server(std::string ip = "127.0.0.1", int port = 8080, int maxWaiters = 5);

  // Close the socket
  ~Server(void);

  // main server loop: accepts new connections
  void run(void);
};
