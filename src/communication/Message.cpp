#include "Message.h"

#include "utils.h"

#include <cstring>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace std {
extern "C" {
#include <sys/socket.h>
}
} // namespace std

std::map<int, std::pair<int, char *>> Message::messages_in_progress_map;

Message::Message(std::string username, std::string content,
                 std::time_t timestamp) {
  this->setUsername(username);
  this->setContent(content);
  this->timestamp = timestamp;
  this->type = MESSAGE_CHAT;
}

Message::Message(std::string username, bool connection_initiated,
                 std::time_t timestamp) {
  this->setUsername(username);
  this->type = (connection_initiated) ? MESSAGE_CONNECT : MESSAGE_DISCONNECT;
  this->timestamp = timestamp;
}

// getters
const std::string Message::getContent(void) {
  if (type != MESSAGE_CHAT) {
    throw std::invalid_argument("message must be of type chat for operation");
  }

  return message_content.substr(0, message_content.find('\0'));
}

const std::string Message::getUsername(void) {
  return username.substr(0, username.find('\0'));
}

std::time_t Message::getTimestamp(void) { return this->timestamp; }

// private setters
void Message::setContent(std::string content) {
  if (content.size() > 200) {
    content = content.substr(200, 0);
  } else if (content.size() < 200) {
    content.append(200 - content.size(), '\0');
  }

  this->message_content = content;
}

void Message::setUsername(std::string username) {
  if (username.size() > 10) {
    username = username.substr(0, 10);
  } else if (username.size() < 10) {
    username = username.append(10 - username.size(), '\0');
  }

  this->username = username;
}

uint8_t Message::getType(void) { return type; }

bool Message::send(int socket_fd) {
  char message_data[MESSAGE_SIZE_MAX];
  int size_to_send;

  uint64_t timestamp_ull = this->timestamp;

  switch (type) {
  case MESSAGE_CHAT:
    std::memcpy(message_data, &this->type, 1);
    std::memcpy(message_data + 1, this->message_content.c_str(), 200);
    std::memcpy(message_data + 201, this->username.c_str(), 10);
    std::memcpy(message_data + 211, &timestamp_ull, 8);
    size_to_send = MESSAGE_SIZE_CHAT;

    debug("sending chat message with content " << message_content);

    break;
  case MESSAGE_CONNECT:
  case MESSAGE_DISCONNECT:
    std::memcpy(message_data, &this->type, 1);
    std::memcpy(message_data + 1, this->username.c_str(), 10);
    std::memcpy(message_data + 11, &timestamp_ull, 8);
    size_to_send = MESSAGE_SIZE_CONN;

    if (type == MESSAGE_CONNECT) {
      debug("sending connect message");
    } else {
      debug("sending disconnect message");
    }

    break;
  default:
    throw std::invalid_argument("message must have a type to be sent");
  }

  int sent = sending_positions[socket_fd];
  int ret;
  while (sent < size_to_send) {
    ret = std::send(socket_fd, message_data - sent, sizeof(message_data) - sent,
                    0);

    if (ret < 0) {
      if (errno == EWOULDBLOCK || errno == EINPROGRESS) {
        sending_positions[socket_fd] = sent;
        return false;
      }

      error("error sending message: " << strerror(errno));
    }

    sent += ret;
  }

  sending_positions.erase(socket_fd);

  debug("sent message correctly");
  return true;
}

Message *Message::recv(int socket_fd, bool &eof) {
  eof = false;
  auto &recv_message = messages_in_progress_map[socket_fd];
  if (recv_message.second == nullptr) {
    messages_in_progress_map[socket_fd] = {0, new char[MESSAGE_SIZE_MAX]};
    recv_message = messages_in_progress_map[socket_fd];
  }

  debug("getting message from " << socket_fd);

  int ret;
  while (recv_message.first != MESSAGE_SIZE_MAX) {
    ret = std::recv(socket_fd, recv_message.second + recv_message.first,
                    MESSAGE_SIZE_MAX - recv_message.first, 0);
    if (ret < 0) {
      if (errno == EWOULDBLOCK || errno == EINPROGRESS) {
        debug("message would block, retuning to main loop");
        return nullptr;
      }

      info("error receiving message: " << strerror(errno));

      delete[] recv_message.second;
      messages_in_progress_map.erase(socket_fd);
      return nullptr;
    }
    if (ret == 0) {
      if (recv_message.first == 0) {
        debug("message is EOF, setting eof flag");

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

  debug("completed message");

  uint64_t timestamp_ull;
  Message *m;

  switch (recv_message.second[0]) {
  case MESSAGE_CHAT:
    std::memcpy(&timestamp_ull, recv_message.second + 211, 8);
    m = new Message(std::string(recv_message.second + 201, 10),
                    std::string(recv_message.second + 1, 200), timestamp_ull);
    debug("message was of type chat");

    break;
  case MESSAGE_CONNECT:
  case MESSAGE_DISCONNECT:
    std::memcpy(&timestamp_ull, recv_message.second + 11, 8);
    m = new Message(std::string(recv_message.second + 1, 10),
                    recv_message.second[0] == MESSAGE_CONNECT, timestamp_ull);

    if (recv_message.second[0] == MESSAGE_CONNECT) {
      debug("message was of type connect");
    } else {
      debug("message was of type disconnect");
    }

    break;
  default:
    info("invalid message type received");
    m = nullptr;
  }

  delete[] recv_message.second;
  messages_in_progress_map.erase(socket_fd);

  return m;
}
