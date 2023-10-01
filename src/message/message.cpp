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

std::map<int, std::pair<int, char*>> Message::messages_in_progress_map;

Message::Message(std::string content, std::string username,
                 std::time_t timestamp) {
  if (content.size() > 200) {
    content = content.substr(200, 0);
  } else if (content.size() < 200) {
    content.append(200 - content.size(), '\0');
  }

  if (username.size() > 10) {
    username = username.substr(0, 10);
  } else if (username.size() < 10) {
    username = username.append(10 - username.size(), '\0');
  }

  this->username = username;

  this->message_content = content;

  this->timestamp = timestamp;
}

std::string Message::getContent(void) { return message_content; }

std::string Message::getUsername(void) { return username; }

std::time_t Message::getTimestamp(void) { return this->timestamp; }

void Message::send(int socket_fd) {
  char message_data[MESSAGE_SIZE];

  std::memcpy(message_data, this->message_content.c_str(), 200);
  std::memcpy(message_data + 200, this->username.c_str(), 10);
  std::strftime(message_data + 210, sizeof(message_data) - 210,
                "%Y-%m-%d %H:%M:%S", std::gmtime(&timestamp));

  debug("sending message with raw data " << message_data);

  int sent = 0;
  int ret;
  while (sent != sizeof(message_data)) {
    ret = std::send(socket_fd, message_data - sent, sizeof(message_data) - sent,
                    0);

    if (ret <= 0) {
      if (errno == EWOULDBLOCK || errno == EINPROGRESS) {
        continue;
      }

      error("error sending message: " << strerror(errno));
    }

    sent += ret;
  }

  debug("sent message correctly");
}

Message *Message::recv(int socket_fd) {
  auto& recv_message = messages_in_progress_map[socket_fd];
  if(recv_message.second == nullptr){
    recv_message = {0, new char[MESSAGE_SIZE]};
    messages_in_progress_map[socket_fd] = recv_message;
  }

  debug("getting message");

  int ret;
  while (recv_message.first != MESSAGE_SIZE) {
    ret = std::recv(socket_fd, recv_message.second + recv_message.first,
                    MESSAGE_SIZE - recv_message.first, 0);
    if (ret < 0) {
      if (errno == EWOULDBLOCK || errno == EINPROGRESS) {
        return nullptr;
      }

      error("error receiving message: " << strerror(errno));
    }

    if (ret == 0) {
      error("unexpected EOF");
    }

    recv_message.first += ret;
  }

  messages_in_progress_map.erase(socket_fd);

  debug("got message correctly, with raw data " << recv_message.second);

  std::tm timestamp_tm;
  std::istringstream ss(
      std::string(recv_message.second + 210, MESSAGE_SIZE - 210));
  ss >> std::get_time(&timestamp_tm, "%Y-%m-%d %H:%M:%S");

  return new Message(std::string(recv_message.second, 200),
                     std::string(recv_message.second + 200, 10),
                     std::mktime(&timestamp_tm));
}
