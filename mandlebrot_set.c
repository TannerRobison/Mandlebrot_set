#include <stdio.h>
#include <stddef.h>
#include <curses.h>
#include <unistd.h>

int get_next_frame();

int main() {
    initscr();
    noecho(); //dont echo user input
    cbreak(); //dont buffer user input

    start_color();

    int max_x;
    int max_y;
    getmaxyx(stdscr, max_y, max_x);

    int y = max_y * 0.5;
    int x = (max_x * 0.5) - 6;

    mvwprintw(stdscr, y, x, "Hello world");
    
    refresh();

    attroff(1);
    attroff(2);
    getch();
    endwin();

    return 0;
}
