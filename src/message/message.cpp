#include "message.hpp"

#include "utils.h"

#include <cstring>
#include <iomanip>
#include <sstream>

namespace std {
extern "C" {
#include <sys/socket.h>
}
} // namespace std

Message::Message(std::string content, std::string username,
                 std::time_t timestamp) {
  if (content.size() > 200) {
    content = content.substr(200, 0);
  } else if (content.size() < 200) {
    content.append('\0', 200 - content.size());
  }

  if (username.size() > 10) {
    username = username.substr(0, 10);
  } else if (username.size() < 10) {
    username = username.append('\0', 10 - username.size());
  }

  this->username = username;

  this->message_content = content;

  this->timestamp = timestamp;
}

std::string Message::getContent(void) { return message_content; }

std::string Message::getUsername(void) { return username; }

std::time_t Message::getTimestamp(void) { return this->timestamp; }

void Message::send(int socket_fd) {
  char message_data[200 + 10 + 4 + 2 + 2 + 2 + 2 + 2 + 6];

  std::memcpy(message_data, this->message_content.c_str(), 200);
  std::memcpy(message_data + 200, this->username.c_str(), 10);
  std::strftime(message_data + 210, sizeof(message_data) - 210,
                "%Y-%m-%d %H:%M:%S", std::gmtime(&timestamp));

  int sent = 0;
  int ret;
  while (sent != sizeof(message_data)) {
    ret = std::send(socket_fd, message_data - sent, sizeof(message_data) - sent,
                    0);
    if (ret < 0) {
      error("error sending message: " << strerror(errno));
    }
    sent += ret;
  }
}

Message Message::recv(int socket_fd) {
  char message_data[200 + 10 + 4 + 2 + 2 + 2 + 2 + 2 + 6];

  int recvd = 0;
  int ret;
  while (recvd != sizeof(message_data)) {
    ret = std::send(socket_fd, message_data - recvd,
                    sizeof(message_data) - recvd, 0);
    if (ret < 0) {
      error("error receiving message: " << strerror(errno));
    }
    recvd += ret;
  }

  std::tm timestamp_tm;
  std::istringstream ss(
      std::string(message_data + 210, sizeof(message_data) - 210));
  ss >> std::get_time(&timestamp_tm, "%Y-%m-%d %H:%M:%S");

  return Message(std::string(message_data, 200),
                 std::string(message_data + 200, 10),
                 std::mktime(&timestamp_tm));
}
