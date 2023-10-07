#include "client.hpp"
#include "utils.h"

#include <thread>

int main(void) {
  auto c = Client("me", "127.0.0.1", 8080);

  auto reader_t = std::thread([&c]() {
    while (true) {
      std::cout << c.getMessage() << std::endl;
    }
  });

  std::string content;
  while(!std::getline(std::cin, content, '\n').eof()) {
    if(content == ":q") {
      break;
    }
    c.sendMessage(content);
  }

  exit(0);
}
