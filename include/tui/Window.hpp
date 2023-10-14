#pragma once

#include <ncurses.h>
#include <iostream>


#include <string>

/* Function to create a window in ncurses */
WINDOW* new_window_box(int y_beg, int x_beg, int y_max, int x_max, std::string win_name);

class ChatElement {
public:
    WINDOW* win;
    std::string element_title;
    int y_max, x_max, y_min, x_min;
    int y_beg, x_beg, y_end, x_end;
    int wwidth, wheight;
    
    ChatElement(std::string element_title);

    virtual auto computeBounds() -> void {}
    auto clear() -> void;
    auto initWindow() -> void; 
    virtual auto write(std::string text) -> void;
};

/* Header of the Chat */
class Header : public ChatElement{
public:
    Header();

    virtual auto computeBounds() -> void override;
};

/* Body of the Chat (The chat box) */
class ChatBox : public ChatElement {
public:
    int line_num=1;

    ChatBox();

    virtual auto computeBounds() -> void override;
    
    auto write(std::string text) -> void override; 
};

/* The box to input message */
class MsgeBox : public ChatElement {
    std::string input_buffer;
public:
    MsgeBox();
    
    virtual auto computeBounds() -> void override;

    auto receiveInput() -> char;
    auto acceptInput(char input) -> void;
    auto isCloseCommand() -> bool;
    auto writeInput() -> void;
    auto clearInput() -> void;
    auto getInputBuffer() -> std::string;
};
