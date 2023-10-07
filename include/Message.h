#pragma once

#include <ctime>
#include <map>
#include <string>

#define MESSAGE_SIZE (200 + 10 + 8)

// class Message is the communication unit of localchat, containing a username
// that sent the message, the contents, and a timestamp.
class Message {
private:
  std::string username;
  std::time_t timestamp;
  std::string message_content;

  std::map<int, int> sending_positions;

  // an internal map to manage receiving of new messages partially, due to the
  // nonblocking possibility of sockets.
  static std::map<int, std::pair<int, char *>> messages_in_progress_map;

  // private setters used only on constructor to parse fields accordingly
  void setContent(std::string content);
  void setUsername(std::string username);

public:
  Message(std::string content, std::string username,
          std::time_t timestamp = std::time(nullptr));

  // send sends the message to a file descriptor, it returns true if the data
  // was completly sent correctly, and file if there is still data to be sent
  bool send(int socket_fd);

  // recv creates a new message coming from a file descriptor, with an EOF flag.
  // Returns nullptr if the data transfer is still in progress and a complete
  // message if everything completed.
  static Message *recv(int socket_fd, bool &eof);

  // getters:

  const std::string getContent(void);
  const std::string getUsername(void);
  std::time_t getTimestamp(void);
};
