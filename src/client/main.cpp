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


      switch (m->getType()) {
      case MESSAGE_CHAT:
        messagestream << " said: " << m->getContent();
        break;
      case MESSAGE_CONNECT:
        messagestream << " connected";
        break;
      case MESSAGE_DISCONNECT:
        messagestream << " disconnected";
        break;
      default:
        messagestream << " sent an invalid message";
        break;
      }


      chatBox.write(messagestream.str().c_str());
      chatBox.refresh();

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

    c.~Client();

    endwin();
    exit(0);
}
