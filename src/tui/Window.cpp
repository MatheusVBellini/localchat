#include "../../include/tui/Window.hpp"

WINDOW* new_window_box(int y_beg, int x_beg, int y_max, int x_max) {
    
    WINDOW* window = newwin(y_max - y_beg, x_max - x_beg, y_beg, x_beg);
    refresh();
    return window;
}

ChatElement::ChatElement(std::string element_title)
    : element_title { element_title } {
    getmaxyx(stdscr, y_max, x_max);
    getbegyx(stdscr, y_min, x_min); 
}

auto ChatElement::clear() -> void {
    std::string clearBuffer(wwidth * wheight, ' ');
    mvwprintw(win, 0, 0, "%s" ,clearBuffer.c_str());
    box(win, 0, 0);
    mvwprintw(win, 0, 0, "%s", element_title.c_str());
    wrefresh(win);
}

auto ChatElement::initWindow() -> void {
    win = new_window_box(y_beg, x_beg, y_end, x_end);
    std::cout << y_beg << x_beg << y_end << x_end;
    clear();
}

auto ChatElement::write(std::string text) -> void { 
    // Clears what was on the element
    clear();
    // Writes new content
    mvwprintw(win, 1, 1, "%s", text.c_str());
}

Header::Header(): ChatElement("Chat") {
    computeBounds();
}

auto Header::computeBounds() -> void {
    int header_w = (x_max - x_min) - 5, 
        header_h = 3,
        header_x_min = x_min + 2,
        header_y_min = y_min + 1,
        header_x_max = header_x_min + header_w,
        header_y_max = header_y_min + header_h;

    this->y_beg = header_y_min;
    this->x_beg = header_x_min;
    this->y_end = header_y_max;
    this->x_end = header_x_max;

    this->wwidth = header_w;
    this->wheight = header_h;
}

ChatBox::ChatBox(): ChatElement("Messages") {
    computeBounds();
}

auto ChatBox::computeBounds() -> void {
    int chat_box_w = (x_max - x_min) - 5, 
        chat_box_h = (y_max - y_min) - 10,
        chat_box_x_min = x_min + 2,
        chat_box_y_min = y_min + 4, //header_y_max
        chat_box_x_max = chat_box_x_min + chat_box_w,
        chat_box_y_max = chat_box_y_min + chat_box_h;
    
    this->y_beg = chat_box_y_min;
    this->x_beg = chat_box_x_min;
    this->y_end = chat_box_y_max;
    this->x_end = chat_box_x_max;

    this->wwidth = chat_box_w;
    this->wheight = chat_box_h;
}

void ChatBox::write(std::string text) {
    if(line_num < (y_max - y_beg - 1)) {
        mvwprintw(this->win, line_num++, 1, "%s", text.c_str());
    } else {
        std::string buffer;
        buffer.resize((x_max - x_beg) - 2 ,' ');
        for(int i = 1; i < y_max; i++) {
            mvwprintw(this->win, i, 1, "%s", buffer.c_str());    
        }
            
        box(this->win, 0, 0);
        mvwprintw(this->win, 0, 0, "%s", this->element_title.c_str());
        this->line_num = 1;
    }
    wrefresh(this->win);
}

MsgeBox::MsgeBox(): ChatElement("Write a new message") {
    computeBounds();
}

auto MsgeBox::computeBounds() -> void {
   int msg_box_w = (x_max - x_min) - 5, 
        msg_box_h = 3,
        msg_box_x_min = x_min + 2,
        msg_box_y_min = y_max - 5, //chat_box_y_max + 1,
        msg_box_x_max = msg_box_x_min + msg_box_w,
        msg_box_y_max = msg_box_y_min + msg_box_h;

    this->y_beg = msg_box_y_min;
    this->x_beg = msg_box_x_min;
    this->y_end = msg_box_y_max;
    this->x_end = msg_box_x_max;
    
    this->wwidth = msg_box_w;
    this->wheight = msg_box_h;
}



auto MsgeBox::receiveInput() -> char {
    return wgetch(this->win); 
}

auto MsgeBox::acceptInput(char input) -> void {
    if(input != (char) KEY_BACKSPACE) {
        input_buffer += input; // Increase Buffer

    } else if(input_buffer.size() >= 1) {
         input_buffer.resize(input_buffer.size() - 1); // Decrease Buffer
    } 
}
    
auto MsgeBox::isCloseCommand() -> bool {
     return input_buffer == ":q";
}
    
auto MsgeBox::writeInput() -> void {
    this->write(input_buffer);
}

auto MsgeBox::clearInput() -> void {
    input_buffer.resize(0); 
}

auto MsgeBox::getInputBuffer() -> std::string {
    return input_buffer;
}
