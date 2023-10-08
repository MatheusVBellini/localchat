#pragma once

#include <ncurses.h>
#include <iostream>

class Window {
private:
    WINDOW* header;
    WINDOW* chat_box;
    WINDOW* msg_box;

    WINDOW* layoutHeader(int y_beg, int x_beg, int y_max, int x_max);  // retorna o header com seu formato padrao
    WINDOW* layoutChatBox(int y_beg, int x_beg, int y_max, int x_max); // retorna o chat box com seu formato padrao
    WINDOW* layoutMsgBox(int y_beg, int x_beg, int y_max, int x_max);  // retorna o msg box com seu formato padrao

public:
    Window();
    ~Window();

    void endWin();  // fecha a tela
    void refresh(); // atualiza a tela
};