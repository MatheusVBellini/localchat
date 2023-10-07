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
    refresh();                            // atualiza conteudos da tela

    // le uma string em um ponto especificado da tela
    char msg[1024];
    echo();
    mvscanw(y/2,x/2, msg);
    noecho();

    getch();

    // fecha tela
    endwin();

}