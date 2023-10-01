#include "Message.h"

#include "utils.h"

#include <cstring>
#include <iomanip>
#include <sstream>

namespace std {
extern "C" {
#include <sys/socket.h>
}
} // namespace std

std::map<int, std::pair<int, char *>> Message::messages_in_progress_map;

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

const std::string Message::getContent(void) { return message_content; }

const std::string Message::getUsername(void) { return username; }

std::time_t Message::getTimestamp(void) { return this->timestamp; }

bool Message::send(int socket_fd) {
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
        return false;
      }

      error("error sending message: " << strerror(errno));
    }

    sent += ret;
  }

  debug("sent message correctly");
  return true;
}

Message *Message::recv(int socket_fd, bool &eof) {
  eof = false;
  auto &recv_message = messages_in_progress_map[socket_fd];
  if (recv_message.second == nullptr) {
    messages_in_progress_map[socket_fd] = {0, new char[MESSAGE_SIZE]};
    recv_message = messages_in_progress_map[socket_fd];
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

      info("error receiving message: " << strerror(errno));

      delete[] recv_message.second;
      messages_in_progress_map.erase(socket_fd);
      return nullptr;
    }
    if (ret == 0) {
      if (recv_message.first == 0) {
        eof = true;
        delete[] recv_message.second;
        messages_in_progress_map.erase(socket_fd);
        return nullptr;
      }

      info("got unexpected EOF while receiving message");

      delete[] recv_message.second;
      messages_in_progress_map.erase(socket_fd);
      return nullptr;
    }

    recv_message.first += ret;
  }

  debug("got message correctly, with raw data " << recv_message.second);

  std::tm timestamp_tm;
  std::istringstream ss(
      std::string(recv_message.second + 210, MESSAGE_SIZE - 210));
  ss >> std::get_time(&timestamp_tm, "%Y-%m-%d %H:%M:%S");
  if (!ss) {
    error("invalid timestamp received");
  }

  Message *m = new Message(std::string(recv_message.second, 200),
                           std::string(recv_message.second + 200, 10),
                           std::mktime(&timestamp_tm));

  delete[] recv_message.second;
  messages_in_progress_map.erase(socket_fd);

  return m;
}
