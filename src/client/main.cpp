#include "client.hpp"
#include "utils.h"
#include "tui/Window.hpp"

#include <ctime>
#include <sstream>
#include <thread>

int main(void) {
    std::string username;
    std::cin >> username;

    auto c = Client(username, "127.0.0.1", 8080);

    initscr();
//    cbreak();
//    raw();
    noecho();

    auto header = Header();
    header.initWindow();

    auto chatBox = ChatBox();
    chatBox.initWindow();

    auto msgeBox = MsgeBox();
    msgeBox.initWindow();

    keypad(msgeBox.win, true);

    auto reader_t = std::thread([&c, &chatBox]() {
    while (true) {
      auto *m = c.getMessage();

      char timestamp_c[9];
      time_t timestamp = m->getTimestamp();

      std::strftime(timestamp_c, sizeof(timestamp_c), "%H:%M:%S",
                    std::localtime(&timestamp));

      std::stringstream messagestream;
      messagestream << "[" << timestamp_c << "] " << m->getUsername();
      chatBox.write(messagestream.str());
      std::string message;
      message += " said:    " + m->getContent();
      chatBox.write(message);
      delete m;
    }
  });

    while(true) {
      int input = (int) msgeBox.receiveInput();
        
       while(input != 10) { // ENTER CODE
         msgeBox.acceptInput((char) input);
         msgeBox.writeInput(); 
          input = (int) msgeBox.receiveInput();
        }
        
        if(msgeBox.isCloseCommand()) {
            break;
        }
        
        c.sendMessage(msgeBox.getInputBuffer().c_str());
        msgeBox.clearInput();
        msgeBox.writeInput();
    }

    endwin();
    exit(0);
}
