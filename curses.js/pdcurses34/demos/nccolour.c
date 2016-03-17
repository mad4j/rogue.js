/* nccolour.c - Print out some ncurses colours.
 * 
 * Compile using:
 *   gcc nccolour.c -o nccolour -lncurses
 *
 * Written by Yoran Heling <projects@yorhel.nl>
 *
 *    Date: 2011-06-11
 * License: MIT
 *     Web: http://dev.yorhel.nl/dump/nccolour
 */

#include <stdlib.h>
#include <curses.h>


struct colour {
  int code; char *desc;
} colours[] = {
  { -1,            "default" },
  { COLOR_BLACK,   "BLACK" },
  { COLOR_RED,     "RED" },
  { COLOR_GREEN,   "GREEN" },
  { COLOR_YELLOW,  "YELLOW" },
  { COLOR_BLUE,    "BLUE" },
  { COLOR_MAGENTA, "MAGENTA" },
  { COLOR_CYAN,    "CYAN" },
  { COLOR_WHITE,   "WHITE" },
};


void print_colourpart(int attr, int i) {
  addstr("    ");
  attron(attr | COLOR_PAIR(i*6+1));
  addstr("FD");
  addstr("  ");
  attron(attr | COLOR_PAIR(i*6+2));
  addstr("FB");
  attroff(attr | COLOR_PAIR(i*6+2));
  addstr("  ");
  attron(attr | COLOR_PAIR(i*6+3));
  addstr("FW");
  attroff(attr | COLOR_PAIR(i*6+3));
  addstr("  ");
  attron(attr | COLOR_PAIR(i*6+4));
  addstr("BG");
  attroff(attr | COLOR_PAIR(i*6+4));
  addstr("  ");
  attron(attr | COLOR_PAIR(i*6+5));
  addstr("BD");
  attroff(attr | COLOR_PAIR(i*6+5));
}


int main(int argc, char **argv) {
  if(!initscr()) {
    printf("Error initializing screen.\n");
    exit(1);
  }
  if(!has_colors()) {
    printf("This terminal does not support colours.\n");
    exit(1);
  }
  start_color();

  // this prevents ncurses from forcing a white-on-black colour scheme,
  // regardless of what scheme is used by the terminal.
  use_default_colors();

  attron(A_BOLD);
  mvaddstr(0, 0, "Colour      FD  FB  FW  BG  BD    BFD BFB BFW BBG BBN");
  attroff(A_BOLD);

  int i;
  for(i=0; i<sizeof(colours)/sizeof(struct colour); i++) {
    init_pair(i*6+1, colours[i].code, -1);
    init_pair(i*6+2, colours[i].code, COLOR_BLACK);
    init_pair(i*6+3, colours[i].code, COLOR_WHITE);
    init_pair(i*6+4, colours[i].code, colours[i].code);
    init_pair(i*6+5, -1, colours[i].code);
    mvaddstr(i+2, 1, colours[i].desc);
    move(i+2, 8);
    print_colourpart(0, i);
    print_colourpart(A_BOLD, i);
  }

  mvaddstr(i+3, 2, "FD  = Front colour on default background");
  mvaddstr(i+4, 2, "FB  = Front colour on black background");
  mvaddstr(i+5, 2, "FW  = Front colour on white background");
  mvaddstr(i+6, 2, "BG  = Front and background colour");
  mvaddstr(i+7, 2, "BD  = Background colour with default front colour");
  mvaddstr(i+8, 2, "B?? = As above, with A_BOLD enabled");
  mvaddstr(i+10, 0, "Hit any key to exit.");
  refresh();
  getch();
  endwin();
  return 0;
}
