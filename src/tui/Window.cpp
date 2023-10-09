#include "../../include/tui/Window.hpp"

WINDOW* new_window_box(int y_beg, int x_beg, int y_max, int x_max, std::string win_name) {
    
    WINDOW* window = newwin(y_max - y_beg, x_max - x_beg, y_beg, x_beg);
    refresh();
    box(window, 0, 0);// enquadra o header
    mvwprintw(window, 0, 0, "%s", win_name.c_str());                  // escreve o titulo do header
    wrefresh(window);
    return window;
}

Window::Window(int y_beg, int x_beg, int y_max, int x_max, std::string win_name)
    : y_beg{ y_beg }, x_beg{ x_beg }, y_max{ y_max }, x_max{ x_max }, win_name { win_name } {
    this->win = new_window_box(y_beg, x_beg, y_max, x_max, this->win_name);
}

void Window::refresh() {
    wrefresh(this->win);
}

void Window::write(std::string text) {
    mvwprintw(this->win, 1, 1, "%s", text.c_str());
}

void Window::clear() {
    box(this->win, 0, 0);
    
    mvwprintw(this->win, 0, 0, "%s", this->win_name.c_str());
    wrefresh(this->win);
}
