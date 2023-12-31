#include <ncurses.h>

int main() {
    
    // inicialização
    initscr();            // inicia a tela
    raw();                // não espera enter para receber inputs do teclado
    noecho();             // não mostra o que é digitado
    keypad(stdscr, TRUE); // permite usar teclas especiais como F1, F2, etc

    // adquire tamanho da tela
    int y, x; getmaxyx(stdscr, y, x);



    // mostra conteudo na tela
    printw("Hello World !!!");
    mvprintw(5,5, "Byebye World !!!");
    mvhline(1,1, '-', x-2);               // desenha uma linha horizontal
    mvvline(1,1, '|', y-3);               // desenha uma linha vertical
    refresh();                            // atualiza conteudos da tela

    // cria uma prompt na tela usando um sub-tela
    WINDOW *win = newwin(3, 20, y/2 -1, x/2 - 8);
    box(win, 0, 0);
    mvwprintw(win, 1, 2, "Text:");
    wrefresh(win);

    // le uma string em um ponto especificado da tela
    char msg[1024];
    echo();
    mvscanw(y/2,x/2, msg);
    noecho();

    refresh();


    getch();

    // fecha tela
    delwin(win);
    endwin();
    return 0;

}