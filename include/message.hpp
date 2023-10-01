#pragma once

#include <ctime>
#include <map>
#include <string>

#define MESSAGE_SIZE (200 + 10 + 4 + 2 + 2 + 2 + 2 + 2 + 6)

class Message {
private:
  std::string username;
  std::time_t timestamp;
  std::string message_content;

  static std::map<int, std::pair<int, char*>>
      messages_in_progress_map;

public:
  Message(std::string content, std::string username,
          std::time_t timestamp = std::time(nullptr));

  void send(int socket_fd);

  static Message *recv(int socket_fd, bool& eof);

  std::string getContent(void);
  std::time_t getTimestamp(void);
  std::string getUsername(void);
};
