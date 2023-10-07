#include "client.hpp"

Client::Client(std::string username, std::string server_ip, int server_port) {
  this->setUsername(username);
  this->is_connected = false;

  // Socket configuration
  this->client_socket = socket(AF_INET, SOCK_STREAM,
                               0); // AF_INET = IPv4, SOCK_STREAM = TCP, 0 = IP
  if (this->client_socket < 0) {
    error("Failed to create socket from client");
  }

  this->server_addr.sin_family = AF_INET;
  this->server_addr.sin_port = htons(server_port);

  // Convert IPv4 and IPv6 addresses from text to binary form
  inet_pton(AF_INET, server_ip.c_str(), &this->server_addr.sin_addr);

  if (connect(client_socket, (struct sockaddr *)&server_addr,
              sizeof(server_addr)) == -1) {
    error("could not connect client to server");
  }

  debug("client connected");
  this->is_connected = true;
}

Client::~Client() { close(client_socket); }

int Client::getClientSocket() { return this->client_socket; }

void Client::setUsername(std::string usr) {
  // this makes the usr always the same size (10 char  '\0')
  if (usr.size() > 10) {
    usr = usr.substr(0, 10);
  } else if (usr.size() < 10) {
    usr = usr.append(10 - usr.size(), '\0');
  }
  this->username = usr;
}

void Client::sendMessage(std::string content) {
  auto m = Message(content, username);

  m.send(client_socket);
}

Message* Client::getMessage(void) {
  bool eof;
  auto *m = Message::recv(client_socket, eof);
  if (eof == true || m == nullptr) {
    error("client got eof or nullptr");
  }

  return m;
}
