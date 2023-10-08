#include "../../include/tui/Window.hpp"


Window::Window() {

    // inicializa a tela com valores padrao
    initscr();
    noecho();
    raw();

    // adquire tamanho da tela
    int y_max,x_max; getmaxyx(stdscr, y_max, x_max);
    int y_min, x_min; getbegyx(stdscr, y_min, x_min);

    // parametros do header
    int header_w = (x_max - x_min) - 5, 
        header_h = 3,
        header_x_min = x_min + 2,
        header_y_min = y_min + 1,
        header_x_max = header_x_min + header_w,
        header_y_max = header_y_min + header_h;

    // parametros do chat box
    int chat_box_w = (x_max - x_min) - 5, 
        chat_box_h = (y_max - y_min) - 10,
        chat_box_x_min = x_min + 2,
        chat_box_y_min = header_y_max,
        chat_box_x_max = chat_box_x_min + chat_box_w,
        chat_box_y_max = chat_box_y_min + chat_box_h;

    // parametros do msg box
    int msg_box_w = (x_max - x_min) - 5, 
        msg_box_h = 3,
        msg_box_x_min = x_min + 2,
        msg_box_y_min = chat_box_y_max + 1,
        msg_box_x_max = msg_box_x_min + msg_box_w,
        msg_box_y_max = msg_box_y_min + msg_box_h;

    // cria sub-janelas
    header   = layoutHeader(header_y_min, header_x_min, header_y_max, header_x_max);
    chat_box = layoutChatBox(chat_box_y_min, chat_box_x_min, chat_box_y_max, chat_box_x_max);
    msg_box  = layoutMsgBox(msg_box_y_min, msg_box_x_min, msg_box_y_max, msg_box_x_max);

}


Window::~Window() {
    endWin();
}

WINDOW* Window::layoutHeader(int y_beg, int x_beg, int y_max, int x_max) {

    WINDOW* win = newwin(y_max - y_beg, x_max - x_beg, y_beg, x_beg);

    box(win,0,0);                                  // enquadra o header
    mvwprintw(win, 0, 0, "Chat");                  // escreve o titulo do header

    return win;

}

WINDOW* Window::layoutChatBox(int y_beg, int x_beg, int y_max, int x_max) {

    WINDOW* win = newwin(y_max - y_beg, x_max - x_beg, y_beg, x_beg);

    box(win,0,0);                                  // enquadra o chat box
    mvwprintw(win, 0, 0, "Messages");              // escreve o titulo do chat box

    return win;

}

WINDOW* Window::layoutMsgBox(int y_beg, int x_beg, int y_max, int x_max) {

    WINDOW* win = newwin(y_max - y_beg, x_max - x_beg, y_beg, x_beg);

    box(win,0,0);                                  // enquadra o msg box
    mvwprintw(win, 0, 0, "Write a new message");   // escreve o titulo do msg box

    return win;

}

void Window::endWin() {
    delwin(header);
    delwin(chat_box);
    delwin(msg_box);
    endwin();
}

void Window::refresh() {
    refresh();
    //wrefresh(header);
    //wrefresh(chat_box);
    //wrefresh(msg_box);
}