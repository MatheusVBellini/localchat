#pragma once

#include <ncurses.h>
#include <iostream>


#include <string>

WINDOW* new_window_box(int y_beg, int x_beg, int y_max, int x_max, std::string win_name);

class Window {
public:
    WINDOW* win;
    std::string win_name;
    int y_beg, x_beg;
    int y_max, x_max;

    Window(int y_beg, int x_beg, int y_max, int x_max, std::string win_name);

    void write(std::string text);
    void clear();
    void refresh(); // atualiza a tela
};

class Header : public Window {
public:
    Header(int y_beg, int x_beg, int y_max, int x_max)
        : Window(y_beg, x_beg, y_max, x_max, std::string("Chat")) {}
};

class ChatBox : public Window {
public:
    int line_num=1;

    ChatBox(int y_beg, int x_beg, int y_max, int x_max)
        : Window(y_beg, x_beg, y_max, x_max, std::string("Messages")) {}

    void write(std::string text) {
        if(line_num < (y_max - y_beg - 1)) {
            mvwprintw(this->win, line_num++, 1, "%s", text.c_str());
        } else {
            std::string buffer;
            buffer.resize((x_max - x_beg) - 2 ,' ');
            for(int i = 1; i < y_max; i++) {
                mvwprintw(this->win, i, 1, "%s", buffer.c_str());    
            }
            
            box(this->win, 0, 0);
            mvwprintw(this->win, 0, 0, "%s", this->win_name.c_str());
            this->line_num = 1;
        }
    }
};

class MsgeBox : public Window {
public:
    MsgeBox(int y_beg, int x_beg, int y_max, int x_max)
        : Window(y_beg, x_beg, y_max, x_max, std::string("Write a new message")) {}
};
