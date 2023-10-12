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
    cbreak();
    raw();
    noecho();
    int y_max,x_max; getmaxyx(stdscr, y_max, x_max);
    int y_min, x_min; getbegyx(stdscr, y_min, x_min);

    // parametros do header
    int header_w = (x_max - x_min) - 5, 
        header_h = 3,
        header_x_min = x_min + 2,
        header_y_min = y_min + 1,
        header_x_max = header_x_min + header_w,
        header_y_max = header_y_min + header_h;
    
    auto header = Header(header_y_min, header_x_min, header_y_max, header_x_max);

    // parametros do chat box
    int chat_box_w = (x_max - x_min) - 5, 
        chat_box_h = (y_max - y_min) - 10,
        chat_box_x_min = x_min + 2,
        chat_box_y_min = header_y_max,
        chat_box_x_max = chat_box_x_min + chat_box_w,
        chat_box_y_max = chat_box_y_min + chat_box_h;
    
    auto chatBox = ChatBox(chat_box_y_min, chat_box_x_min, chat_box_y_max, chat_box_x_max);


    // parametros do msg box
    int msg_box_w = (x_max - x_min) - 5, 
        msg_box_h = 3,
        msg_box_x_min = x_min + 2,
        msg_box_y_min = chat_box_y_max + 1,
        msg_box_x_max = msg_box_x_min + msg_box_w,
        msg_box_y_max = msg_box_y_min + msg_box_h;
   
    auto msgeBox = MsgeBox(msg_box_y_min, msg_box_x_min, msg_box_y_max, msg_box_x_max);
    
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
        int chara = wgetch(msgeBox.win);
        std::string str;
        str.resize(0);
        std::string buffer;
        buffer.resize(msg_box_w, ' ');

        while(chara != 10) {
            if(chara != KEY_BACKSPACE) {
                str += (char) chara;
            } else if('\n' == (char) chara) {
            } else if(str.size() >= 1) {
                str.resize(str.size() - 1);
            }
            msgeBox.write(buffer);
            msgeBox.clear();
            msgeBox.write(str); 
            chara = wgetch(msgeBox.win); 
        }
        
        if(str == ":q") {
            break;
        }
     
        c.sendMessage(str.c_str());
        msgeBox.write(buffer);
        msgeBox.clear();
    }

    c.~Client();
    endwin();
    exit(0);
}
