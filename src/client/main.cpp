#include "client.hpp"
#include "utils.h"

int main(void) {
  auto c = Client("me", "127.0.0.1", 8080);

  std::cout << c.getMessage() << std::endl;

  c.sendMessage("hi server");
}
