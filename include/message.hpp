#pragma once

#include <ctime>
#include <string>

class Message {
private:
  std::string username;
  std::time_t timestamp;
  std::string message_content;

public:
  Message(std::string content, std::string username,
          std::time_t timestamp = std::time(nullptr));

  void send(int socket_fd);

  static Message recv(int socket_fd);

  std::string getContent(void);
  std::time_t getTimestamp(void);
  std::string getUsername(void);
};
