#include "client.hpp"
#include "utils.h"

#include <ctime>
#include <thread>

int main(void) {
  std::string username;
  std::cin >> username;

  auto c = Client(username, "127.0.0.1", 8080);

  auto reader_t = std::thread([&c]() {
    while (true) {
      auto *m = c.getMessage();

      char timestamp_c[9];
      time_t timestamp = m->getTimestamp();

      std::strftime(timestamp_c, sizeof(timestamp_c), "%H:%M:%S",
                    std::gmtime(&timestamp));

      std::cout << "[" << timestamp_c << "] " << m->getUsername()
                << " said: " << m->getContent() << std::endl;
      delete m;
    }
  });

  std::string content;
  std::getline(std::cin, content, '\n'); // discard initial \n from username

  while (!std::getline(std::cin, content, '\n').eof()) {
    if (content == ":q") {
      break;
    }
    c.sendMessage(content);
  }

  exit(0);
}
